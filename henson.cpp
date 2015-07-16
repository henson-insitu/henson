#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <memory>

#include <mpi.h>

#include <opts/opts.h>
#include <format.h>

#include <henson/puppet.hpp>
#include <henson/data.hpp>
#include <henson/procs.hpp>
#include <henson/hwl.hpp>
namespace h = henson;


struct CommandLine
{
    // need to disallow copy to avoid pitfalls with the argv pointers into arguments

                                CommandLine()                   =default;
                                CommandLine(const CommandLine&) =delete;
                                CommandLine(CommandLine&&)      =default;
                                CommandLine(const std::string& line)
    {
        int prev = -1, pos = 0;
        while (pos != std::string::npos)
        {
            pos = line.find(' ', pos + 1);

            arguments.push_back(std::vector<char>(line.begin() + prev + 1, pos == std::string::npos ? line.end() : line.begin() + pos));
            arguments.back().push_back('\0');

            prev = pos;
        }

        for (auto& s : arguments)
            argv.push_back(&s[0]);
    }

    CommandLine&                operator=(const CommandLine&)   =delete;
    CommandLine&                operator=(CommandLine&&)        =default;

    std::string                 executable() const              { return std::string(arguments[0].begin(), arguments[0].end()); }

    std::vector<std::vector<char>>  arguments;
    std::vector<char*>              argv;
};


int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);

    MPI_Comm world = MPI_COMM_WORLD;

    int rank, size;
    MPI_Comm_rank(world, &rank);
    MPI_Comm_size(world, &size);

    fmt::print("[{}]: henson started; total processes = {}\n", rank, size);

    using namespace opts;
    Options ops(argc, argv);

    std::string script_fn;
    if (  ops >> Present('h', "help", "show help") ||
        !(ops >> PosOption(script_fn)))
    {
        fmt::print("Usage: {} SCRIPT [procs=SIZE]+\n{}", argv[0], ops);
        return 1;
    }

    henson::ProcMap::Vector         procs;
    std::string                     procs_size;
    while(ops >> PosOption(procs_size))
    {
        int eq_pos = procs_size.find('=');
        if (eq_pos == std::string::npos)
        {
            fmt::print("Can't parse {}\n", procs_size);
            return 1;
        }
        procs.emplace_back(procs_size.substr(0, eq_pos), std::stoi(procs_size.substr(eq_pos + 1, procs_size.size() - eq_pos)));
    }

    typedef             std::unique_ptr<h::ProcMap>        ProcMapUniquePtr;
    ProcMapUniquePtr    procmap;        // splits the communicator into groups
    try
    {
        procmap = ProcMapUniquePtr(new h::ProcMap(world, procs));
    } catch (std::runtime_error& e)
    {
        fmt::print("Abort: {}\n", e.what());
        return 1;
    }
    henson::NameMap                     namemap;        // global namespace shared by the puppets

    // convert script into puppets
    // TODO: technically, we don't need to load puppets that we don't need, but I'm guessing it's a small overhead
    hwl::Script script;
    std::ifstream script_ifs(script_fn.c_str());
    parser::state ps(script_ifs);
    bool result = hwl::script(ps, script);
    if (!result)
    {
        fmt::print("Couldn't parse script: {}\n", script_fn);
        return 1;
    }

    std::string prefix = script_fn;
    if (prefix[0] != '/')
        prefix = "./" + prefix;
    prefix = prefix.substr(0, prefix.rfind('/') + 1);

    typedef     std::unique_ptr<h::Puppet>          PuppetUniquePtr;
    std::vector<CommandLine>                        command_lines;
    std::map<std::string, PuppetUniquePtr>          puppets;
    for (auto& p : script.puppets)
    {
        command_lines.emplace_back(p.command);
        auto& cmd_line = command_lines.back();
        puppets[p.name] = PuppetUniquePtr(new h::Puppet(prefix + cmd_line.executable(),
                                                        cmd_line.argv.size(),
                                                        &cmd_line.argv[0],
                                                        procmap.get(),
                                                        &namemap));
    }

    int                     group   = procmap->color();
    std::string             name    = procs[group].first;
    const hwl::ControlFlow& control = script.nodes[name];

    bool stop_execution = false;
    do
    {
        for (size_t i = 0; i < control.commands.size(); ++i)
        {
            auto& puppet = *puppets[control.commands[i]];

            puppet.proceed();

            if (i == 0 && !puppet.running())    // 0-th puppet is assumed to be the simulation; stop if its done
            {
                stop_execution = true;
                break;
            }
        }
    } while (!stop_execution);

    fmt::print("[{}]: henson done\n", rank);

    MPI_Finalize();
}

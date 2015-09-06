#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
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

            std::vector<char> arg(line.begin() + prev + 1, pos == std::string::npos ? line.end() : line.begin() + pos);
            for (char c : arg)
            {
                if (!std::isspace(c))
                {
                    arg.push_back('\0');
                    arguments.push_back(std::move(arg));
                    break;
                }
            }

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

    std::vector<std::string>    procs_sizes;
    using namespace opts;
    Options ops(argc, argv);
    ops
        >> Option('p', "procs", procs_sizes, "number of processors to use for a control group")
    ;

    bool show_sizes = ops >> Present('s', "show-sizes", "show group sizes");
    bool verbose    = ops >> Present('v', "verbose",    "verbose output");

    std::string script_fn;
    if (  ops >> Present('h', "help", "show help") ||
        !(ops >> PosOption(script_fn)))
    {
        if (rank == 0)
        {
            fmt::print("Usage: {} SCRIPT [-p group=SIZE]* [variable=value]*\n\n", argv[0]);
            fmt::print("Execute SCRIPT. procs are the names of execution groups in the script.\n");
            fmt::print("Leftover processors get split evenly among the execution groups in the SCRIPT\n");
            fmt::print("but not specified in the procs list.\n\n");
            fmt::print("{}", ops);
        }
        return 1;
    }

    // parse the script
    hwl::Script script;
    std::ifstream script_ifs(script_fn.c_str());
    std::string   content((std::istreambuf_iterator<char>(script_ifs)),
                          (std::istreambuf_iterator<char>()));
    content = hwl::indent_to_tokens(content);
    std::istringstream  content_iss(content);
    parser::state ps(content_iss);
    bool result = hwl::script(ps, script);
    if (!result)
    {
        fmt::print("Couldn't parse script: {}\n", script_fn);
        return 1;
    }

    // parse the procs
    henson::ProcMap::Vector         procs;
    int                             total_procs = 0;
    for (std::string procs_size : procs_sizes)
    {
        int eq_pos = procs_size.find('=');
        if (eq_pos == std::string::npos)
        {
            fmt::print("Can't parse {}\n", procs_size);
            return 1;
        }
        int sz = std::stoi(procs_size.substr(eq_pos + 1, procs_size.size() - eq_pos));
        procs.emplace_back(procs_size.substr(0, eq_pos), sz);
        total_procs += sz;
    }

    // parse variables
    std::map<std::string,std::string>   variables;
    std::string                         var;
    while (ops >> PosOption(var))
    {
        int eq_pos = var.find('=');
        if (eq_pos == std::string::npos)
        {
            fmt::print("Can't parse {}\n", var);
            return 1;
        }
        auto var_name  = var.substr(0, eq_pos);
        auto var_value = var.substr(eq_pos + 1, var.size() - eq_pos);
        variables[var_name] = var_value;
    }

    if (total_procs > size)
    {
        fmt::print("Specified procs exceed MPI size: {} > {}\n", total_procs, size);
        return 1;
    }

    // record assigned groups
    std::set<std::string>   assigned_procs;
    for (auto& x : procs)
        assigned_procs.insert(x.first);

    int unassigned = script.procs.size() - assigned_procs.size();

    // assign unassigned procs
    for (auto& x : script.procs)
        if (assigned_procs.find(x.first) == assigned_procs.end())
            procs.emplace_back(x.first, (size - total_procs) / unassigned);

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

    if (rank == 0 && show_sizes)
    {
        fmt::print("Group sizes:\n");
        for (auto& x : script.procs)
            fmt::print("  {} = {}\n", x.first, procmap->size(x.first));
    }


    henson::NameMap                     namemap;        // global namespace shared by the puppets

    // convert script into puppets
    // TODO: technically, we don't need to load puppets that we don't need, but I'm guessing it's a small overhead
    std::string prefix = script_fn;
    if (prefix[0] != '/' && prefix[0] != '~')
        prefix = "./" + prefix;
    prefix = prefix.substr(0, prefix.rfind('/') + 1);

    typedef     std::unique_ptr<h::Puppet>          PuppetUniquePtr;
    std::vector<CommandLine>                        command_lines;
    std::map<std::string, PuppetUniquePtr>          puppets;
    for (auto& p : script.puppets)
    {
        // parse and process command variables
        //fmt::print("Parsing command: {}\n", p.command);
        hwl::Command cmd;
        std::istringstream command_in(p.command);
        parser::state ps(command_in);
        bool result = hwl::command(ps, cmd);
        if (!result)
        {
            fmt::print("Couldn't parse command: {}\n", p.command);
            return 1;
        }
        auto cmd_expanded = cmd.generate(variables);
        //fmt::print("Command parsed and expanded: {}\n", cmd_expanded);

        command_lines.emplace_back(cmd_expanded);
        auto& cmd_line = command_lines.back();
        auto exec = cmd_line.executable();
        if (exec[0] != '/' && exec[0] != '~')
            exec = prefix + exec;
        puppets[p.name] = PuppetUniquePtr(new h::Puppet(exec,
                                                        cmd_line.argv.size(),
                                                        &cmd_line.argv[0],
                                                        procmap.get(),
                                                        &namemap));
    }

    int                     color       = procmap->color();
    std::string             group       = procs[color].first;
    int                     group_size  = procs[color].second;
    const hwl::ControlFlow& control     = script.procs[group];

    bool stop_execution = false;
    do
    {
        std::vector<std::string>    revisit;

        for (size_t i = 0; i < control.commands.size(); ++i)
        {
            std::string name = control.commands[i];
            if (name[0] == '*')
            {
                name = name.substr(1);
                revisit.push_back(name);
            }

            auto&               puppet = *puppets[name];

            if (stop_execution)
            {
                if (verbose) fmt::print("Sending stop to {}\n", name);
                puppet.signal_stop();
            }
            if (verbose) fmt::print("Proceeding with {}\n", name);
            puppet.proceed();

            if (!puppet.running() && control.control == name)
            {
                if (verbose) fmt::print("Control puppet {} stopped in {}\n", name, control.name);
                stop_execution = true;
            }
        }

        // revisit the puppets that need to finilize their execution
        for (auto& name : revisit)
        {
            if (verbose) fmt::print("Revisiting {}\n", name);
            puppets[name]->proceed();
        }
    } while (!stop_execution);

    fmt::print("[{}]: henson done\n", rank);

    MPI_Finalize();
}

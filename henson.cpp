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
namespace h = henson;


struct CommandLine
{
    // need to disallow copy to avoid pitfalls with the argv pointers into arguments

                                CommandLine()                   =default;
                                CommandLine(const CommandLine&) =delete;
                                CommandLine(CommandLine&&)      =default;

    CommandLine&                operator=(const CommandLine&)   =delete;
    CommandLine&                operator=(CommandLine&&)        =default;

    std::string                 executable() const              { return std::string(arguments[0].begin(), arguments[0].end()); }

    std::vector<std::vector<char>>  arguments;
    std::vector<char*>              argv;
};

std::vector<CommandLine>
parse_script(const std::string& filename)
{
    std::ifstream   in(filename.c_str());
    std::string     line;

    std::vector<CommandLine>    commands;

    while (std::getline(in,line))
    {
        if (line.empty() || line[0] == '#')
            continue;

        CommandLine cmd;
        int prev = -1, pos = 0;
        while (pos != std::string::npos)
        {
            pos = line.find(' ', pos + 1);

            cmd.arguments.push_back(std::vector<char>(line.begin() + prev + 1, pos == std::string::npos ? line.end() : line.begin() + pos));
            cmd.arguments.back().push_back('\0');

            prev = pos;
        }

        for (auto& s : cmd.arguments)
            cmd.argv.push_back(&s[0]);

        commands.push_back(std::move(cmd));
    }

    return commands;
}


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
        fmt::print("Usage: {} SCRIPT\n{}", argv[0], ops);
        return 1;
    }

    auto command_lines = parse_script(script_fn);

    henson::NameMap             namemap;        // global namespace shared by the puppets

    std::vector<std::unique_ptr<h::Puppet>>     puppets;
    for (CommandLine& cmd_line : command_lines)
        puppets.emplace_back(new h::Puppet(cmd_line.executable(), cmd_line.argv.size(), &cmd_line.argv[0], world, &namemap));

    bool stop_execution = false;
    do
    {
        for (size_t i = 0; i < puppets.size(); ++i)
        {
            auto& puppet = *puppets[i];

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

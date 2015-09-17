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

std::string
clock_to_seconds(h::time_type t)
{
    return fmt::format("{}.{:02}", t / 1000000000, (t % 1000000000 / 10000000));
}

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

typedef     std::unique_ptr<h::Puppet>                  PuppetUniquePtr;
typedef     std::map<std::string, PuppetUniquePtr>      Puppets;

struct Executor
{
            Executor(bool&                      stop_,
                     std::vector<std::string>&  revisit_,
                     Puppets&                   puppets_,
                     const hwl::ControlFlow&    control_,
                     bool                       verbose_):
                stop(stop_), revisit(revisit_), puppets(puppets_), control(control_), verbose(verbose_)       {}

    void    statement(hwl::Statement s)
    {
        std::string name = s.call;
        if (name[0] == '*')
        {
            name = name.substr(1);
            revisit.push_back(name);
        }

        auto& puppet = *puppets[name];

        exec(name, puppet);

        // if the statment has body, check puppet's return result, and execute, if necessary
        if (!s.body.empty())
        {
            // check result and, if necessary, execute the body
            if (puppet.result() == 0)       // NB: 0 signals normal exit
                for (hwl::Statement sb : s.body)
                    statement(sb);
        }
    }

    void    exec(std::string name, henson::Puppet& puppet)
    {
        if (stop)
        {
            if (verbose) fmt::print("Sending stop to {}\n", name);
            puppet.signal_stop();
        }
        if (verbose) fmt::print("Proceeding with {}\n", name);
        puppet.proceed();

        if (!puppet.running() && control.control == name)
        {
            if (verbose) fmt::print("Control puppet {} stopped in {}\n", name, control.name);
            stop = true;
        }
    }

    bool&                       stop;
    std::vector<std::string>&   revisit;
    Puppets&                    puppets;
    const hwl::ControlFlow&     control;
    bool                        verbose;
};


int main(int argc, char *argv[])
{
    h::time_type start_time = h::get_time();

    MPI_Init(&argc, &argv);

    MPI_Comm world = MPI_COMM_WORLD;

    int rank, size;
    MPI_Comm_rank(world, &rank);
    MPI_Comm_size(world, &size);

    std::vector<std::string>    procs_sizes;
    using namespace opts;
    Options ops(argc, argv);
    ops
        >> Option('p', "procs", procs_sizes, "number of processors to use for a control group")
    ;

    bool show_sizes = ops >> Present('s', "show-sizes", "show group sizes");
    bool verbose    = ops >> Present('v', "verbose",    "verbose output");
    bool times      = ops >> Present('t', "show-times", "show time spent in each puppet");
    bool every_iteration = ops >> Present("every-iteration", "report times at every iteration");

    if (verbose || rank == 0)
        fmt::print("[{}]: henson started; total processes = {}\n", rank, size);

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

    int                     color       = procmap->color();
    std::string             group       = procs[color].first;
    int                     group_size  = procs[color].second;
    const hwl::ControlFlow& control     = script.procs[group];

    // convert script into puppets
    // TODO: technically, we don't need to load puppets that we don't need, but I'm guessing it's a small overhead
    std::string prefix = script_fn;
    if (prefix[0] != '/' && prefix[0] != '~')
        prefix = "./" + prefix;
    prefix = prefix.substr(0, prefix.rfind('/') + 1);

    std::vector<CommandLine>                        command_lines;
    Puppets                                         puppets;
    for (auto& p : script.puppets)
    {
        if (!control.uses(p.name)) continue;

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

    // Timer reporter
    h::time_type initialization_time = h::get_time() - start_time;
    auto report_times = [&](size_t iteration)
    {
        h::time_type puppet_time = 0;
        h::time_type total_execution_time = h::get_time() - start_time;

        std::vector< std::tuple<std::string, h::time_type> >    max_puppet_times;
        for (auto& p : puppets)
        {
            if (!control.uses(p.first)) continue;
            h::time_type t = p.second->total_time();

            if (verbose)
                fmt::print("[{}]: {} took {}\n", rank, p.first, h::clock_to_string(t));

            h::time_type max_t;
            MPI_Reduce(&t, &max_t, 1, MPI_UNSIGNED_LONG_LONG, MPI_MAX, 0, procmap->local());
            max_puppet_times.emplace_back(p.first, max_t);

            puppet_time += t;
        }
        h::time_type context_switching_time = total_execution_time - puppet_time - initialization_time;

        h::time_type max_init, max_context_switching, max_puppet_time, max_total_time;
        MPI_Reduce(&initialization_time,    &max_init,              1, MPI_UNSIGNED_LONG_LONG, MPI_MAX, 0, procmap->local());
        MPI_Reduce(&context_switching_time, &max_context_switching, 1, MPI_UNSIGNED_LONG_LONG, MPI_MAX, 0, procmap->local());
        MPI_Reduce(&puppet_time,            &max_puppet_time,       1, MPI_UNSIGNED_LONG_LONG, MPI_MAX, 0, procmap->local());
        MPI_Reduce(&total_execution_time,   &max_total_time,        1, MPI_UNSIGNED_LONG_LONG, MPI_MAX, 0, procmap->local());

        if (procmap->is_leader(rank))
        {
            fmt::print("Max times (iter={}) for group {}:\n  init = {} ({}), other = {} ({}); puppet = {} ({}); total = {} ({})\n",
                       iteration, group,
                       h::clock_to_string(max_init),
                       clock_to_seconds(max_init),
                       h::clock_to_string(max_context_switching),
                       clock_to_seconds(max_context_switching),
                       h::clock_to_string(max_puppet_time),
                       clock_to_seconds(max_puppet_time),
                       h::clock_to_string(max_total_time),
                       clock_to_seconds(max_total_time));
            for (auto& x : max_puppet_times)
                fmt::print("  Max time (iter={}) for {:<12} in group {}: {} ({})\n",
                           iteration, std::get<0>(x), group,
                           h::clock_to_string(std::get<1>(x)),
                           clock_to_seconds(std::get<1>(x)));
        }

        if (verbose)
            fmt::print("[{}]: initialization = {}; other = {}; puppet time = {}; total time = {}\n",
                       rank,
                       h::clock_to_string(initialization_time),
                       h::clock_to_string(context_switching_time),
                       h::clock_to_string(puppet_time),
                       h::clock_to_string(total_execution_time));
    };

    bool                        stop_execution = false;
    std::vector<std::string>    revisit;
    Executor                    executor(stop_execution, revisit, puppets, control, verbose);
    size_t                      iteration = 0;
    do
    {
        for (hwl::Statement s : control.commands)
            executor.statement(s);

        // revisit the puppets that need to finilize their execution
        for (auto& name : revisit)
        {
            if (verbose) fmt::print("Revisiting {}\n", name);
            puppets[name]->proceed();
        }
        revisit.clear();

        if (times && every_iteration)
            report_times(iteration);

        ++iteration;
    } while (!stop_execution);

    if (verbose || rank == 0)
        fmt::print("[{}]: henson done\n", rank);

    if (times)
        report_times(iteration);

    MPI_Finalize();
}

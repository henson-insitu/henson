#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>

#include <mpi.h>

#include <opts/opts.h>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <spdlog/spdlog.h>
namespace spd = spdlog;
std::shared_ptr<spd::logger> logger;

#include <henson/context.h>
#include <henson/puppet.hpp>
#include <henson/data.hpp>
#include <henson/procs.hpp>
#include <henson/hwl.hpp>
#include <henson/command-line.hpp>
namespace h = henson;

// http://stackoverflow.com/a/1486931/44738
#define UNUSED(expr) do { (void)(expr); } while (0)

std::string
clock_to_seconds(h::time_type t)
{
    return fmt::format("{}.{:02}", t / 1000000000, (t % 1000000000 / 10000000));
}

std::string
clock_output(h::time_type t)
{
    return fmt::format("{} ({})", h::clock_to_string(t), clock_to_seconds(t));
}

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
            logger->debug("Sending stop to {}", name);
            puppet.signal_stop();
        }
        logger->debug("Proceeding with {}", name);
        puppet.proceed();

        if (!puppet.running() && control.control == name)
        {
            logger->debug("Control puppet {} stopped in {}", name, control.name);
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

    h::MPIEnvironment mpi_env(&argc, &argv);    // RAII

    MPI_Comm world;
    MPI_Comm_dup(MPI_COMM_WORLD, &world);

    int rank, size;
    MPI_Comm_rank(world, &rank);
    MPI_Comm_size(world, &size);

    std::vector<std::string>    procs_sizes;
    std::string                 log_level = "info";
    using namespace opts;
    Options ops(argc, argv);
    ops
        >> Option('p', "procs", procs_sizes, "number of processors to use for a control group")
        >> Option('l', "log",   log_level,   "log level to use")
    ;

    bool show_sizes = ops >> Present('s', "show-sizes", "show group sizes");
    bool verbose    = ops >> Present('v', "verbose",    "verbose output");
    bool times      = ops >> Present('t', "show-times", "show time spent in each puppet");
    bool every_iteration = ops >> Present("every-iteration", "report times at every iteration");

    logger = spd::stderr_logger_st("console");
    logger->set_level(spd::level::off);
    int lvl;
    for (lvl = spd::level::trace; lvl < spd::level::off; ++lvl)
        if (spd::level::level_names[lvl] == log_level)
            break;
    if (verbose || rank == 0)
        logger->set_level(static_cast<spd::level::level_enum>(lvl));

    logger->set_pattern(fmt::format("[{}]: [%Y-%m-%d %H:%M:%S.%e] [%l] %v", rank));

    if (verbose || rank == 0)
        logger->info("henson started; total processes = {}", size);

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
        logger->error("Couldn't parse script: {}", script_fn);
        return 1;
    }

    // parse the procs
    std::vector<std::string>        all_group_names;
    for (auto& x : script.procs)
        all_group_names.emplace_back(x.first);
    h::ProcMap::Vector  procs = h::ProcMap::parse_procs(procs_sizes, size, all_group_names);

    h::ProcMap          procmap(world, procs);        // splits the communicator into groups
    henson_set_procmap(&procmap);                     // "activate henson" in libhenson-pmpi.so
    if (rank == 0 && show_sizes)
    {
        fmt::print("Group sizes:\n");
        for (auto& x : script.procs)
            fmt::print("  {} = {}\n", x.first, procmap.size(x.first));
    }

    // parse variables
    std::map<std::string,std::string>   variables;
    std::string                         var;
    while (ops >> PosOption(var))
    {
        size_t eq_pos = var.find('=');
        if (eq_pos == std::string::npos)
        {
            logger->error("Can't parse {}", var);
            return 1;
        }
        auto var_name  = var.substr(0, eq_pos);
        auto var_value = var.substr(eq_pos + 1, var.size() - eq_pos);
        variables[var_name] = var_value;
    }


    henson::NameMap                     namemap;        // global namespace shared by the puppets

    int                     color       = procmap.color();
    std::string             group       = procs[color].first;
    int                     group_size  = procs[color].second; UNUSED(group_size);
    const hwl::ControlFlow& control     = script.procs[group];

    // convert script into puppets
    // TODO: technically, we don't need to load puppets that we don't need, but I'm guessing it's a small overhead
    std::string prefix = h::prefix(script_fn);

    Puppets                                         puppets;
    for (auto& p : script.puppets)
    {
        if (!control.uses(p.name)) continue;

        // parse and process command variables
        logger->trace("Parsing command: {}", p.command);
        hwl::Command cmd;
        std::istringstream command_in(p.command);
        parser::state ps(command_in);
        bool result = hwl::command(ps, cmd);
        if (!result)
        {
            logger->error("Couldn't parse command: {}", p.command);
            return 1;
        }
        auto cmd_expanded = cmd.generate(variables);
        logger->trace("Command parsed and expanded: {}", cmd_expanded);

        auto cmd_line = h::CommandLine(cmd_expanded);
        puppets[p.name] = PuppetUniquePtr(new h::Puppet(cmd_line.executable(prefix),
                                                        cmd_line.argv.size(),
                                                        &cmd_line.argv[0],
                                                        &procmap,
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
            MPI_Reduce(&t, &max_t, 1, MPI_UNSIGNED_LONG_LONG, MPI_MAX, 0, procmap.local());
            max_puppet_times.emplace_back(p.first, max_t);

            puppet_time += t;
        }
        h::time_type context_switching_time = total_execution_time - puppet_time - initialization_time;

        h::time_type max_init, max_context_switching, max_puppet_time, max_total_time;
        MPI_Reduce(&initialization_time,    &max_init,              1, MPI_UNSIGNED_LONG_LONG, MPI_MAX, 0, procmap.local());
        MPI_Reduce(&context_switching_time, &max_context_switching, 1, MPI_UNSIGNED_LONG_LONG, MPI_MAX, 0, procmap.local());
        MPI_Reduce(&puppet_time,            &max_puppet_time,       1, MPI_UNSIGNED_LONG_LONG, MPI_MAX, 0, procmap.local());
        MPI_Reduce(&total_execution_time,   &max_total_time,        1, MPI_UNSIGNED_LONG_LONG, MPI_MAX, 0, procmap.local());

        if (procmap.is_leader(rank))
        {
            fmt::print("Max times (iter={}) for group {}:\n  init = {}, other = {}; puppet = {}; total = {}\n",
                       iteration, group,
                       clock_output(max_init),
                       clock_output(max_context_switching),
                       clock_output(max_puppet_time),
                       clock_output(max_total_time));
            for (auto& x : max_puppet_times)
                fmt::print("  Max time (iter={}) for {:<12} in group {}: {}\n",
                           iteration, std::get<0>(x), group,
                           clock_output(std::get<1>(x)));
        }

        if (verbose)
            fmt::print("[{}]: initialization = {}; other = {}; puppet time = {}; total time = {}\n",
                       rank,
                       clock_output(initialization_time),
                       clock_output(context_switching_time),
                       clock_output(puppet_time),
                       clock_output(total_execution_time));
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
            logger->debug("Revisiting {}", name);
            puppets[name]->proceed();
        }
        revisit.clear();

        if (times && every_iteration)
            report_times(iteration);

        ++iteration;
    } while (!stop_execution);

    if (verbose || rank == 0)
        logger->info("henson done");

    if (times)
        report_times(iteration);
}

#include <vector>
#include <string>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <unistd.h>

#include <mpi.h>

#include <opts/opts.h>
#include <format.h>
#include <spdlog/spdlog.h>
namespace spd = spdlog;
std::shared_ptr<spd::logger> logger;

#include <chaiscript/chaiscript.hpp>
#include <chaiscript/chaiscript_stdlib.hpp>

#include <henson/time.hpp>
#include <henson/procs.hpp>
#include <henson/data.hpp>
#include <henson/puppet.hpp>
#include <henson/scheduler.hpp>
#include <henson/command-line.hpp>
namespace h = henson;


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
    std::vector<std::string>    variables;
    std::string                 log_level = "info";
    using namespace opts;
    Options ops(argc, argv);
    ops
        >> Option('p', "procs", procs_sizes, "number of processors to use for a control group")
        >> Option('v', "var",   variables,   "define variables to inject into the script")
        >> Option('l', "log",   log_level,   "log level to use")
    ;

    bool verbose    = ops >> Present('v', "verbose",    "verbose output");

    logger = spd::stderr_logger_st("henson");
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
            fmt::print("Usage: {} SCRIPT [-p group=SIZE]*\n\n", argv[0]);
            fmt::print("Execute SCRIPT. procs are the names of execution groups in the script.\n");
            fmt::print("Leftover processors get split evenly among the execution groups in the SCRIPT\n");
            fmt::print("but not specified in the procs list.\n\n");
            fmt::print("{}", ops);
        }
        return 1;
    }

    // figure out the prefix
    std::string script_prefix = h::prefix(script_fn);

    h::NameMap namemap;
    h::ProcMap proc_map(world, h::ProcMap::parse_procs(procs_sizes, size));

    chaiscript::ChaiScript chai(chaiscript::Std_Lib::library());

    // Puppet
    chai.add(chaiscript::user_type<h::Puppet>(),        "Puppet");
    chai.add(chaiscript::fun(&h::Puppet::proceed),      "proceed");
    chai.add(chaiscript::fun(&h::Puppet::running),      "running");
    chai.add(chaiscript::fun(&h::Puppet::signal_stop),  "signal_stop");

    chai.add(chaiscript::fun([&namemap,script_prefix](std::string cmd_line_str, henson::ProcMap* pm)
    {
        auto cmd_line = h::CommandLine(cmd_line_str);
        return std::make_shared<h::Puppet>(cmd_line.executable(script_prefix),
                                           cmd_line.argv.size(),
                                           &cmd_line.argv[0],
                                           pm,
                                           &namemap);
    }), "load");

    // NameMap
    // TODO: why not just create a new namemap?
    chai.add(chaiscript::fun([&namemap] () { return &namemap; }), "NameMap");
    // Probably should figure out what to do if something isn't in the map
    // NB: not exposed to chai: arrays
    chai.add(chaiscript::fun([](henson::NameMap* namemap, std::string name)
    {
        henson::Value val = namemap->get(name);
        if(val.tag == val._int)
            return chaiscript::Boxed_Value(val.i);
        else if(val.tag == val._double)
            return chaiscript::Boxed_Value(val.d);
        else if(val.tag == val._float)
            return chaiscript::Boxed_Value(val.f);
        else if(val.tag == val._size_t)
            return chaiscript::Boxed_Value(val.s);
        else if(val.tag == val._ptr)
            return chaiscript::Boxed_Value((intptr_t) val.p);
        else
        {
            throw std::runtime_error("Error: namemap value (" + name + ") was not an accepted type, or may not exist");
            return chaiscript::Boxed_Value();
        }
    }), "get");
    chai.add(chaiscript::fun([](h::NameMap* namemap, std::string name, int x)    { h::Value v; v.tag = h::Value::_int;    v.i = x; namemap->add(name, v); }), "add");
    chai.add(chaiscript::fun([](h::NameMap* namemap, std::string name, size_t x) { h::Value v; v.tag = h::Value::_size_t; v.s = x; namemap->add(name, v); }), "add");
    chai.add(chaiscript::fun([](h::NameMap* namemap, std::string name, float x)  { h::Value v; v.tag = h::Value::_float;  v.f = x; namemap->add(name, v); }), "add");
    chai.add(chaiscript::fun([](h::NameMap* namemap, std::string name, double x) { h::Value v; v.tag = h::Value::_double; v.d = x; namemap->add(name, v); }), "add");
    chai.add(chaiscript::fun(&h::NameMap::create_queue),                    "create_queue");
    chai.add(chaiscript::fun(&h::NameMap::queue_empty),                     "queue_empty");
    chai.add(chaiscript::fun(&h::NameMap::exists),                          "exists");


    // ProcMap
    chai.add(chaiscript::fun([&proc_map]() { return &proc_map; }),          "ProcMap");
    chai.add(chaiscript::fun(&h::ProcMap::group),                           "group");
    chai.add(chaiscript::fun(&h::ProcMap::color),                           "color");
    chai.add(chaiscript::fun(&h::ProcMap::world_rank),                      "world_rank");
    chai.add(chaiscript::fun(&h::ProcMap::local_rank),                      "local_rank");
    chai.add(chaiscript::fun([](h::ProcMap* pm, std::string to)
                             { pm->intercomm(to); }),                       "intercomm");

    // Scheduler
    chai.add(chaiscript::fun([&chai, &proc_map]()
    {
        return std::make_shared<h::Scheduler>(proc_map.local(), &chai, &proc_map);
    }),                                                                     "Scheduler");

    auto clone    = chai.eval<std::function<chaiscript::Boxed_Value (const chaiscript::Boxed_Value&)>>("clone");
    auto schedule = [&clone](h::Scheduler* s, std::string name, std::string function, chaiscript::Boxed_Value arg, std::map<std::string, chaiscript::Boxed_Value> groups, int size)
    {
        h::ProcMap::Vector groups_vector;

        // divide unused procs between groups of size <= 0
        std::vector<std::string> unspecified;
        int specified = 0;
        for (auto& x : groups)
        {
            int sz = chaiscript::boxed_cast<int>(x.second);
            if (sz <= 0)
                unspecified.push_back(x.first);
            else
                specified += sz;
        }

        int leftover = size - specified;
        int leftover_group_size = size / unspecified.size();
        for (auto& x : groups)
        {
            int sz = chaiscript::boxed_cast<int>(x.second);
            if (sz > 0)
                groups_vector.emplace_back(x.first, sz);
            else if (x.first == unspecified.back())
                groups_vector.emplace_back(x.first, leftover - leftover_group_size * (unspecified.size() - 1));     // in case leftover doesn't divide evenly
            else
                groups_vector.emplace_back(x.first, leftover_group_size);
        }
        s->schedule(name, function, clone(arg), groups_vector, size);
    };
    chai.add(chaiscript::fun(schedule),                                     "schedule");
    chai.add(chaiscript::fun(&h::Scheduler::listen),                        "listen");
    chai.add(chaiscript::fun(&h::Scheduler::size),                          "size");
    chai.add(chaiscript::fun(&h::Scheduler::rank),                          "rank");
    chai.add(chaiscript::fun(&h::Scheduler::workers),                       "workers");
    chai.add(chaiscript::fun(&h::Scheduler::job_queue_empty),               "job_queue_empty");
    chai.add(chaiscript::fun(&h::Scheduler::is_controller),                 "is_controller");
    chai.add(chaiscript::fun(&h::Scheduler::control),                       "control");
    chai.add(chaiscript::fun(&h::Scheduler::results_empty),                 "results_empty");
    chai.add(chaiscript::fun(&h::Scheduler::pop),                           "pop");
    chai.add(chaiscript::fun(&h::Scheduler::finish),                        "finish");

    chai.add(chaiscript::fun([](int secs) { sleep(secs); }),                "sleep");
    chai.add(chaiscript::fun([]() { std::cout << std::flush; }),            "flush");
    chai.add(chaiscript::fun(&h::get_time),                                 "time");
    chai.add(chaiscript::fun(&h::clock_to_string),                          "clock_to_string");

    // Read and broadcast the script
    std::vector<char> buffered_in;
    int file_size = 0;
    if(rank == 0)
    {
        std::ifstream script_file(script_fn, std::ios::in | std::ios::binary | std::ios::ate);
        file_size = script_file.tellg();
        buffered_in.resize(file_size);
        script_file.seekg(0, std::ios::beg);
        script_file.read(buffered_in.data(), file_size);
    }

    // Broadcast the file size then the file itself
    MPI_Bcast(&file_size, 1, MPI_INT, 0, world);
    buffered_in.resize(file_size);  // does nothing on rank 0
    MPI_Bcast(buffered_in.data(), file_size, MPI_CHAR, 0, world);

    // parse variables and inject them as globals
    std::map<std::string,std::string>   vars;
    for (auto& var : variables)
    {
        size_t eq_pos = var.find('=');
        if (eq_pos == std::string::npos)
        {
            logger->error("Can't parse {}", var);
            return 1;
        }
        auto var_name  = var.substr(0, eq_pos);
        auto var_value = var.substr(eq_pos + 1, var.size() - eq_pos);
        chai.add_global(chaiscript::var(var_value), var_name);
    }

    try
    {
        std::string new_string(buffered_in.begin(), buffered_in.end());
        chai.eval(new_string);
    } catch(const std::exception& e)
    {
        logger->critical("Caught: {}", e.what());
        MPI_Abort(world, 1);
    }

    logger->info("henson done");
}

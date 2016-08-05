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

    MPI_Init(&argc, &argv);

    MPI_Comm world = MPI_COMM_WORLD;

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
    h::ProcMap proc_map(world, procs_sizes, size);

    chaiscript::ChaiScript chai(chaiscript::Std_Lib::library());
    chai.add(chaiscript::bootstrap::standard_library::vector_type<std::vector<double>>("VecDouble"));

    // Puppet
    chai.add(chaiscript::user_type<h::Puppet>(),        "Puppet");
    chai.add(chaiscript::fun(&h::Puppet::proceed),      "proceed");
    chai.add(chaiscript::fun(&h::Puppet::running),      "running");
    chai.add(chaiscript::fun(&h::Puppet::signal_stop),  "signal_stop");

    chai.add(chaiscript::fun([&namemap,script_prefix](std::string name, std::string cmd_line_str, henson::ProcMap* pm)
    {
        auto cmd_line = h::CommandLine(cmd_line_str);
        return std::make_shared<h::Puppet>(cmd_line.executable(script_prefix),
                                           cmd_line.argv.size(),
                                           &cmd_line.argv[0],
                                           pm,
                                           &namemap);
    }), "load");

    // NameMap
    chai.add(chaiscript::fun([&namemap] () { return &namemap; }), "NameMap");
    // Probably should figure out what to do if something isn't in the map
    // NB: not exposed to chai: arrays
    chai.add(chaiscript::fun([](henson::NameMap* namemap, std::string name)
    {
        henson::DataType* the_value = namemap->get(name);
        if(henson::Value<int>* temp = dynamic_cast< henson::Value<int> * >(the_value))
            return chaiscript::Boxed_Value(temp->value);
        else if(henson::Value<double>* temp = dynamic_cast<henson::Value<double>*>(the_value))
            return chaiscript::Boxed_Value(temp->value);
        else if(henson::Value<float>* temp = dynamic_cast<henson::Value<float>*>(the_value))
            return chaiscript::Boxed_Value(temp->value);
        else if(henson::Value<size_t>* temp = dynamic_cast<henson::Value<size_t>*>(the_value))
            return chaiscript::Boxed_Value(temp->value);
        else if(henson::Value<void*>* temp = dynamic_cast<henson::Value<void*>*>(the_value))
            return chaiscript::Boxed_Value((intptr_t)temp->value);
        else
        {
            throw std::runtime_error("Error: namemap value (" + name + ") was not an accepted type, or may not exist");
            return chaiscript::Boxed_Value();
        }
    }), "get");

    chai.add(chaiscript::fun([&chai](chaiscript::Boxed_Value& bv)
    {
        void* temp = (void*) (chai.boxed_cast<intptr_t>(bv));
        std::vector<double>* v = (std::vector<double> *)temp;
        return v;
    }), "convert_to_vec_double");

    // ProcMap
    chai.add(chaiscript::fun([&proc_map]() { return proc_map.get_lowest_procmap(); }),  "ProcMap");
    chai.add(chaiscript::fun(&henson::ProcMap::isInGroup),                              "isInGroup");
    chai.add(chaiscript::fun(&henson::ProcMap::get_local_rank),                         "get_local_rank");
    chai.add(chaiscript::fun(&henson::ProcMap::get_job_rank),                           "get_job_rank");
    chai.add(chaiscript::fun(&henson::ProcMap::color),                                  "color");
    chai.add(chaiscript::fun(&henson::ProcMap::get_job_name),                           "get_job_name");
    chai.add(chaiscript::fun([&](const std::string& to)
    {
        proc_map.get_lowest_procmap()->intercomm(to);
    }), "henson_get_intercomm");

    // Scheduler
    //chai.add(chaiscript::user_type<henson::Scheduler>(), "Scheduler");
    chai.add(chaiscript::fun([&chai, &proc_map]()
    {
        return std::make_shared<h::Scheduler>(proc_map.local(), &chai, &proc_map);
    }), "Scheduler");
    chai.add(chaiscript::fun(&h::Scheduler::schedule_job),                              "schedule_job");
    chai.add(chaiscript::fun(&h::Scheduler::listen),                                    "listen");
    chai.add(chaiscript::fun(&h::Scheduler::get_size),                                  "get_size");
    chai.add(chaiscript::fun(&h::Scheduler::get_schedule_rank),                         "get_schedule_rank");
    chai.add(chaiscript::fun(&h::Scheduler::is_active),                                 "is_active");
    chai.add(chaiscript::fun(&h::Scheduler::is_job_queue_empty),                        "is_job_queue_empty");
    chai.add(chaiscript::fun(&h::Scheduler::is_controller),                             "is_controller");
    chai.add(chaiscript::fun(&h::Scheduler::control),                                   "control");
    chai.add(chaiscript::fun(&h::Scheduler::check_for_complete_jobs),                   "check_for_complete_jobs");
    chai.add(chaiscript::fun(&h::Scheduler::is_stack_empty),                            "is_stack_empty");
    chai.add(chaiscript::fun(&h::Scheduler::next_on_stack),                             "next_on_stack");
    chai.add(chaiscript::fun(&h::Scheduler::pop_stack),                                 "pop_stack");
    chai.add(chaiscript::fun(&h::Scheduler::finish),                                    "finish");

    chai.add(chaiscript::fun([](int secs) { sleep(secs); }),                            "sleep");
    chai.add(chaiscript::fun([]() { std::cout << std::flush; }),                        "flush");

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

    std::string new_string(buffered_in.begin(), buffered_in.end());
    chai.eval(new_string);

    logger->info("henson done");

    MPI_Finalize();
}

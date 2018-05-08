#include <vector>
#include <string>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <unistd.h>

#include <signal.h>
#include <execinfo.h>

#include <mpi.h>

#include <opts/opts.h>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <spdlog/spdlog.h>
namespace spd = spdlog;
std::shared_ptr<spd::logger> logger;

#include <chaiscript/chaiscript.hpp>
#include <chaiscript/chaiscript_stdlib.hpp>

#include <henson/context.h>
#include <henson/time.hpp>
#include <henson/procs.hpp>
#include <henson/data.hpp>
#include <henson/puppet.hpp>
#include <henson/scheduler.hpp>
#include <henson/command-line.hpp>
namespace h = henson;
#ifdef HENSON_PYTHON
#include <henson/python-puppet.hpp>
#endif

// used for debugging of segfaults
int rank;
std::string active_puppet;
std::shared_ptr<h::ProcMap> proc_map;
bool abort_on_segfault_ = true;

#include <cxxabi.h>

void catch_sig(int signum)
{
    logger->critical("caught signal {}; active puppet {}; local group = {}, local rank = {}",
                     signum, active_puppet, proc_map->group(), proc_map->local_rank());

    // print backtrace
    void*   callstack[128];
    int     frames      = backtrace(callstack, 128);
    char**  strs        = backtrace_symbols(callstack, frames);

    size_t funcnamesize = 256;
    char*  funcname     = (char*) malloc(funcnamesize);

    // iterate over the returned symbol lines. skip the first, it is the
    // address of this function.
    for (int i = 1; i < frames; i++)
    {
        char *begin_name = 0, *begin_offset = 0, *end_offset = 0;

        // find parentheses and +address offset surrounding the mangled name:
        // ./module(function+0x15c) [0x8048a6d]
        for (char *p = strs[i]; *p; ++p)
        {
            if (*p == '(')
                begin_name = p;
            else if (*p == '+')
                begin_offset = p;
            else if (*p == ')' && begin_offset)
            {
                end_offset = p;
                break;
            }
        }

        if (begin_name && begin_offset && end_offset && begin_name < begin_offset)
        {
            *begin_name++   = '\0';
            *begin_offset++ = '\0';
            *end_offset     = '\0';

            // mangled name is now in [begin_name, begin_offset) and caller
            // offset in [begin_offset, end_offset). now apply __cxa_demangle():

            int status;
            char* ret = abi::__cxa_demangle(begin_name, funcname, &funcnamesize, &status);
            if (status == 0)
            {
                funcname = ret; // use possibly realloc()-ed string
                logger->critical("  {} : {}+{}", strs[i], funcname, begin_offset);
            } else
            {
                // demangling failed. Output function name as a C function with no arguments.
                logger->critical("  {} : {}()+{}", strs[i], begin_name, begin_offset);
            }
        }
        else
        {
            // couldn't parse the line? print the whole line.
            logger->critical("  {}", strs[i]);
        }
    }

    free(funcname);
    free(strs);

    //for (int i = 0; i < frames; ++i)
    //    logger->critical("{}", strs[i]);

    signal(signum, SIG_DFL);    // restore the default signal
    if (abort_on_segfault_)
        MPI_Abort(MPI_COMM_WORLD, 1);
}


int main(int argc, char *argv[])
{
    signal(SIGSEGV, catch_sig);     // catch segfault

    h::MPIEnvironment mpi_env(&argc, &argv);    // RAII

    MPI_Comm world;
    MPI_Comm_dup(MPI_COMM_WORLD, &world);

    int size;
    MPI_Comm_rank(world, &rank);
    MPI_Comm_size(world, &size);

    std::vector<std::string>    procs_sizes { "world" };
    std::vector<std::string>    variables;
    std::string                 log_level = "info";
    bool verbose, help;

    using namespace opts;
    Options ops;
    ops
        >> Option('p', "procs", procs_sizes, "number of processors to use for a control group")
        >> Option('v', "var",   variables,   "define variables to inject into the script")
        >> Option('l', "log",   log_level,   "log level to use")
        >> Option("verbose",    verbose,     "verbose output")
        >> Option('h', "help",  help,        "show help")
    ;

    std::string script_fn;
    if (!ops.parse(argc,argv) || help || !(ops >> PosOption(script_fn)))
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

    logger = spd::stderr_logger_st("henson");
    logger->set_level(spd::level::warn);
    int lvl;
    for (lvl = spd::level::trace; lvl < spd::level::off; ++lvl)
        if (spd::level::level_names[lvl] == log_level)
            break;
    if (verbose || rank == 0)
    {
        auto level = static_cast<spd::level::level_enum>(lvl);
        logger->set_level(level);
        logger->flush_on(level);
    }

    logger->set_pattern(fmt::format("[{}]: [%Y-%m-%d %H:%M:%S.%e] [%l] %v", rank));

    if (verbose || rank == 0)
        logger->info("henson started; total processes = {}", size);

    if (procs_sizes.empty())
        logger->warn("No procs specified on the command line");

    // figure out the prefix
    std::string script_prefix = h::prefix(script_fn);

    h::NameMap namemap;
    proc_map = std::make_shared<h::ProcMap>(world, h::ProcMap::parse_procs(procs_sizes, size));

    henson_set_procmap(&*proc_map);         // "activate henson" in libhenson-pmpi.so

    std::unique_ptr<chaiscript::ChaiScript> chai_ptr { new chaiscript::ChaiScript(chaiscript::Std_Lib::library()) };
    chaiscript::ChaiScript& chai = *chai_ptr;

    // Puppet
    chai.add(chaiscript::base_class<h::BaseCoroutine, h::Puppet>());
    chai.add(chaiscript::user_type<h::Puppet>(),        "Puppet");
    chai.add(chaiscript::fun([](h::Puppet& puppet)
    {
        active_puppet = puppet.name();
        logger->debug("Proceeding with {}", puppet.name());
        puppet.proceed();
        return puppet.running();
    }), "proceed");
    chai.add(chaiscript::fun(&h::Puppet::running),      "running");
    chai.add(chaiscript::fun(&h::Puppet::signal_stop),  "signal_stop");
    chai.add(chaiscript::fun(&h::Puppet::total_time),   "total_time");

    chai.add(chaiscript::fun([&namemap,script_prefix](std::string cmd_line_str, henson::ProcMap* pm)
    {
        auto cmd_line = h::CommandLine(cmd_line_str);
        return std::make_shared<h::Puppet>(cmd_line.executable(script_prefix),
                                           cmd_line.argv.size(),
                                           &cmd_line.argv[0],
                                           pm,
                                           &namemap);
    }), "load");

#ifdef HENSON_PYTHON
    // PythonPuppet
    chai.add(chaiscript::base_class<h::BaseCoroutine, h::PythonPuppet>());
    chai.add(chaiscript::user_type<h::PythonPuppet>(),  "PythonPuppet");
    chai.add(chaiscript::fun([](h::PythonPuppet& puppet)
    {
        active_puppet = puppet.name();
        logger->debug("Proceeding with {}", puppet.name());
        puppet.proceed();
        return puppet.running();
    }), "proceed");
    chai.add(chaiscript::fun([&namemap,script_prefix](std::string python_script, henson::ProcMap* pm)
    {
        return std::make_shared<h::PythonPuppet>(python_script, pm, &namemap);
    }), "python");
#endif

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
    chai.add(chaiscript::fun([]() { return proc_map; }),                    "ProcMap");
    chai.add(chaiscript::fun(&h::ProcMap::group),                           "group");
    chai.add(chaiscript::fun(&h::ProcMap::color),                           "color");
    chai.add(chaiscript::fun(&h::ProcMap::world_rank),                      "world_rank");
    chai.add(chaiscript::fun(&h::ProcMap::local_rank),                      "local_rank");
    chai.add(chaiscript::fun([](h::ProcMap* pm, std::string to)
                             { pm->intercomm(to); }),                       "intercomm");

    // Scheduler
    chai.add(chaiscript::fun([&chai]()
    {
        return std::make_shared<h::Scheduler>(proc_map->local(), &chai, proc_map.get());
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
    chai.add(chaiscript::fun([](bool abort_on_segfault)
                             { abort_on_segfault_ = abort_on_segfault; }),  "abort_on_segfault");

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
        chai_ptr = nullptr;        // trigger destruction of all the puppets
    } catch(const std::exception& e)
    {
        logger->critical("Caught: {}", e.what());
        MPI_Abort(world, 1);
    }

    logger->info("henson done");
    MPI_Barrier(world);

    signal(SIGSEGV, SIG_DFL);       // restore default signal

    proc_map = std::shared_ptr<h::ProcMap>();   // release ProcMap before MPI_Finalize from RAII
}

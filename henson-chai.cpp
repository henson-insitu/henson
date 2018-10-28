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

#include <henson/version.hpp>
#include <henson/context.h>
#include <henson/procs.hpp>
#include <henson/command-line.hpp>
namespace h = henson;

#include <henson/chai/puppet.hpp>
#include <henson/chai/data.hpp>
#include <henson/chai/procs.hpp>
#include <henson/chai/scheduler.hpp>
#include <henson/chai/util.hpp>
#include <henson/chai/stdlib.hpp>

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
    int                         controller_ranks = 1;
    bool verbose, help, version;

    using namespace opts;
    Options ops;
    ops
        >> Option('p', "procs", procs_sizes, "number of processors to use for a control group")
        >> Option('v', "var",   variables,   "define variables to inject into the script")
        >> Option('l', "log",   log_level,   "log level to use")
        >> Option('c', "controller", controller_ranks,  "ranks to use for scheduler controller")
        >> Option("verbose",    verbose,     "verbose output")
        >> Option('h', "help",  help,        "show help")
        >> Option('v', "version", version,   "show version")
    ;

    std::string script_fn;
    if (!ops.parse(argc,argv) || help || (!version && !(ops >> PosOption(script_fn))))
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

    if (version)
    {
        if (rank == 0)
        {
            fmt::print("henson-chai version {}", HENSON_VERSION);
#if defined(MPICH_VERSION)
            fmt::print(" (MPICH version {})", MPICH_VERSION);
#elif defined(OMPI_MAJOR_VERSION)
            fmt::print(" (OpenMPI version {}.{}.{})", OMPI_MAJOR_VERSION, OMPI_MINOR_VERSION, OMPI_RELEASE_VERSION);
#else
            fmt::print(" (unknown MPI)");
#endif
            fmt::print("\n");
        }
        return 0;
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

    auto chai_ptr = chai_stdlib();
    chaiscript::ChaiScript& chai = *chai_ptr;

    chai_puppet(chai, namemap, script_prefix);
    chai_data(chai, namemap);
    chai_procs(chai);
    chai_scheduler(chai, controller_ranks);
    chai_util(chai);

    // Read and broadcast the script
    std::vector<char> buffered_in;
    int file_size = 0;
    if(rank == 0)
    {
        std::ifstream script_file(script_fn, std::ios::in | std::ios::binary | std::ios::ate);
        if (!script_file)
        {
            logger->critical("Unable to open: {}", script_fn);
            MPI_Abort(world, 1);
        }
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

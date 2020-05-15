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

#include <henson/version.hpp>
#include <henson/context.h>
#include <henson/procs.hpp>
#include <henson/data.hpp>
#include <henson/puppet.hpp>
namespace h = henson;

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

std::string henson_version()
{
    std::string result;
    result += fmt::format("henson-exec version {}", HENSON_VERSION);
#if defined(MPICH_VERSION)
    result += fmt::format(" (MPICH version {})", MPICH_VERSION);
#elif defined(OMPI_MAJOR_VERSION)
    result += fmt::format(" (OpenMPI version {}.{}.{})", OMPI_MAJOR_VERSION, OMPI_MINOR_VERSION, OMPI_RELEASE_VERSION);
#else
    result += fmt::format(" (unknown MPI)");
#endif
    return result;
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
        >> Option('l', "log",   log_level,   "log level to use")
        >> Option("verbose",    verbose,     "verbose output")
        >> Option('h', "help",  help,        "show help")
        >> Option('v', "version", version,   "show version")
    ;

    bool parse_result = ops.parse(argc,argv);

    // read remaining positional arguments
    std::vector<std::string> args;
    std::string str;
    if (parse_result)
        while(ops >> PosOption(str))
            args.push_back(str);

    if (!parse_result || help || args.empty())
    {
        if (rank == 0)
        {
            fmt::print("Usage: {} [OPTIONS] -- PROG ARG1 ARG2 ARG3 ...\n\n", argv[0]);
            fmt::print("Execute PROG with ARG1 ... arguments.\n\n");
            fmt::print("{}", ops);
        }
        return 1;
    }

    if (version)
    {
        if (rank == 0)
        {
            fmt::print(henson_version());
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
    {
        logger->info(henson_version());
        logger->info("henson started; total processes = {}", size);
    }

    h::NameMap namemap;
    proc_map = std::make_shared<h::ProcMap>(world, h::ProcMap::parse_procs(procs_sizes, size));

    henson_set_procmap(&*proc_map);         // "activate henson" in libhenson-pmpi.so

    try
    {
        h::Puppet puppet(args[0], args, proc_map.get(), &namemap);
        do
        {
            puppet.proceed();
        } while(puppet.running());
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

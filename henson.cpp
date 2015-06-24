#include <stdexcept>

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <dlfcn.h>

#include <mpi.h>

#include <boost/context/all.hpp>
namespace bc = boost::context;

#include <opts/opts.h>
#include <format.h>

#include <henson/namemap.hpp>

struct Puppet
{
    typedef             int  (*MainType)(int argc, char *argv[]);
    typedef             void (*SetContextType)(void* parent, void* local);
    typedef             void (*SetWorldType)(MPI_Comm world);
    typedef             void (*SetNameMapType)(void* namemap);

                        Puppet(const std::string& fn, int argc, char** argv, MPI_Comm world, henson::NameMap* namemap):
                            filename_(fn),
                            argc_(argc), argv_(argv),
                            //stack_(bc::stack_traits::default_size())      // requires Boost 1.58
                            stack_(1024*1024)
                        {
                            void* lib = dlopen(fn.c_str(), RTLD_LAZY);

                            main_ = get_function<MainType>(lib, "main");

                            get_function<SetContextType>(lib, "henson_set_contexts")(&from_, &to_);
                            get_function<SetWorldType>  (lib, "henson_set_world")(world);
                            get_function<SetNameMapType>(lib, "henson_set_namemap")(namemap);

                            to_ = bc::make_fcontext(&stack_[0] + stack_.size(), stack_.size(), exec);
                        }

    void                proceed()               { bc::jump_fcontext(&from_, to_, (intptr_t) this); }
    void                yield()                 { bc::jump_fcontext(&to_, from_, 0); }

    bool                running() const         { return running_; }

    static void         exec(intptr_t self_)    { Puppet* self = (Puppet*) self_; self->running_ = true; self->main_(self->argc_,self->argv_); self->running_ = false; self->yield(); }

    template<class T>
    T                   get_function(void* lib, const char* name)
    {
        T f = (T) dlsym(lib, name);
        if (f == NULL)
            throw std::runtime_error(fmt::format("Could not load {}() in {}\n{}\n", name, filename_, dlerror()));
        return f;
    }

    std::string         filename_;
    int                 argc_;
    char**              argv_;
    std::vector<char>   stack_;

    MainType            main_;
    bc::fcontext_t      from_, to_;
    bool                running_;
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

    std::string simulation_fn, analysis_fn;
    if (  ops >> Present('h', "help", "show help") ||
        !(ops >> PosOption(simulation_fn) >> PosOption(analysis_fn)))
    {
        fmt::print("Usage: {} SIMULATION ANALYSIS\n{}", argv[0], ops);
        return 1;
    }

    henson::NameMap     namemap;

    Puppet  simulation(simulation_fn, 0, 0, world, &namemap);
    Puppet  analysis  (analysis_fn,   0, 0, world, &namemap);

    do
    {
        simulation.proceed();
        if (!simulation.running())
            break;

        // reset namemap (so analysis can read from it)
        for (henson::NameMap::iterator it = namemap.begin(); it != namemap.end(); ++it)
            it->second.reset();

        analysis.proceed();

        // reset namemap (so analysis can read from it)
        for (henson::NameMap::iterator it = namemap.begin(); it != namemap.end(); ++it)
            it->second.reset();
    } while (true);

    fmt::print("[{}]: henson done\n", rank);

    MPI_Finalize();
}

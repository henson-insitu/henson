#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <dlfcn.h>

#include <mpi.h>

#include <boost/context/all.hpp>
namespace bc = boost::context;

#include <opts/opts.h>
#include <format.h>


struct Puppet
{
    typedef             int  (*MainType)(int argc, char *argv[]);
    typedef             void (*SetContextType)(void* parent, void* local);

                        Puppet(const std::string& fn, int argc, char** argv):
                            argc_(argc), argv_(argv),
                            //stack_(bc::stack_traits::default_size())      // requires Boost 1.58
                            stack_(1024*1024)
                        {
                            void* lib = dlopen(fn.c_str(), RTLD_LAZY);
                            main_ = (MainType) dlsym(lib, "main");
                            if (main_ == NULL)
                                fmt::print("Could not load main() in {}\n{}\n", fn, dlerror());
                            SetContextType set_contexts = (SetContextType) dlsym(lib, "set_contexts");
                            if (set_contexts == NULL)
                                fmt::print("Could not load set_contexts() in {}\n{}\n", fn, dlerror());
                            set_contexts(&from_, &to_);

                            to_ = bc::make_fcontext(&stack_[0] + stack_.size(), stack_.size(), exec);
                        }

    void                proceed()               { bc::jump_fcontext(&from_, to_, (intptr_t) this); }
    void                yield()                 { bc::jump_fcontext(&to_, from_, 0); }

    bool                running() const         { return running_; }

    static void         exec(intptr_t self_)    { Puppet* self = (Puppet*) self_; self->running_ = true; self->main_(self->argc_,self->argv_); self->running_ = false; self->yield(); }


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

    fmt::print("henson: {} out of {}\n", rank, size);

    using namespace opts;
    Options ops(argc, argv);

    std::string simulation_fn, analysis_fn;
    if (  ops >> Present('h', "help", "show help") ||
        !(ops >> PosOption(simulation_fn) >> PosOption(analysis_fn)))
    {
        fmt::print("Usage: {} SIMULATION ANALYSIS\n{}", argv[0], ops);
        return 1;
    }

    Puppet  simulation(simulation_fn, 0, 0);
    Puppet  analysis(analysis_fn, 0, 0);

    do
    {
        simulation.proceed();
        if (!simulation.running())
            break;
        analysis.proceed();
    } while (true);

    fmt::print("[{}]: henson done\n", rank);

    MPI_Finalize();
}

#ifndef HENSON_PUPPET_HPP
#define HENSON_PUPPET_HPP

#include <vector>
#include <stdexcept>
#include <iostream>
#include <cstring>

#include <unistd.h>
#include <errno.h>
#include <dlfcn.h>

#include <mpi.h>
#include <coro.h>

#include <format.h>
#include <henson/data.hpp>
#include <henson/procs.hpp>
#include <henson/time.hpp>

namespace henson
{

struct Puppet
{
    typedef             coro_context        context_t;
    typedef             int  (*MainType)(int argc, char *argv[]);
    typedef             void (*SetContextType)(void* parent, void* local);
    typedef             void (*SetWorldType)(MPI_Comm world);
    typedef             void (*SetNameMapType)(void* namemap);
    typedef             void (*SetProcMapType)(void* procmap);
    typedef             void (*SetStopType)(int* stop);

                        Puppet(const std::string& fn, int argc, char** argv, ProcMap* procmap, NameMap* namemap):
                            filename_(fn),
                            argc_(argc), argv_(argc_)
                        {
                            for (size_t i = 0; i < argc_; ++i)
                            {
                                argv_[i] = new char[strlen(argv[i]) + 1];
                                strcpy(argv_[i], argv[i]);
                            }

                            void* lib = dlopen(fn.c_str(), RTLD_LAZY);

                            main_ = get_function<MainType>(lib, "main");

                            get_function<SetContextType>(lib, "henson_set_contexts")(&from_, &to_);
                            get_function<SetProcMapType>(lib, "henson_set_procmap")(procmap);
                            get_function<SetStopType>   (lib, "henson_set_stop")(&stop_);

                            try
                            {
                                get_function<SetNameMapType>(lib, "henson_set_namemap")(namemap);
                            } catch(std::runtime_error& e)
                            {
                                // it's a weird situation, but possible if the puppet doesn't need to get any data in or out
                                // (the linker may choose not to pull src/data.cpp and so henson_set_namemap will be missing)
                                fmt::print(std::cerr, "Warning: {}", e.what());
                                fmt::print(std::cerr, "Reasonable only if {} doesn't need to exchange any data\n", filename_);
                            }

                            coro_stack_alloc(&stack_, 8*1024*1024);     // 8MB stack
                            coro_create(&to_, exec, this, stack_.sptr, stack_.ssze);
                            coro_create(&from_, NULL, NULL, NULL, 0);
                        }

                        ~Puppet()               { for (char* a : argv_) delete[] a; coro_stack_free(&stack_); }

    // can't even move a puppet since the addresses of its from_ and to_ fields
    // are stored in the modules (saved via henson_set_context, in the constructor above)
                        Puppet(const Puppet&)   =delete;
                        Puppet(Puppet&&)        =delete;

    Puppet&             operator=(const Puppet&)=delete;
    Puppet&             operator=(Puppet&&)     =delete;

    void                proceed()               { start_time_ = get_time(); coro_transfer(&from_, &to_); time_type diff = get_time() - start_time_; total_time_ += diff; }
    void                yield()                 { coro_transfer(&to_, &from_); }

    void                signal_stop()           { stop_ = 1; }

    bool                running() const         { return running_; }
    int                 result() const          { return result_; }

    time_type           total_time() const      { return total_time_; }

    static void         exec(void* arg)
    {
        while(true)
        {
            Puppet* self = (Puppet*) arg;
            self->running_ = true;
            self->start_time_ = get_time();
            self->result_ = self->main_(self->argc_,&self->argv_[0]);
            self->running_ = false;
            self->yield();      // the time for the final portion will get recorded thanks to this call
        }
    }

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
    std::vector<char*>  argv_;
    coro_stack          stack_;

    MainType            main_;
    context_t           from_, to_;
    bool                running_;
    int                 stop_ = 0;
    int                 result_ = -1;

    time_type           start_time_;
    time_type           total_time_ = 0;
};


}

#endif

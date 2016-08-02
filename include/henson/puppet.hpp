#ifndef HENSON_PUPPET_HPP
#define HENSON_PUPPET_HPP

#include <vector>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <cstring>

#include <unistd.h>
#include <errno.h>
#include <dlfcn.h>

#include <mpi.h>

#ifdef USE_BOOST
#include <boost/context/all.hpp>
#else
#include <coro.h>
#endif

#include <format.h>
#include <henson/data.hpp>
#include <henson/procs.hpp>
#include <henson/time.hpp>

namespace henson
{
#ifdef USE_BOOST
namespace bc = boost::context;
#endif

struct Puppet
{
#ifdef USE_BOOST
    typedef             bc::fcontext_t      context_t;
#else
    typedef             coro_context        context_t;
#endif

    typedef             int  (*MainType)(int argc, char *argv[]);
    typedef             void (*SetContextType)(void* parent, void* local);
    typedef             void (*SetWorldType)(MPI_Comm world);
    typedef             void (*SetNameMapType)(void* namemap);
    typedef             void (*SetProcMapType)(void* procmap);
    typedef             void (*SetStopType)(int* stop);

                        Puppet(const std::string& fn, int argc, char** argv, ProcMap* procmap, NameMap* namemap):
                            filename_(fn),
                            argc_(argc), argv_(argc_), running_(false)
                        {
                            for (int i = 0; i < argc_; ++i)
                            {
                                argv_[i] = new char[strlen(argv[i]) + 1];
                                strcpy(argv_[i], argv[i]);
                            }

                            void* lib = dlopen(fn.c_str(), RTLD_LAZY);
                            lib_ = lib;
                            puppet_name_ = fn;

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

#ifdef USE_BOOST
                            stack_ = allocator_.allocate();
                            to_ = bc::make_fcontext(stack_.sp, stack_.size, exec);
#else
                            coro_stack_alloc(&stack_, 8*1024*1024);     // 8MB stack
                            coro_create(&to_, exec, this, stack_.sptr, stack_.ssze);
                            coro_create(&from_, NULL, NULL, NULL, 0);
#endif
                        }

#ifdef USE_BOOST
                        ~Puppet()
                        {
                            //std::cout << "Destroying " << puppet_name_ << std::endl;
                            if(running_)
                            {
                                signal_stop();
                                proceed();
                            }

                            //dlclose(lib_);

                            for (char* a : argv_)
                                delete[] a;
                            allocator_.deallocate(stack_);

                        }
    void                proceed()               { start_time_ = get_time(); bc::jump_fcontext(&from_, to_, (intptr_t) this); time_type diff = get_time() - start_time_; total_time_ += diff; }
    void                yield()                 { bc::jump_fcontext(&to_, from_, 0); }

#else
                        ~Puppet()
                        {
                            //std::cout << "Destroying " << puppet_name_ << std::endl;
                            if(running_)
                            {
                                //std::cout << "Signal_stoping and proceeding!" << std::endl;
                                signal_stop();
                                proceed();
                            }

                            //dlclose(lib_);

                            //std::cout << "About to delete argv_" << std::endl;
                            for (char* a : argv_)
                                delete[] a; 
                            //std::cout << "About to free coro_stack" << std::endl;
                            coro_stack_free(&stack_);

                        }
    void                proceed()               { start_time_ = get_time(); coro_transfer(&from_, &to_); time_type diff = get_time() - start_time_; total_time_ += diff; }
    void                yield()                 { coro_transfer(&to_, &from_); }
#endif


    // can't even move a puppet since the addresses of its from_ and to_ fields
    // are stored in the modules (saved via henson_set_context, in the constructor above)
                        Puppet(const Puppet&)   =delete;
                        Puppet(Puppet&&)        =delete;

    Puppet&             operator=(const Puppet&)=delete;
    Puppet&             operator=(Puppet&&)     =delete;

    void                signal_stop()           { stop_ = 1; }

    bool                running() const         { return running_; }
    int                 result() const          { return result_; }

    time_type           total_time() const      { return total_time_; }

#if USE_BOOST
    static void         exec(intptr_t self_)
#else
    static void         exec(void* self_)
#endif
    {
        while(true)
        {
            Puppet* self = (Puppet*) self_;
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
        {
            std::string temp = "Could not load: " + std::string(name) + " in " + filename_;
            throw std::runtime_error(fmt::format("Could not load {}() in {}\n{}\n", name, filename_, dlerror()));
        }
        return f;
    }

    std::string         filename_;
    int                 argc_;
    std::vector<char*>  argv_;

#ifdef USE_BOOST
    bc::stack_context   stack_;
    bc::fixedsize_stack allocator_;
#else
    coro_stack          stack_;
#endif

    MainType            main_;
    context_t           from_, to_;
    bool                running_;
    int                 stop_ = 0;
    int                 result_ = -1;
    void *              lib_;

    time_type           start_time_;
    time_type           total_time_ = 0;
    std::string         puppet_name_;
};


}

#endif

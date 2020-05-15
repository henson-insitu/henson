#ifndef HENSON_PUPPET_HPP
#define HENSON_PUPPET_HPP

#include <vector>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <cstring>
#include <memory>

#include <unistd.h>
#include <errno.h>
#include <dlfcn.h>

#include <mpi.h>

#include <fmt/format.h>
#include <henson/data.hpp>
#include <henson/procs.hpp>

#include "coroutine.hpp"

namespace henson
{
struct Puppet: public Coroutine<Puppet>
{
    using Parent = Coroutine<Puppet>;

    typedef             int  (*MainType)(int argc, char *argv[]);
    typedef             void (*SetContextType)(void* parent, void* local);
    typedef             void (*SetWorldType)(MPI_Comm world);
    typedef             void (*SetNameMapType)(void* namemap);
    typedef             void (*SetProcMapType)(void* procmap);
    typedef             void (*SetStopType)(int* stop);

                        Puppet(const std::string& fn, int argc, char** argv, ProcMap* procmap, NameMap* namemap):
                            Parent(fn),
                            filename_(fn),
                            argc_(argc), argv_(argc_)
                        {
                            for (int i = 0; i < argc_; ++i)
                            {
                                argv_[i] = new char[strlen(argv[i]) + 1];
                                strcpy(argv_[i], argv[i]);
                            }

                            load(procmap, namemap);
                        }

                        Puppet(const std::string& fn, const std::vector<std::string>& args, ProcMap* procmap, NameMap* namemap):
                            Parent(fn),
                            filename_(fn),
                            argc_(args.size()), argv_(argc_)
                        {
                            for (int i = 0; i < argc_; ++i)
                            {
                                argv_[i] = new char[strlen(args[i].c_str()) + 1];
                                strcpy(argv_[i], args[i].c_str());
                            }

                            load(procmap, namemap);
                        }

    void                load(ProcMap* procmap, NameMap* namemap)
                        {
                            void* lib = dlopen(filename_.c_str(), RTLD_LAZY);
                            lib_ = lib;
                            if (lib == NULL)
                                throw std::runtime_error(fmt::format("Could not load {}\n{}\n", filename_, dlerror()));

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
                                log_->warn("{} | Reasonable only if {} doesn't need to exchange any data", e.what(), filename_);
                            }
                        }

                        ~Puppet()
                        {
                            if(running_)
                            {
                                signal_stop();
                                proceed();
                            }

                            for (char* a : argv_)
                                delete[] a;

                            dlclose(lib_);
                        }

    static void         exec(void* self_)
    {
        Puppet* self = (Puppet*) self_;

        while(true)
        {
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

    MainType            main_;
    void*               lib_;
};


}

#endif

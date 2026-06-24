#ifndef HENSON_CONTEXT_H
#define HENSON_CONTEXT_H

#include <mpi.h>

#ifdef __cplusplus
extern "C" {
#endif

int         henson_active();
void        henson_yield();
void        henson_set_contexts(void* parent, void* local);
void        henson_set_stop(int* s);
int         henson_stop();

void        henson_set_procmap(void* procmap);
void*       henson_get_procmap();
MPI_Comm    henson_get_world();
MPI_Comm    henson_get_intercomm(const char* to);
MPI_Comm    henson_get_intracomm(const char* to);

#ifdef __cplusplus
}

namespace henson
{
struct ScopedProcMapActivation
{
        explicit ScopedProcMapActivation(void* procmap):
                previous_(henson_get_procmap())             { henson_set_procmap(procmap); }
        ~ScopedProcMapActivation()                         { henson_set_procmap(previous_); }

        ScopedProcMapActivation(const ScopedProcMapActivation&)            =delete;
        ScopedProcMapActivation& operator=(const ScopedProcMapActivation&) =delete;

        void* previous_;
};
}
#endif

#endif

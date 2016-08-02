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
MPI_Comm    henson_get_world();
MPI_Comm    henson_get_intercomm(const char* to);
MPI_Comm    henson_get_intracomm(const char* to);
MPI_Comm    henson_get_subintercomm(const char* to, MPI_Comm intra_comm, int remote_leader);
void        henson_destroy_subintercomm(const char* to);

#ifdef __cplusplus
}
#endif

#endif

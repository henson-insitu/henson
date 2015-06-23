#ifndef HENSON_CONTEXT_H
#define HENSON_CONTEXT_H

#include <mpi.h>

#ifdef __cplusplus
extern "C" {
#endif

void        henson_yield();
void        henson_set_contexts(void* parent, void* local);

MPI_Comm    henson_get_world();
void        henson_set_world(MPI_Comm world);

#ifdef __cplusplus
}
#endif

#endif

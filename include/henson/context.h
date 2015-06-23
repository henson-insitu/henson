#ifndef HENSON_CONTEXT_H
#define HENSON_CONTEXT_H

#include <mpi.h>

#ifdef __cplusplus
extern "C" {
#endif

void        yield();
void        set_contexts(void* parent, void* local);

MPI_Comm    get_world();
void        set_world(MPI_Comm world);

#ifdef __cplusplus
}
#endif

#endif

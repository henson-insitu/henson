#include <henson/context.h>

#define HENSON_REPLACE_COMM_WORLD(comm) \
    if (comm == MPI_COMM_WORLD) \
        comm = henson_get_world();

{{fn foo MPI_Init MPI_Finalize}}
    if (henson_active())
        return MPI_SUCCESS;
    {{callfn}}
{{endfn}}

{{fn foo MPI_Init_thread}}
    if (henson_active())
    {
        *{{3}} = MPI_THREAD_SINGLE;
        return MPI_SUCCESS;
    }
    {{callfn}}
{{endfn}}

{{fnall foo MPI_Init MPI_Init_thread MPI_Finalize}}
  {{apply_to_type MPI_Comm HENSON_REPLACE_COMM_WORLD}}
  {{callfn}}
{{endfnall}}

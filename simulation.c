#include <stdio.h>
#include <unistd.h>
#include <mpi.h>

#include "henson/context.h"

int main(int argc, char** argv)
{
    // Under henson, MPI will be initialized before we are launched;
    // still need to initialize MPI in the stand-alone mode,
    // so initialize it if it's not already initialized
    int mpi_initialized;
    MPI_Initialized(&mpi_initialized);
    if (!mpi_initialized)
        MPI_Init(&argc, &argv);

    MPI_Comm world = henson_get_world();

    int rank, size;
    MPI_Comm_rank(world, &rank);
    MPI_Comm_size(world, &size);

    int t;
    for (t = 0; t < 3; ++t)
    {
        sleep(rank);
        printf("Simulation [t=%d]: rank = %d out of %d\n", t, rank, size);
        henson_yield();
    }

    if (!mpi_initialized)       // we initialized MPI
        MPI_Finalize();

    return 0;
}

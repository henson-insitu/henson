#include <stdio.h>
#include <unistd.h>
#include <mpi.h>

#include "henson/context.h"

int main(int argc, char** argv)
{
    MPI_Comm world = MPI_COMM_WORLD;

    int rank, size;
    MPI_Comm_rank(world, &rank);
    MPI_Comm_size(world, &size);

    int t = 0;
    while(true)
    {
        sleep(rank);
        printf("Analysis   [t=%d]: rank = %d out of %d\n", t, rank, size);
        yield();
        ++t;
    }

    return 0;
}

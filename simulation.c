#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <mpi.h>

#include <henson/data.h>
#include <henson/context.h>

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

    srand(time(NULL) + rank);

    int t;
    for (t = 0; t < 3; ++t)
    {
        //sleep(rank);

        int n = 50;
        float* array = malloc(n * sizeof(float));
        for (size_t i = 0; i < n; ++i)
            array[i] = (float) rand() / (float) RAND_MAX;

        float sum = 0;
        for (size_t i = 0; i < n; ++i)
            sum += array[i];
        printf("[%d]: Simulation [t=%d]: sum = %f\n", rank, t, sum);

        henson_save_array("data", array, sizeof(float), n, sizeof(float));
        henson_yield();

        free(array);
    }

    if (!mpi_initialized)       // we initialized MPI
        MPI_Finalize();

    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <mpi.h>

#include <henson/data.h>
#include <henson/context.h>

// simulation [NUM [ITER [SLEEP]]]
int main(int argc, char** argv)
{
    // Under henson, MPI will be initialized before we are launched;
    // still need to initialize MPI in the stand-alone mode,
    // so initialize it if it's not already initialized
    if (!henson_active())
        MPI_Init(&argc, &argv);

    MPI_Comm world = henson_get_world();
    int rank, size;
    MPI_Comm_rank(world, &rank);
    MPI_Comm_size(world, &size);

    size_t n = 50;
    if (argc > 1)
        n = atoi(argv[1]);
    if (rank == 0)
        printf("Using %zu random numbers\n", n);

    int iterations = 3;
    if (argc > 2)
        iterations = atoi(argv[2]);

    int sleep_interval = 0;
    if (argc > 3)
        sleep_interval = atoi(argv[3]);

    srand(time(NULL) + rank);

    int t;
    for (t = 0; t < iterations; ++t)
    {
        if (sleep_interval)
            sleep(sleep_interval);

        float* array = malloc(n * sizeof(float));
        size_t i;
        for (i = 0; i < n; ++i)
            array[i] = (float) rand() / (float) RAND_MAX;

        float sum = 0;
        for (i = 0; i < n; ++i)
            sum += array[i];
        printf("[%d]: Simulation [t=%d]: sum = %f\n", rank, t, sum);

        float total_sum;
        MPI_Reduce(&sum, &total_sum, 1, MPI_FLOAT, MPI_SUM, 0, world);

        if (rank == 0)
            printf("[%d]: Simulation [t=%d]: total_sum = %f\n", rank, t, total_sum);

        henson_save_int("t", t);
        henson_save_array("data", array, sizeof(float), n, sizeof(float));
        henson_yield();

        free(array);
    }

    if (!henson_active())
        MPI_Finalize();

    return 0;
}

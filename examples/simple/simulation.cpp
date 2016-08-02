#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
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
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

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

        float* array = (float *) malloc(n * sizeof(float));
        size_t i;
        for (i = 0; i < n; ++i)
            array[i] = (float) rand() / (float) RAND_MAX;

        float sum = 0;
        for (i = 0; i < n; ++i)
            sum += array[i];
        printf("[%d]: Simulation [t=%d]: sum = %f\n", rank, t, sum);

        float total_sum;
        MPI_Reduce(&sum, &total_sum, 1, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);

        if (rank == 0)
            printf("[%d]: Simulation [t=%d]: total_sum = %f\n", rank, t, total_sum);

        henson_save_int("t", t);

        std::vector<double> stuff(5, 0);
        for(int i = 0; i < stuff.size(); ++i)
        {
            stuff[i] = i;
            //std::cout << "Vector value at: " << i << " is: " << stuff[i] << std::endl;
        }
        //std::cout << "Void * inside sim is: " << (void *)&stuff << std::endl;
        henson_save_pointer("testing", (void *)(&stuff));

        henson_save_array("data", array, sizeof(float), n, sizeof(float));
        henson_yield();

        /*if(t != 0)
        {
            for(int i = 0; i < stuff.size(); ++i)
            {
                if(rank == 0)
                    std::cout << "Vector value after first yield at: " << i << " is: " << stuff[i] << std::endl;
            }
        }
        henson_yield();
        */

        free(array);
    }


   // henson_yield();

    MPI_Finalize();

    return 0;
}

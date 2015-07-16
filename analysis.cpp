#include <cstdio>
#include <cassert>
#include <unistd.h>
#include <mpi.h>

#include <henson/data.h>
#include <henson/context.h>

int main(int argc, char** argv)
{
    if (!henson_active())
    {
        printf("Must run under henson, but henson is not active\n");
        return 1;
    }

    MPI_Comm world = henson_get_world();

    int rank, size;
    MPI_Comm_rank(world, &rank);
    MPI_Comm_size(world, &size);

    int t = 0;
    while(true)
    {
        //sleep(rank);

        float* data;
        size_t count;
        size_t type;
        size_t stride;
        henson_load_array("data", (void**) &data, &type, &count, &stride);
        assert(type == sizeof(float));

        float sum = 0;
        for (size_t i = 0; i < count; ++i)
            sum += data[i];

        printf("[%d]: Analysis   [t=%d]: sum = %f\n", rank, t, sum);

        float total_sum;
        MPI_Reduce(&sum, &total_sum, 1, MPI_FLOAT, MPI_SUM, 0, world);

        if (rank == 0)
            printf("[%d]: Analysis   [t=%d]: total_sum = %f\n", rank, t, total_sum);

        henson_yield();
        ++t;
    }

    return 0;
}

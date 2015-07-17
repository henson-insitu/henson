#include <cstdio>
#include <cassert>
#include <unistd.h>
#include <mpi.h>

#include <henson/data.h>
#include <henson/data.hpp>
#include <henson/context.h>

// simulation [SLEEP]
int main(int argc, char** argv)
{
    if (!henson_active())
    {
        printf("Must run under henson, but henson is not active\n");
        return 1;
    }

    int sleep_interval = 0;
    if (argc > 1)
        sleep_interval = atoi(argv[1]);

    MPI_Comm world = henson_get_world();

    int rank, size;
    MPI_Comm_rank(world, &rank);
    MPI_Comm_size(world, &size);

    if (henson_stop())
        return 0;

    int t;
    henson_load_int("t", &t);

    if (sleep_interval)
        sleep(sleep_interval);

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
}

#include <mpi.h>

#include <vector>

#include <henson/data.h>
#include <henson/context.h>

#include "tags.hpp"

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

    MPI_Comm intercomm = henson_get_intercomm("producer");

    int producer_size;
    MPI_Comm_remote_size(intercomm, &producer_size);

    if (producer_size != size)
    {
        printf("Abort: producer must be the same size as the consumer\n");
        return 1;
    }

    while(true)
    {
        size_t count;
        MPI_Status s;
        int flag;

        MPI_Probe(rank, MPI_ANY_TAG, intercomm, &s);
        MPI_Iprobe(rank, tags::stop, intercomm, &flag, &s);

        if (flag)
        {
            printf("[%d]: stop signal in receive\n", rank);
            return 0;
        }

        int t;
        MPI_Recv(&t,     1, MPI_INT,           rank, 0, intercomm, &s);
        MPI_Recv(&count, 1, MPI_UNSIGNED_LONG, rank, 0, intercomm, &s);

        std::vector<float> data(count);
        MPI_Recv(&data[0], count, MPI_FLOAT, rank, 0, intercomm, &s);
        printf("[%d]: data received, count = %lu\n", rank, count);

        henson_save_int("t", t);
        henson_save_array("data", &data[0], sizeof(float), count, sizeof(float));
        henson_yield();     // yield to let the data persist in memory; so receive needs a second jump to exit
    }
}

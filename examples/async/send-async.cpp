#include <cassert>

#include <mpi.h>

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

    MPI_Comm intercomm = henson_get_intercomm("consumer");

    int consumer_size;
    MPI_Comm_remote_size(intercomm, &consumer_size);

    if (consumer_size != size)
    {
        printf("Abort: consumer must be the same size as the producer\n");
        return 1;
    }

    MPI_Status s;
    if (henson_stop())
    {
        // unblock the root's data request
        if (rank == 0)
            MPI_Recv(0, 0, MPI_INT, rank, tags::request_data, intercomm, &s);

        printf("[%d]: send signalling stop\n", rank);
        MPI_Send(0,0,MPI_INT,rank,tags::stop,intercomm);
        return 0;
    }

    // check if receiver is ready
    bool receiver_ready = false;
    int flag;
    if (rank == 0)
    {
        MPI_Status s;
        MPI_Iprobe(rank, tags::request_data, intercomm, &flag, &s);
        if (flag)
            MPI_Recv(0, 0, MPI_INT, rank, tags::request_data, intercomm, &s);       // unblock the send

        MPI_Bcast(&flag,1,MPI_INT,0,world);
    } else
        MPI_Bcast(&flag,1,MPI_INT,0,world);

    if (!flag) return 0;

    float* data;
    size_t count;
    size_t type;
    size_t stride;
    int t;
    henson_load_int("t", &t);
    henson_load_array("data", (void**) &data, &type, &count, &stride);
    assert(type == sizeof(float));

    MPI_Send(&t,     1, MPI_INT,           rank, tags::data, intercomm);
    MPI_Send(&count, 1, MPI_UNSIGNED_LONG, rank, tags::data, intercomm);
    MPI_Send(&data[0], count, MPI_FLOAT, rank, tags::data, intercomm);
    printf("[%d]: data sent, count = %lu\n", rank, count);
}

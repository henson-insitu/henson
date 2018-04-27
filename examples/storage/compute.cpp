#include <mpi.h>

#include <henson/data.h>
#include <henson/context.h>

#include <fmt/format.h>
#include <fmt/ostream.h>
#include <opts/opts.h>

#include "common.hpp"

int main(int argc, char** argv)
{
    using namespace opts;
    Options ops(argc,argv);

    std::string remote_group;
    if (  ops >> Present('h', "help", "show help")    ||
        !(ops >> PosOption(remote_group)))
    {
        fmt::print("Usage: {} REMOTE_GROUP\n{}", argv[0], ops);
        return 1;
    }

    if (!henson_active())
    {
        fmt::print("Must run under henson, but henson is not active\n");
        return 1;
    }

    MPI_Comm local = henson_get_world();
    int rank, size;
    MPI_Comm_rank(local, &rank);
    MPI_Comm_size(local, &size);

    MPI_Comm remote       = henson_get_intercomm(remote_group.c_str());
    MPI_Comm remote_intra = henson_get_intracomm(remote_group.c_str());
    int remote_intra_rank;
    MPI_Comm_rank(remote_intra, &remote_intra_rank);

    // send request to remote group, identifying self
    int peer_intra_rank;
    if (rank == 0)
    {
        MPI_Status status;
        MPI_Send(&remote_intra_rank, 1, MPI_INT, /* rank = */ 0, tags::init, remote);
        MPI_Recv(&peer_intra_rank, 1, MPI_INT, 0, tags::rank, remote, &status);
    }
    MPI_Bcast(&peer_intra_rank, 1, MPI_INT, 0, local);

    // set up the intercomm
    MPI_Comm remote_local;
    MPI_Intercomm_create(local, 0, remote_intra, peer_intra_rank, tags::intercomm, &remote_local);

    // receive the data
    MPI_Status status;
    MPI_Probe(rank, tags::data, remote_local, &status);
    int count;
    MPI_Get_count(&status, MPI_FLOAT, &count);

    std::vector<float> data(count);
    MPI_Recv(data.data(), data.size(), MPI_FLOAT, rank, tags::data, remote_local, &status);

    // TODO: sleep

    // sum up the data
    float sum = 0;
    for (auto x : data)
        sum += x;
    fmt::print("Compute sum on {}: {}\n", rank, sum);

    float total_sum;
    MPI_Reduce(&sum, &total_sum, 1, MPI_FLOAT, MPI_SUM, 0, local);
    if (rank == 0)
    {
        fmt::print("Compute total sum: {}\n", total_sum);
        henson_save_float("total", total_sum);
    }
}

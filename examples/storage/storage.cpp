#include <random>

#include <mpi.h>

#include <henson/data.h>
#include <henson/context.h>

#include <format.h>
#include <opts/opts.h>

#include "common.hpp"

int main(int argc, char** argv)
{
    using namespace opts;
    Options ops(argc,argv);

    bool stop = ops >> Present('s', "stop", "signal remote storage to stop the execution");

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

    if (stop)
    {
        if (rank == 0)
            MPI_Send(nullptr, 0, MPI_INT, 0, tags::stop, remote);
        return 0;
    }

    // generate dummy data
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0, 1);
    std::vector<float>  data(16*1024*1024);
    for (auto& x : data)
        x = dis(gen);

    float sum = 0;
    for (auto x : data)
        sum += x;
    fmt::print("Sum on {}: {}\n", rank, sum);

    float total_sum;
    MPI_Reduce(&sum, &total_sum, 1, MPI_FLOAT, MPI_SUM, 0, local);
    if (rank == 0)
        fmt::print("Total sum: {}\n", total_sum);

    while(true)
    {
        // listen for requests
        int from_intra;
        if (rank == 0)
        {
            MPI_Status status;
            MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, remote, &status);

            if (status.MPI_TAG == tags::stop)
                from_intra = -1;        // signal stop to everybody
            else
            {
                int from = status.MPI_SOURCE;
                MPI_Recv(&from_intra, 1, MPI_INT, from, tags::init, remote, &status);

                MPI_Send(&remote_intra_rank, 1, MPI_INT, from, tags::rank, remote);
            }
        }
        MPI_Bcast(&from_intra, 1, MPI_INT, 0, local);

        if (from_intra == -1)
            return 0;

        // set up intercomm with the remote group
        MPI_Comm remote_local;
        MPI_Intercomm_create(local, 0, remote_intra, from_intra, tags::intercomm, &remote_local);

        // send the data
        MPI_Send(data.data(), data.size(), MPI_FLOAT, rank, tags::data, remote_local);
    }
}

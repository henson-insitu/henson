#include <cassert>

#include <mpi.h>

#include <henson/data.h>
#include <henson/context.h>

#include <format.h>
#include <opts/opts.h>

#include "common.hpp"

#define WRITE_TYPE(VARTYPE) \
    if (var.type == #VARTYPE)  \
    {   \
        VARTYPE x;  \
        henson_load_##VARTYPE(var.name.c_str(), &x);    \
        write(buffer, position, x); \
    }

bool receiver_ready(int rank, MPI_Comm local, MPI_Comm remote)
{
    int flag;
    if (rank == 0)
    {
        MPI_Status s;
        MPI_Iprobe(rank, tags::request_data, remote, &flag, &s);
        if (flag)
            MPI_Recv(0, 0, MPI_INT, rank, tags::request_data, remote, &s);       // unblock the send

        MPI_Bcast(&flag,1,MPI_INT,0,local);
    } else
        MPI_Bcast(&flag,1,MPI_INT,0,local);

    return flag;
}

int main(int argc, char** argv)
{
    using namespace opts;
    Options ops(argc,argv);

    bool async = ops >> Present('a', "async", "asynchronous mode");

    std::string remote_group;
    if (  ops >> Present('h', "help", "show help")    ||
        !(ops >> PosOption(remote_group)))
    {
        fmt::print("Usage: {} REMOTE_GROUP [variables]+\n{}", argv[0], ops);
        return 1;
    }

    std::vector<Variable>   variables;
    std::string             var;
    while (ops >> PosOption(var))
        variables.push_back(parse_variable(var));

    if (!henson_active())
    {
        fmt::print("Must run under henson, but henson is not active\n");
        return 1;
    }

    // Setup communicators
    MPI_Comm local = MPI_COMM_WORLD;
    int rank, size;
    MPI_Comm_rank(local, &rank);
    MPI_Comm_size(local, &size);

    MPI_Comm remote = henson_get_intercomm(remote_group.c_str());
    int remote_size;
    MPI_Comm_remote_size(remote, &remote_size);

    // Figure out partner ranks
    std::vector<int>    ranks;
    int                 fraction = 1;       // fraction of an array to send to a remote rank
    if (size >= remote_size)
    {
        if (size % remote_size != 0)
        {
            if (rank == 0)
                fmt::print("[send]: group size must be divisible by remote size (or vice versa), got {} vs {}\n", size, remote_size);
            return 1;
        }

        ranks.push_back(rank / (size / remote_size));
        fraction = 1;
    } else if (size < remote_size)
    {
        if (remote_size % size != 0)
        {
            if (rank == 0)
                fmt::print("[send]: remote size must be divisible by the group size (or vice versa), got {} vs {}\n", size, remote_size);
            return 1;
        }
        fraction = remote_size / size;
        for (int i = 0; i < fraction; ++i)
            ranks.push_back(rank*fraction + i);
    }

    MPI_Status s;
    if (henson_stop())
    {
        // unblock the root's data request
        if (async && rank == 0)
            MPI_Recv(0, 0, MPI_INT, rank, tags::request_data, remote, &s);

        if (rank == 0)
        {
            //fmt::print("[{}]: send signalling stop\n", rank);
            MPI_Send(0,0,MPI_INT,rank,tags::stop,remote);
        }

        return 0;
    }

    // check if receiver is ready
    if (async && !receiver_ready(rank,local,remote))
        return 0;

    for (size_t r = 0; r < ranks.size(); ++r)
    {
        int                 rank = ranks[r];
        std::vector<char>   buffer;
        size_t              position = 0;
        for (const Variable& var : variables)
        {
            WRITE_TYPE(int)     else
            WRITE_TYPE(size_t)  else
            WRITE_TYPE(float)   else
            WRITE_TYPE(double)  else
            if (var.type == "array")
            {
                // pack array into a buffer (pack parallel arrays together)
                // TODO: eventually optimize the path of a single contiguous array
                for (auto name : split(var.name, ','))
                {
                    void* data_;
                    size_t count;
                    size_t type;
                    size_t stride;
                    henson_load_array(name.c_str(), &data_, &type, &count, &stride);
                    char* data = (char*) data_;

                    size_t from = r * (count / fraction);
                    size_t to   = (r + 1 == ranks.size() ? count : (r+1) * (count / fraction));
                    write(buffer, position, (to - from));
                    write(buffer, position, type);
                    for (size_t i = from; i < to; ++i)
                        write(buffer, position, *((char*) data + i*stride), type);
                }
            } else
                fmt::print("Warning: unknown type {} for {}\n", var.type, var.name);
        }
        MPI_Send(&buffer[0], buffer.size(), MPI_BYTE, rank, tags::data, remote);
    }
}

#include <cassert>

#include <mpi.h>

#include <henson/data.h>
#include <henson/context.h>

#include <format.h>
#include <opts/opts.h>

#include "common.hpp"

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
    MPI_Comm local = henson_get_world();
    int rank, size;
    MPI_Comm_rank(local, &rank);
    MPI_Comm_size(local, &size);

    MPI_Comm remote = henson_get_intercomm(remote_group.c_str());
    int remote_size;
    MPI_Comm_remote_size(remote, &remote_size);

    // Figure out partner ranks
    std::vector<int>    ranks;
    ranks.push_back(rank);          // FIXME

    MPI_Status s;
    if (henson_stop())
    {
        // unblock the root's data request
        if (async && rank == 0)
            MPI_Recv(0, 0, MPI_INT, rank, tags::request_data, remote, &s);

        for (int rank : ranks)
        {
            fmt::print("[{}]: send signalling stop\n", rank);
            MPI_Send(0,0,MPI_INT,rank,tags::stop,remote);
        }
        return 0;
    }

    // check if receiver is ready
    if (async && !receiver_ready(rank,local,remote))
        return 0;

    std::vector<char>   buffer;
    size_t              position = 0;
    for (const Variable& var : variables)
    {
        if (var.type == "int")
        {
            int x;
            henson_load_int(var.name.c_str(), &x);
            for (int rank : ranks)
                write(buffer, position, x);
        } else if (var.type == "size_t")
        {
            size_t x;
            henson_load_size_t(var.name.c_str(), &x);
            for (int rank : ranks)
                write(buffer, position, x);
        } else if (var.type == "float")
        {
            float x;
            henson_load_float(var.name.c_str(), &x);
            for (int rank : ranks)
                write(buffer, position, x);
        } else if (var.type == "double")
        {
            double x;
            henson_load_double(var.name.c_str(), &x);
            for (int rank : ranks)
                write(buffer, position, x);
        } else if (var.type == "array")
        {
            for (int rank : ranks)
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

                    write(buffer, position, count);
                    write(buffer, position, type);
                    for (size_t i = 0; i < count; ++i)
                        write(buffer, position, *((char*) data + i*stride), type);
                }
            }
        } else
            fmt::print("Warning: unknown type {} for {}\n", var.type, var.name);
    }
    MPI_Send(&buffer[0], buffer.size(), MPI_BYTE, rank, tags::data, remote);
}

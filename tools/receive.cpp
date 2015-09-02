#include <mpi.h>

#include <vector>
#include <memory>

#include <henson/data.h>
#include <henson/context.h>

#include <format.h>
#include <opts/opts.h>

#include "common.hpp"

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

    while(true)
    {
        size_t count;
        MPI_Status s;
        bool stop;

        // request more data
        if (async && rank == 0)
            MPI_Send(0, 0, MPI_INT, rank, tags::request_data, remote);

        // check if we are told to stop
        // TODO: this loop forces us to wait until there is a message waiting from every rank
        //       we communicate with; in general, this is not great
        for (int rank : ranks)
        {
            int flag;
            MPI_Probe(rank,  MPI_ANY_TAG, remote, &s);
            MPI_Iprobe(rank, tags::stop,  remote, &flag, &s);
            stop |= flag;
        }

        if (stop)
        {
            fmt::print("[{}]: stop signal in receive\n", rank);
            return 0;
        }

        std::vector<std::unique_ptr<std::vector<char>>>     buffers;

        for (const Variable& var : variables)
        {
            fmt::print("Receiving {}\n", var.name);

            if (var.type == "int")
            {
                for (int rank : ranks)
                {
                    int x;
                    MPI_Recv(&x, 1, MPI_INT, rank, tags::data, remote, &s);
                    henson_save_int(var.name.c_str(), x);
                }
            } else if (var.type == "size_t")
            {
                for (int rank : ranks)
                {
                    size_t x;
                    MPI_Recv(&x, 1, MPI_UNSIGNED_LONG, rank, tags::data, remote, &s);
                    henson_save_size_t(var.name.c_str(), x);
                }
            } else if (var.type == "array")
            {
                for (int rank : ranks)
                {
                    std::vector<char>   buffer;

                    // FIXME: need to combine arrays from different ranks together
                    int c;
                    MPI_Probe(rank, tags::data, remote, &s);
                    MPI_Get_count(&s, MPI_BYTE, &c);
                    buffer.resize(c);
                    MPI_Recv(&buffer[0], buffer.size(), MPI_BYTE, rank, tags::data, remote, &s);

                    size_t position = 0;
                    for (auto name : split(var.name, ','))
                    {
                        size_t count; size_t type;
                        read(buffer, position, count);
                        read(buffer, position, type);
                        void*  data = &buffer[position];

                        henson_save_array(name.c_str(), data, type, count, type);

                        position += count*type;
                    }

                    buffers.emplace_back(new std::vector<char>);
                    buffers.back()->swap(buffer);
                }
            } else
                fmt::print("Warning: unknown type {} for {}\n", var.type, var.name);
        }

        henson_yield();
    }
}

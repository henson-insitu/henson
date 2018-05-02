#include <mpi.h>

#include <vector>
#include <memory>

#include <henson/data.h>
#include <henson/context.h>

#include <fmt/format.h>
#include <fmt/ostream.h>
#include <opts/opts.h>

#include "common.hpp"

// http://stackoverflow.com/a/1486931/44738
#define UNUSED(expr) do { (void)(expr); } while (0)

#define READ_TYPE(VARTYPE) \
    if (var.type == #VARTYPE)  \
    {   \
        VARTYPE x;  \
        read(buffer, position, x);  \
        henson_save_##VARTYPE(var.name.c_str(), x);   \
    }

int main(int argc, char** argv)
{
    using namespace opts;
    Options ops;

    bool async, help;
    ops
        >> Option('a', "async", async, "asynchronous mode")
        >> Option('h', "help",  help,  "show help");

    std::string remote_group;
    if (!ops.parse(argc,argv) || help || !(ops >> PosOption(remote_group)))
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
    if (size >= remote_size)
    {
        if (size % remote_size != 0)
        {
            if (rank == 0)
                fmt::print("[receive]: group size must be divisible by remote size (or vice versa), got {} vs {}\n", size, remote_size);
            return 1;
        }
        ranks.push_back(rank / (size / remote_size));
    } else if (size < remote_size)
    {
        if (remote_size % size != 0)
        {
            if (rank == 0)
                fmt::print("[receive]: remote size must be divisible by the group size (or vice versa), got {} vs {}\n", size, remote_size);
            return 1;
        }
        int fraction = remote_size / size;
        for (int i = 0; i < fraction; ++i)
            ranks.push_back(rank*fraction + i);
    }

    size_t array_count = 0;
    for (const Variable& var : variables)
        if (var.type == "array")
            array_count += split(var.name, ',').size();

    while(true)
    {
        MPI_Status s;

        // request more data
        if (async && rank == 0)
            MPI_Send(0, 0, MPI_INT, rank, tags::request_data, remote);

        // check if we are told to stop
        // TODO: this loop forces us to wait until there is a message waiting from every rank
        //       we communicate with; in general, this is not great
        int stop;
        if (rank == 0)
        {
            MPI_Probe(rank,  MPI_ANY_TAG, remote, &s);
            MPI_Iprobe(rank, tags::stop,  remote, &stop, &s);

            if (stop)
            {
                fmt::print("[{}]: stop signal in receive\n", rank);
                MPI_Recv(0, 0, MPI_INT, rank, tags::stop, remote, &s);       // unblock the send
            }

            MPI_Bcast(&stop,1,MPI_INT,0,local);
        } else
            MPI_Bcast(&stop,1,MPI_INT,0,local);

        if (stop)
            return 0;

        std::vector<std::vector<char>>     buffers(ranks.size());
        for (size_t i = 0; i < ranks.size(); ++i)
        {
            int   rank   = ranks[i];
            auto& buffer = buffers[i];

            int c;
            MPI_Probe(rank, tags::data, remote, &s);
            MPI_Get_count(&s, MPI_BYTE, &c);
            buffer.resize(c);
            MPI_Recv(&buffer[0], buffer.size(), MPI_BYTE, rank, tags::data, remote, &s);
        }

        std::vector<std::vector<char>>              arrays(array_count);
        std::vector<std::tuple<size_t, size_t>>     arrays_meta(array_count, std::make_tuple<size_t,size_t>(0,0));        // (count, type)
        for (size_t i = 0; i < ranks.size(); ++i)
        {
            int     rank      = ranks[i]; UNUSED(rank);
            auto&   buffer    = buffers[i];
            size_t  position  = 0;
            size_t  array_idx = 0;

            for (const Variable& var : variables)
            {
                READ_TYPE(int)      else
                READ_TYPE(size_t)   else
                READ_TYPE(float)    else
                READ_TYPE(double)   else
                if (var.type == "array")
                {
                    for (auto name : split(var.name, ','))
                    {
                        size_t count; size_t type;
                        read(buffer, position, count);
                        read(buffer, position, type);
                        void*  data = &buffer[position];

                        if (ranks.size() == 1)
                            henson_save_array(name.c_str(), data, type, count, type);       // save directly
                        else
                        {
                            // copy the data
                            auto&  array = arrays[array_idx];
                            size_t sz    = array.size();
                            array.resize(sz + count*type);
                            std::copy((char*) data, (char*) data + count*type, &array[sz]);

                            std::get<0>(arrays_meta[array_idx]) += count;
                            std::get<1>(arrays_meta[array_idx])  = type;
                        }

                        position += count*type;
                        ++array_idx;
                    }
                } else
                    fmt::print("Warning: unknown type {} for {}\n", var.type, var.name);
            }

            if (ranks.size() != 1)
                std::vector<char>().swap(buffers[i]);       // wipe out the buffer that we no longer need
        }

        if (ranks.size() != 1)
        {
            // save the arrays
            size_t array_idx = 0;
            for (const Variable& var : variables)
                if (var.type == "array")
                    for (auto name : split(var.name, ','))
                    {
                        size_t count; size_t type;
                        std::tie(count,type) = arrays_meta[array_idx];
                        henson_save_array(name.c_str(), &arrays[array_idx][0], type, count, type);
                        ++array_idx;
                    }
        }

        henson_yield();
    }
}

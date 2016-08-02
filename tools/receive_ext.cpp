#include <mpi.h>

#include <vector>
#include <memory>

#include <henson/data.h>
#include <henson/context.h>

#include <format.h>
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

    //std::cout << "{" << rank << "}\t\tAbout to call henson_get_intercomm" << std::endl;
    MPI_Comm remote = henson_get_intercomm(remote_group.c_str());
    //std::cout << "{" << rank << "}\t\tPast Henson_get_intercomm" << std::endl;

    MPI_Comm intra_comm = henson_get_intracomm(remote_group.c_str());
    //std::cout << "{" << rank << "}\t\tPast Henson_get_intracomm" << std::endl;
    int leader_proc = -1;
    if(rank == 0)
    {
        //We know the tag is 0, this can be set by calling henson_get_intercomm(string, int)
        //std::cout << "{" << rank << "}\t\tWaiting for job request" << std::endl;
        MPI_Recv(&leader_proc, 1, MPI_INT, MPI_ANY_SOURCE, 0, intra_comm, MPI_STATUS_IGNORE);

        //std::cout << "{" << rank << "}\t\tSending broadcast" << std::endl;
        MPI_Bcast(&leader_proc, 1, MPI_INT, 0, local);
    }
    else
    {
        //std::cout << "{" << rank << "}\t\tWaiting for broadcast" << std::endl;
        MPI_Bcast(&leader_proc, 1, MPI_INT, 0, local);
    }

    //std::cout << "{" << rank << "}\t\tAll trying to get the subintercomm" << std::endl;
    MPI_Comm remote_small = henson_get_subintercomm("first", intra_comm, leader_proc);

    int remote_size;
    MPI_Comm_remote_size(remote_small, &remote_size);

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
            MPI_Send(0, 0, MPI_INT, rank, tags::request_data, remote_small);

        // check if we are told to stop
        // TODO: this loop forces us to wait until there is a message waiting from every rank
        //       we communicate with; in general, this is not great
        int stop;
        if (rank == 0)
        {
            MPI_Probe(rank,  MPI_ANY_TAG, remote_small, &s);
            MPI_Iprobe(rank, tags::stop,  remote_small, &stop, &s);

            if (stop)
            {
                //fmt::print("[{}]: stop signal in receive\n", rank);
                MPI_Recv(0, 0, MPI_INT, rank, tags::stop, remote_small, &s);       // unblock the send
            }

            MPI_Bcast(&stop,1,MPI_INT,0,local);
        } else
            MPI_Bcast(&stop,1,MPI_INT,0,local);

        if (stop)
        {
            henson_destroy_subintercomm("first");
            return 0;
        }

        std::vector<std::vector<char>>     buffers(ranks.size());
        for (size_t i = 0; i < ranks.size(); ++i)
        {
            int   rank   = ranks[i];
            auto& buffer = buffers[i];

            int c;
            MPI_Probe(rank, tags::data, remote_small, &s);
            MPI_Get_count(&s, MPI_BYTE, &c);
            buffer.resize(c);
            MPI_Recv(&buffer[0], buffer.size(), MPI_BYTE, rank, tags::data, remote_small, &s);
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

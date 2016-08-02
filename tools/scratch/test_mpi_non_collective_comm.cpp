#include <string>
#include <iostream>
#include <vector>
#include "mpi.h"

#define TESTING_TAG 77

void MY_MPI_Non_collective_comm_create(MPI_Group the_group, MPI_Comm parent_comm, int tag, MPI_Comm * new_comm)
{
    int parent_rank, group_rank, group_size;
    MPI_Group parent_group;

    //MPI_Comm_rank(parent_comm, &rank);
    MPI_Group_rank(the_group, &group_rank);
    MPI_Group_size(the_group, &group_size);

    
    MPI_Comm_dup(MPI_COMM_SELF, new_comm);

    //Translate ranks
    MPI_Comm_group(parent_comm, &parent_group);
    std::vector<int> group_ranks(group_size, 0);
    for(int i = 0; i < group_size; ++i)
        group_ranks[i] = i;

    std::vector<int> translated_ranks(group_size, 0);
    MPI_Group_translate_ranks(the_group, group_size, group_ranks.data(), parent_group, translated_ranks.data());
    MPI_Group_free(&parent_group);


    MPI_Comm ic;
    for(int merge_size = 1; merge_size < group_size; merge_size = merge_size * 2)
    {
        MPI_Comm comm_old;
        int grp_id = group_rank / merge_size;
        comm_old = *new_comm;

        if (grp_id % 2 == 0)
        {
            if((grp_id + 1) * merge_size < group_size)
            {
                MPI_Intercomm_create(*new_comm, 0, parent_comm, translated_ranks[(grp_id + 1) * merge_size], tag, &ic);
                MPI_Intercomm_merge(ic, 0, new_comm);
            }
        }
        else
        {
            MPI_Intercomm_create(*new_comm, 0, parent_comm, translated_ranks[(grp_id - 1) * merge_size], tag, &ic);
            MPI_Intercomm_merge(ic, 1, new_comm);
        }

        if(*new_comm != comm_old)
        {
            MPI_Comm_free(&ic);
            MPI_Comm_free(&comm_old);
        }
    }
}


bool create_local_groups(std::vector<int> group_sizes, int first, int last, MPI_Group * my_groups)
{
    MPI_Group world_group;
    int placeholder = first;
    //*my_groups = new MPI_Group[group_sizes.size()];

    MPI_Comm_group(MPI_COMM_WORLD, &world_group);

    int i = 0;
    for(auto group_size : group_sizes)
    {
        std::vector<int> group_array(group_size, -1);
        for(int i = 0; i < group_size; ++i)
            group_array[i] = placeholder++;

        MPI_Group_incl(world_group, group_size, group_array.data(), &(my_groups[i]));

        ++i;
    }

    return true;


}

int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);

    int rank, size;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if(argc <= 2)
    {
        if(rank == 0)
            std::cout << "USAGE: mpirun -n $PROCS ./test_non_collective_com $PRINT_RANK ${Space separated num procs per group list}\n";
        return 1;
    }
    
    std::string::size_type sz;
    std::string temp(argv[1]);

    int printer = std::stoi(temp, &sz);

    std::vector<int> group_sizes(argc - 2, 0);
    for(int i = 1; i < argc - 1; ++i)
    {
        temp = argv[i + 1];
        int next = std::stoi(temp, &sz);

        group_sizes[i - 1] = next;
    }

    int low = 2;
    int highest = 8;
    int total = 0;
    for(auto small_size : group_sizes)
        total += small_size;

    if(total != (highest - low))
    {
        std::cerr << "Error: total group procs is: " << total << " and should be: " << size << "\n";
        return 1;
    }


    MPI_Group * group_holder = new MPI_Group[group_sizes.size()];

    create_local_groups(group_sizes, low, highest, group_holder);

    if(rank == printer)
    {
        std::cout << "[" << rank << "]\t\t";
        for(int i = 0; i < group_sizes.size(); ++i)
        {
            int grp_rank;
            MPI_Group_rank(group_holder[i], &grp_rank);
            std::string temp;
            grp_rank == MPI_UNDEFINED ? temp = "Undefined" : temp = std::to_string(grp_rank);
            std::cout << "Group" << i << " rank: " << temp << "\t\t";
        }
        std::cout << "\n";
    }


    MPI_Comm newcomm = MPI_COMM_NULL;
    for(int i = 0; i < group_sizes.size(); ++i)
    {
        if(rank == printer)
            std::cout << "[" << rank << "]\t\tCreating group " << i << "\n";


        int high = low + group_sizes[i];
        if(rank < high && rank >= low)
        {
            MY_MPI_Non_collective_comm_create(group_holder[i], MPI_COMM_WORLD, TESTING_TAG, &newcomm);
        }
        low = high;
    }

    //Test the new communicator that we just built
    int new_rank, new_size;
    if(newcomm != MPI_COMM_NULL)
    {
        MPI_Comm_rank(newcomm, &new_rank);
        MPI_Comm_size(newcomm, &new_size);

        if(new_rank == 0)
        {
            for(int i = 1; i < new_size; ++i)
            {
                MPI_Request req;
                MPI_Isend(&i, 1, MPI_INT, i, 2, newcomm, &req);
            }

        }
        else 
        {
            int hold = 0;
            MPI_Recv(&hold, 1, MPI_INT, 0, 2, newcomm, MPI_STATUS_IGNORE);
            std::cout << "[" << rank << "]\t\tReceived: " << hold << "\n";
            if(new_rank != hold)
                std::cerr << "Err: Something wrong!\n";
        }
    }


    MPI_Finalize();
    return 0;

}

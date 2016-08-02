#ifndef MPI_NONCOLL_COMM
#define MPI_NONCOLL_COMM

#include <vector>
#include <iostream>
#include <string>
#include "mpi.h"

#define NONCOLLECTIVE_TAG 77


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


void create_group(int first, int last, MPI_Comm parent_comm, MPI_Group* new_group)
{
    MPI_Group parent_group;

    MPI_Comm_group(parent_comm, &parent_group);

    std::vector<int> group_array(last - first + 1, -1);
    for(int i = 0; i < group_array.size(); ++i)
        group_array[i] = first + i;

    MPI_Group_incl(parent_group, group_array.size(), group_array.data(), new_group);

}

//This function creates groups where each element of group_sizes lists size of each group
//first and last are parameters that dictate the range of procs in which to create all of the groups
//my_groups is an array to store the groups inside of once they are created
//will return false on failure to create group
bool create_local_groups(std::vector<int> group_sizes, int first, int last, MPI_Group * my_groups)
{


    int total = 0;
    for(int small_size : group_sizes)
        total += small_size;

    if(total != (last - first + 1))
    {
        std::cerr << "Error: group sizes do not match range given from first to last\n";
        return false;
    }



    MPI_Group world_group;
    int placeholder = first;
    //*my_groups = new MPI_Group[group_sizes.size()];

    MPI_Comm_group(MPI_COMM_WORLD, &world_group);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int i = 0;
    for(int group_size : group_sizes)
    {
        std::vector<int> group_array(group_size, -1);
        for(int j = 0; j < group_size; ++j)
            group_array[j] = placeholder++;

        /*if(rank == 3)
            std::cout << "Group " << i << ": ";
        for(int j = 0; j < group_size; ++j)
        {
            if(rank == 3)
                std::cout << group_array[j] << "\t";
        }
        if(rank == 3)
            std::cout << "\n";
            */


        MPI_Group_incl(world_group, group_size, group_array.data(), &(my_groups[i]));

        ++i;
    }

    return true;


}

#endif

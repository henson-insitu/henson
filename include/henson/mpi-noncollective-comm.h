#ifndef HENSON_MPI_NONCOLLECTIVE_COMM
#define HENSON_MPI_NONCOLLECTIVE_COMM

#include <vector>
#include <iostream>
#include <string>
#include "mpi.h"

// Implements algorithm from "Noncollective Communicator Creation in MPI" by Dinan et al.
inline void non_collective_comm_create(MPI_Group the_group, MPI_Comm parent_comm, int tag, MPI_Comm* new_comm)
{
    int parent_rank, group_rank, group_size;
    MPI_Group parent_group;

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

#endif

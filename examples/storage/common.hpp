#include <mpi.h>

struct tags
{
    enum
    {
        init, intercomm, data, rank, stop
    };
};

int translate_rank(MPI_Comm comm1, int rank1, MPI_Comm comm2)
{
    MPI_Group group1, group2;
    MPI_Comm_group(comm1, &group1);
    MPI_Comm_group(comm2, &group2);

    int rank2;
    MPI_Group_translate_ranks(group1, 1, &rank1, group2, &rank2);

    MPI_Group_free(&group2);
    MPI_Group_free(&group1);

    return rank2;
}

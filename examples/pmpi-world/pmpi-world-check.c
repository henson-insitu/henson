#include <mpi.h>

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
    MPI_Init(&argc, &argv);

    int expected_size = 1;
    if (argc > 1)
        expected_size = atoi(argv[1]);

    int rank = -1;
    int size = -1;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int rank_sum = 0;
    MPI_Allreduce(&rank, &rank_sum, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

    int expected_rank_sum = expected_size * (expected_size - 1) / 2;
    int local_ok = (size == expected_size && rank >= 0 && rank < expected_size && rank_sum == expected_rank_sum);
    int all_ok = 0;
    MPI_Allreduce(&local_ok, &all_ok, 1, MPI_INT, MPI_MIN, MPI_COMM_WORLD);

    if (!local_ok || !all_ok)
    {
        fprintf(stderr,
                "PMPI world check failed: expected size %d, observed rank %d, observed size %d, observed rank sum %d, expected rank sum %d\n",
                expected_size,
                rank,
                size,
                rank_sum,
                expected_rank_sum);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    if (rank == 0)
        printf("PMPI world check passed: size = %d, rank sum = %d\n", size, rank_sum);

    MPI_Finalize();
    return 0;
}

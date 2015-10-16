#pragma once

#include <mpi.h>

struct communicator
{
        communicator(MPI_Comm comm = MPI_COMM_WORLD):
            comm_(comm)                 {}

        operator MPI_Comm() const       { return comm_; }

    int size() const                    { int sz; MPI_Comm_size(comm_, &sz); return sz; }
    int rank() const                    { int rk; MPI_Comm_rank(comm_, &rk); return rk; }

    MPI_Comm comm_;
};

struct environment
{
        environment()                   { MPI_Init(0,0); }
        ~environment()                  { MPI_Finalize(); }
};

from mpi4py import MPI
import pyhenson as h

comm = MPI.COMM_WORLD
rank = comm.Get_rank()
size = comm.Get_size()

print("MPI rank: %d of %d" % (rank, size))

if rank == 0:
    h.create_queue("trials")
    h.add("trials", 250)
    h.add("trials", 300)
    h.add("trials", 200)

from mpi4py import MPI

comm = MPI.COMM_WORLD
rank = comm.Get_rank()
size = comm.Get_size()

print("MPI rank: %d of %d" % (rank, size))

if rank == 0:
    henson_create_queue("trials")
    henson_add("trials", 250)
    henson_add("trials", 300)
    henson_add("trials", 200)

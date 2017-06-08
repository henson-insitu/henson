import pyhenson as h
import numpy as np
from mpi4py import MPI

w = MPI.COMM_WORLD.Dup()
print w.rank, w.size

a = MPI._addressof(w)       # required to interface with mpi4py
pm = h.ProcMap(a, [('world', w.size)])
nm = h.NameMap()

sim = h.Puppet('../../examples/simple/simulation', ['1250'], pm, nm)
ana = h.Puppet('../../examples/simple/analysis',   [],       pm, nm)

sim.proceed()
ana.proceed()
while sim.running():
    a = nm.get_array("data", 'f')       # f means, we expect this to be an array of floats
    t = nm.get('t', 'i')
    print "[%d]: From Python: %d -> %f" % (w.rank, t, np.sum(a))
    sim.proceed()
    ana.proceed()

t = sim.total_time()
print "Total time:", h.clock_to_string(t)

#!/usr/bin/env python3

from __future__ import print_function

import pyhenson as h
from mpi4py import MPI

w = MPI.COMM_WORLD.Dup()
print(w.rank, w.size)

pm = h.ProcMap(w, [('world', w.size)])
nm = h.NameMap()

sim = h.Puppet('../../examples/simple/simulation', ['1250'], pm, nm)
ana = h.Puppet('../../examples/simple/analysis',   [],       pm, nm)

sim.proceed()
ana.proceed()
while sim.running():
    t = nm.get('t')
    print("[%d]: From Python: t=%d" % (w.rank, t))
    sim.proceed()
    if sim.running():
        ana.proceed()

t = sim.total_time()
print("Total time:", h.clock_to_string(t))

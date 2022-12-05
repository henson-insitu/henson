import sys
import os
from mpi4py import MPI
import pyhenson as h

def world_func(arg):
    vals, n_iters, delay = arg
    print(f"Enter world_func, {vals =}, {n_iters=}, {delay=}")

    global procmap

    namemap = h.NameMap()

    if(procmap.group() == "producer"):
        # print(f"In producer")
        sim = h.Puppet("../../examples/simple/simulation", [f"{vals}", f"{n_iters}", f"{delay}"], procmap, namemap)
        snd = h.Puppet("../../tools/send", ["--async", "consumer", "t:int", "data:array"], procmap, namemap)

        sim.proceed()

        while(sim.running()):
            snd.proceed()
            sim.proceed()

        snd.signal_stop()
        snd.proceed()
    elif procmap.group() == "consumer":
        rcv = h.Puppet("../../tools/receive", ["--async", "producer" , "t:int", "data:array"], procmap, namemap)
        ana = h.Puppet("../../examples/simple/analysis", ["3"], procmap, namemap)

        # print(f"In consumer, calling rcv.proceed")
        rcv.proceed()

        while(rcv.running()):
            ana.proceed()
            rcv.proceed()

        if (procmap.local_rank() == 0):
            s = namemap.get("sum")
            print(f"Final sum: {s}")
            return s
    else:
        print("I'm not part of producer or consumer group!!! Error!")

procmap = None
mpi_world = None

def call_me():
    global procmap
    global mpi_world

    procmap = h.ProcMap(mpi_world, [("world", mpi_world.size)])

    mpi_world = MPI.COMM_WORLD.Dup()

    sched = h.Scheduler(procmap, 1)

    if sched.is_controller():
        for i in range(0, 5):
            sched.schedule(f"{i}", "world_func", [250, 3, 1], {"producer" : 0, "consumer" : 0}, sched.workers())

        while sched.control():
            pass

        sched.finish()

        while not sched.results_empty():
            x = sched.pop()
            print(f"Got result: {x}")
    else:
        sched.listen()

# del sched

if __name__ == "__main__":
    call_me()


# ProcMap

ProcMap maintains a vector of levels (`Levels`), which represents a stack of
communicator subdivisions that this rank belongs to. The last entry in the
vector represents the latest, active communicator.

Each level records its `world` and `local` communicator. The former is the
input communicator to the level, typically the `local` communicator from the
previous level (or, initially, a duplicate of `MPI_COMM_WORLD`). This is the
communicator that is split into groups, of which `local` is the communicator of
the group that this rank belongs to. `world` is used to access
inter-communicators between groups.

Besides `world` and `local`, a level stores a group map, which translates from
symbolic names of execution groups to their size and the rank of the leader (in
terms of `world`).

`henson_get_world()` returns `ProcMap::local()` of the last level.

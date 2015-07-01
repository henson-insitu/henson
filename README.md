# Henson

## Problem and Solution

**Original problem.**

**Proposed solution.**

**Unexpected advantage.**

## Code

### Headers

`henson`'s main interface is in C to make it easy to use from C simulation and
analysis routines. Additionally, it provides some C++ helper routines for
moving data across coroutines.

```{.cpp}
#include <henson/context.h>
#include <henson/data.h>
#include <henson/data.hpp>      // C++ functions
```

### MPI Initialization

Since all executables become part of the main `henson` process, MPI must be
initialized only once. `henson` takes care of the initialization, so, when
running under it, puppets cannot initialize MPI themselves.
`henson_get_world()` returns the MPI communicator. Currently, it is not
strictly necessary to call this function under [MPICH](http://www.mpich.org),
where `MPI_COMM_WORLD` is a constant. But it is necessary under
[OpenMPI](http://www.open-mpi.org). And it will become necessary once `henson`
becomes capable of launching different executables on different nodes.
(`henson` will split the world communicator as necessary.)

```{.c}
if (!henson_active())
    MPI_Init(&argc, &argv);

MPI_Comm world = henson_get_world();

...

if (!henson_active())
    MPI_Finalize();
```

### Producer

```{.c}
for (/* every timestep */)
{
    // perform some computation

    henson_save_array(...);
    henson_yield();             // return control back to henson
}
```

### Consumer

Process every time step:
```{.c}
while(1)
{
    henson_load_array(...);

    // process the data
    // if there is any output to pass to other analysis routines downstream,
    // call an appropriate henson_save_...(...)

    henson_yield();             // return control back to henson
}

```

Or process a single snapshot:
```{.c}
float* data;
size_t size;
if (!henson_active())
    // load data from file
else
{
    henson_load_...
    henson_load_...
}

// process the data
```


### Compiling and Linking

`-fPIE -pie`
`-lhenson -Wl,--export-dynamic`


### Sample henson script

```
./simulation 1000
./analysis
```

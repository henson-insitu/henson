# Henson

## Problem and Solution

**Original problem.**
Running under a regular Linux OS, one can launch simulation and analysis as
separate processes and use inter-process communication to synchronize execution.
But this approach does not work on supercomputers. Both Edison at NERSC and
Mira at ALCF (and likely all Crays and IBM BGs) prohibit users from launching
multiple processes on a single node.  Furthermore, both prevent a process from
launching child processes. The standard `fork`/`exec` pattern does not
work: on a Cray, a process can `fork`, but a child that tries to `exec` a
different process gets killed; on a BG/Q, even the `fork` is disabled.

**Proposed solution.**
A convenient solution was [suggested on StackOverflow](http://stackoverflow.com/a/30036251/44738).
If the executables are built as position-independent code, then their `main`s
can be loaded using the [dynamic loading](https://en.wikipedia.org/wiki/Dynamic_loading) facilities `dlopen` and `dlsym`:

```{.cpp}
typedef     int  (*MainType)(int argc, char *argv[]);

void*       lib      = dlopen(fn.c_str(), RTLD_LAZY);
MainType    lib_main = (MainType) dlsym(lib, "main");
```

The `main`s can subsequently be called from a controlling process in the
appropriate order.

**Coroutines.**
A question that remains is how to transfer control between different
executables. For example, a simulation may want to stop after every time-step,
transfer control to the analysis code, but then resume where it left off
without losing its state. Similarly, analysis may not be a single executable,
but a chain (or a DAG) of different executables that transfer control to each
other and resume where they left off when the execution returns.

A convenient way to support such a cooperative multitasking regime is via
[coroutines](https://en.wikipedia.org/wiki/Coroutine).
This is precisely the approach we take, implemented using
[Boost.Context library](http://www.boost.org/doc/libs/1_58_0/libs/context/doc/html/index.html).
Executables call `henson_yield()` when they want to relinquish control back to
the controlling process, `henson`. When not running under `henson`, this
routine does nothing and returns immediately.

**Shared address space.**
The above solution has an unexpected advantage. Since all the routines get
loaded into a shared address space, the data can be exchanged between them
by simply passing pointers around — no copying or special tricks are
required. (Achieving the same zero-copy between separate processes is much more
complicated.)


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
running under it, coroutines cannot initialize MPI themselves.
`henson_get_world()` returns the MPI communicator. Currently, it is not
strictly necessary to call this function under [MPICH](http://www.mpich.org),
where `MPI_COMM_WORLD` is a constant. But it is necessary under
[OpenMPI](http://www.open-mpi.org). And it will become necessary once `henson`
is capable of launching different executables on different nodes.
(`henson` will split the world communicator appropriately.)

```{.c}
if (!henson_active())
    MPI_Init(&argc, &argv);

MPI_Comm world = henson_get_world();

...

if (!henson_active())
    MPI_Finalize();
```

### Control Transfer

A coroutine can transfer control back to `henson` by calling `henson_yield()`.
`henson` in turn decides where to continue execution next. (Current
implementation simply cycles through all coroutines in order until the first
one terminates.)

### Data Exchange

Coroutines can exchange data by posting it to a global namespace. Several
helper functions allow exchanging specific types of data:

  * `henson_{save,load}_array(name, address, type, count, stride)`
  * `henson_{save,load}_pointer(name, ptr)`
  * `henson_{save,load}_size_t(name, size)`

In C++, there are additional functions:

  * `henson::save(name, x)`
  * `template<class T> T* henson::load(name)`

See [include/henson/data.h](include/henson/data.h) and
[include/henson/data.hpp](include/henson/data.hpp)
for more details.

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
    henson_load_array("data", &data, ...);
    henson_load_size_t("size", &size);
}

// process the data
```


### Compiling and Linking

Henson depends on [Boost](http://www.boost.org) `>=1.57`, specifically, the
Boost.Context library, responsible for switching contexts between the
coroutines.

You can build Henson library, executable, and examples using CMake:
```
cmake .../path/to/henson
make
```

When building your own executables, it's important to pass certain flags to the
compiler and the linker. Compiler needs to prepare position-independent code;
for executables, GCC and Clang need option `-fPIE` to do so.
(In CMake, this options is added automatically when
`CMAKE_POSITION_INDEPENDENT_CODE` is `on`.)

We need to make sure the linker exposes all symbols that `henson` needs. Under
Mac OS X, this seems to happen automatically, but under Linux we need
additional flags. At a minimum we need to add `-pie -Wl,--export-dynamic`.
Sometimes we have to add `-Wl,-u,henson_set_contexts,-u,henson_set_namemap` to
force the linker to export functions even if they are not called within the
executable itself.

In general, getting the linker flags right for the executables is the most
complicated part of using Henson.

### Sample henson script

See [simulation.c](simulation.c) and [analysis.cpp](analysis.cpp) for sample code.

```
sample.hwl:
./simulation 1000
./analysis

mpirun -n 4 henson sample.hwl
```

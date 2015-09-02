# Henson

## Contents

  * [Example](#markdown-header-example)
  * [Problem and Solution](#markdown-header-problem-and-solution)
  * [Code](#markdown-header-code)
    - [Headers](#markdown-header-headers)
    - [MPI Initialization](#markdown-header-mpi-initialization)
    - [Control Transfer](#markdown-header-control-transfer)
    - [Data Exchange](#markdown-header-data-exchange)
    - [Execution Groups][]
  * [HWL][]
  * [Compiling and Linking](#markdown-header-compiling-and-linking)

[Execution Groups]:   #markdown-header-execution-groups
[HWL]:                #markdown-header-hwl

## Example

See [simulation.c][] and [analysis.cpp][] in [examples/simple](examples/simple) for sample code.

[simulation.c]:     examples/simple/simulation.c
[analysis.cpp]:     examples/simple/analysis.cpp

```
simple.hwl:
sim = ./simulation 1000
ana = ./analysis

world while sim:
    sim
    ana


mpirun -n 4 henson simple.hwl
```

Other examples:

  * [examples/intercomm](examples/intercomm) illustrates multiple execution groups;
  * [examples/async](examples/async) does the same, but with asynchronous data exchange.

## Problem and Solution

**Original problem.**
Running under a regular Linux OS, one can launch simulation and analysis as
separate processes and use inter-process communication to synchronize execution.
But this approach does not work on supercomputers. Both Edison at NERSC and
Mira at ALCF (and likely all Crays and IBM BGs) prohibit users from launching
multiple executables on a single node.  Furthermore, both prevent a process from
launching child processes. The standard `fork`/`exec` pattern does not
work: on a Cray, a process can `fork`, but a child that tries to `exec` a
different executable gets killed; on a BG/Q, even the `fork` is disabled.

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
How can we transfer control between different `main`s?
A simulation may want to stop after every time-step,
transfer control to the analysis code, but then resume where it left off
without losing its state. Similarly, analysis may not be a single executable,
but a chain (or a DAG) of different executables that transfer control to each
other and resume where they left off when the execution returns.

A convenient way to support such a cooperative multitasking regime is via
[coroutines](https://en.wikipedia.org/wiki/Coroutine).
We implement this approach using
[Boost.Context library](http://www.boost.org/doc/libs/1_58_0/libs/context/doc/html/index.html).
Executables call `henson_yield()` when they want to relinquish control back to
`henson`. When not running under `henson`, this
routine does nothing and returns immediately.

**Shared address space.**
The above solution has an unexpected advantage. Since all the routines get
loaded into the same address space, the data can be exchanged between them
by simply passing pointers around — no copying or special tricks are
required. (Achieving the same zero-copy between separate processes is much more
complicated.)


## Code

### Headers

`henson`'s main interface is in C to make it easy to use from C simulation and
analysis routines. Additionally, it provides some C++ helper functions for
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
`henson_get_world()` returns the MPI communicator.
(This communicator may be different from `MPI_COMM_WORLD` if multiple
[execution groups][Execution Groups] are used.)

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
`henson` in turn decides where to continue execution next. `henson` cycles
through all coroutines in an [execution group][Execution Groups] in order until
the controlling coroutine specified in the `while`-clause terminates. Once this happens, every
remaining coroutine in the group loop gets called once. A coroutine may check
if its execution group is about to stop (i.e., if a controlling coroutine has
indicated that it's done) by calling `henson_stop()`.

### Data Exchange

Coroutines can exchange data by posting it to a global namespace. Several
helper functions allow exchanging specific types of data:

  * `henson_{save,load}_array(name, address, type, count, stride)`
  * `henson_{save,load}_pointer(name, ptr)`
  * `henson_{save,load}_size_t(name, size)`
  * `henson_{save,load}_int(name, x)`

In C++, there are additional functions:

  * `henson::save(name, x)`
  * `template<class T> T* henson::load(name)`
  * `henson::exists(name)`

See [include/henson/data.h](include/henson/data.h) and
[include/henson/data.hpp](include/henson/data.hpp)
for more details.

#### Producer

```{.c}
for (/* every timestep */)
{
    // perform some computation

    henson_save_array(...);
    henson_yield();             // return control back to henson
}
```

#### Consumer

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
Note that the non-controlling coroutine will be restarted over and over again
as long the controlling coroutine is running. So if the producer generates data
over multiple time-steps, it's fine to supply a consumer that processes only
a single snapshot; it will be restarted automatically. See [HWL][].


### Execution Groups

Different sets of processors may run through different coroutine loops. For
example, the simulation and analysis may run on different processes;
they could switch contexts with additional coroutines that exchange the data
between the groups. The user may specify the different execution groups in the
script supplied to `henson`, specifying the number of processors to dedicate to
each group on the command-line. (If no group sizes are specified, the processes
are split evenly between the groups.)
[examples/intercomm/intercomm.hwl](examples/intercomm/intercomm.hwl) implements
such a pattern. Instead of switching directly to [analysis.cpp][],
[simulation.c][] switches to [send.cpp][], which sends its data over MPI to
[receive.cpp][] on a different set of processes; [receive.cpp][] in turn
switches to [analysis.cpp][].

[send.cpp]:     tools/send.cpp
[receive.cpp]:  tools/receive.cpp

`henson_get_world()` returns the communicator restricted to each execution
group. To communicate across groups, [send.cpp][] and [receive.cpp][] use
`henson_get_intercomm(group_name)` to get the appropriate MPI inter-communicator.

Section [HWL][] describes how to specify execution groups in the scripts
supplied to `henson`.

## HWL

The following annotated example
(original in [intercomm.hwl](examples/intercomm/intercomm.hwl))
illustrates the syntax of the scripts supplied to henson.
(A simpler example is [simple.hwl](examples/simple/simple.hwl).)

First, the script specifies the command lines to run and assigns them names.
These commands become the coroutines. Command lines may contain variables (with
defaults in parentheses); the variable values can be supplied to `henson` on
its command line.
```
sim = ../simple/simulation $size(250)
snd = ../../tools/send      consumer t:int data:array
rcv = ../../tools/receive   producer t:int data:array
ana = ../simple/analysis
```

Next, the script specifies two execution groups, `producer` and `consumer`. The
former cycles through coroutines `sim` and `snd`; the latter cycles through
`rcv` and `ana`.
```
producer while sim:
	sim
	snd

consumer while rcv:
	rcv
	ana
```

The script specifies, via the `while`-clause, that `sim` and `rcv` control execution. I.e.,
`producer` group will stop when `sim` stops. `consumer` group stops when `rcv`
does. Notice that for `rcv` to find out that no more data will arrive, `snd`
needs to send it the appropriate message. Accordingly, [send.cpp][] checks whether
`producer` group is stopping by calling `henson_stop()`, and sends the stop
message when this happens.


## Compiling and Linking

Henson depends on [Boost](http://www.boost.org) `>=1.58`, specifically, the
Boost.Context library, responsible for switching contexts between the
coroutines.

You can build Henson library, executable, and examples using CMake:
```
cmake .../path/to/henson
make
```

When building your own executables, it's important to pass certain flags to the
compiler and to the linker. Compiler needs to prepare position-independent code;
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

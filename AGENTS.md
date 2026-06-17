# AGENTS.md

## Build
- Use an out-of-tree CMake build: `cmake -S . -B build`, then `cmake --build build`.
- CI also verifies the Boost.Context path separately: `cmake -S . -B build-boost -Duse_boost=on` then `cmake --build build-boost`.
- Default CMake options are `python=ON`, `mpi-wrappers=ON`, `regenerate-wrapper=ON`, `henson_backward=ON`, `use_boost=OFF`; disable Python with `-Dpython=off` when you only need core C++/MPI changes.
- CMake defaults to `Release` if `CMAKE_BUILD_TYPE` is unset and uses C++14.

## Verification
- There is no CTest setup for first-party code. After building, run the MPI smoke tests from the build tree: `cd build/examples && ./run_test.sh`.
- `examples/run_test.sh` exercises only `simple`, `intercomm`, and `async`; it runs both `henson-hwl` and `henson-chai` under `mpirun` with 2, 4, and 8 ranks.
- For focused manual checks, run built examples from their build subdirectory, e.g. `mpirun -n 4 ../../henson-hwl simple.hwl` or `mpirun -n 4 ../../henson-chai simple.chai -p world` inside `build/examples/simple`.

## Project Shape
- Main executables are root files: `henson-hwl.cpp`, `henson-chai.cpp`, and `henson-exec.cpp`.
- Core library code is in `src/context.cpp`, `src/data.cpp`, and headers under `include/henson/`; ChaiScript integration is under `src/chai/` and `include/henson/chai/`.
- `bindings/python/` builds the `pyhenson` module only when CMake option `python` is enabled.
- `ext/` is vendored third-party code (`fmt`, `pybind11`, `spdlog`, `backward-cpp`); do not treat its tests or configs as repo-level workflow unless you are changing the vendored dependency itself.

## Henson-Specific Gotchas
- Henson loads other programs' `main` functions into one process; example puppets built with `add_henson_executable` become extensionless shared objects on non-Apple platforms, not normal executables.
- Loaded puppets must be position-independent and export Henson symbols; preserve the `add_henson_executable` linker flags (`--export-dynamic` and forced `henson_set_contexts`/`henson_set_namemap` symbols) when adding examples/tools.
- MPI is initialized/finalized by Henson. Puppet code should use Henson's MPI behavior, not call `MPI_Init`/`MPI_Finalize` itself.
- With `mpi-wrappers=ON`, `restrict-world.c` is generated from `src/restrict-world.w` by `src/wrap.py` unless `-Dregenerate-wrapper=off` is used, in which case the checked-in `src/restrict-world.c` is copied.
- `henson_get_world()` returns the current execution group's communicator; cross-group communication uses `henson_get_intercomm(group_name)`.

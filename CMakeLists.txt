project                     (Henson)
cmake_minimum_required      (VERSION 3.9)

option                      (use_boost          "Use Boost.Context"              OFF)
option                      (python             "Build Python bindings"          ON)
option                      (mpi-wrappers       "Include MPI wrappers"           ON)
option                      (regenerate-wrapper "Regenerate MPI wrapper"         ON)

# Default to Release
if                          (NOT CMAKE_BUILD_TYPE)
    set                     (CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
    set_property            (CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif                       (NOT CMAKE_BUILD_TYPE)

set                         (CMAKE_POSITION_INDEPENDENT_CODE ON)

# Boost
if                          (use_boost)
    find_package            (Boost COMPONENTS context)
    include_directories     (SYSTEM ${Boost_INCLUDE_DIR})
endif()

# MPI
find_package                (MPI REQUIRED)
set                         (mpi_libraries MPI::MPI_C MPI::MPI_CXX)

# DL
set                         (libraries ${libraries} ${CMAKE_DL_LIBS})

# Threads
find_package                (Threads)
set                         (libraries ${libraries} ${CMAKE_THREAD_LIBS_INIT})

include_directories         (${CMAKE_CURRENT_BINARY_DIR}
                             ${CMAKE_CURRENT_SOURCE_DIR}
                             ${CMAKE_CURRENT_SOURCE_DIR}/include
                             SYSTEM ${MPI_C_INCLUDE_PATH})

if                          (NOT use_boost)
    set                     (CORO_TYPE "-DCORO_SJLJ")
    if                      (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
        set                 (CORO_TYPE "-DCORO_ASM")
    endif()

    # Libcoro
    add_library             (coro src/coro.c)
    set_target_properties   (coro PROPERTIES COMPILE_FLAGS ${CORO_TYPE})

    set                     (HENSON_LIB_SOURCES src/coro.c)
endif()

if                          (python)
    add_subdirectory        (ext/pybind11)
    add_definitions         (-DHENSON_PYTHON)
endif()

set                         (CMAKE_CXX_STANDARD 14)

# Format
add_subdirectory            (ext/fmt)
include_directories         (ext/fmt)

set                         (HENSON_LIB_SOURCES src/context.cpp src/data.cpp ${HENSON_LIB_SOURCES})
add_library                 (henson_lib ${HENSON_LIB_SOURCES})
set_target_properties       (henson_lib PROPERTIES OUTPUT_NAME henson)
if                          (use_boost)
    set_target_properties   (henson_lib PROPERTIES COMPILE_FLAGS "-DUSE_BOOST")
else()
    set_target_properties   (henson_lib PROPERTIES COMPILE_FLAGS ${CORO_TYPE})
endif()

# MPI wrapper
if                          (mpi-wrappers)
if                          (regenerate-wrapper)
    set                     (WRAP ${PROJECT_SOURCE_DIR}/src/wrap.py)
    include                 (${PROJECT_SOURCE_DIR}/src/WrapConfig.cmake)
    add_wrapped_file        (restrict-world.c src/restrict-world.w)
else()
  add_custom_command        (OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/restrict-world.c
                             COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/src/restrict-world.c ${CMAKE_CURRENT_BINARY_DIR}/restrict-world.c
                             MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/src/restrict-world.c)
endif()
add_library                 (henson-pmpi SHARED restrict-world.c)
target_link_libraries       (henson-pmpi henson_lib ${mpi_libraries})
set                         (libraries ${libraries} henson-pmpi)
add_library                 (henson-pmpi-static STATIC restrict-world.c)
else()
set                         (libraries henson_lib ${libraries} ${mpi_libraries})
endif()


# git version
include                     (git_watcher.cmake)

add_executable              (henson-hwl  henson-hwl.cpp)
add_executable              (henson-chai henson-chai.cpp
                                         ${CMAKE_CURRENT_BINARY_DIR}/src/version.cpp
                                         src/chai/puppet.cpp
                                         src/chai/data.cpp
                                         src/chai/procs.cpp
                                         src/chai/scheduler.cpp
                                         src/chai/util.cpp
                                         src/chai/stdlib.cpp)
add_executable              (henson-exec henson-exec.cpp
                                         ${CMAKE_CURRENT_BINARY_DIR}/src/version.cpp)
target_link_libraries       (henson-hwl  PRIVATE fmt ${libraries})
target_link_libraries       (henson-chai PRIVATE fmt ${libraries})
target_link_libraries       (henson-exec PRIVATE fmt ${libraries})
if                          (python)
    target_link_libraries(henson-chai PRIVATE pybind11::embed)
endif()

if                          (use_boost)
    target_link_libraries   (henson-hwl PRIVATE ${Boost_CONTEXT_LIBRARY})
    target_link_libraries   (henson_lib PRIVATE ${Boost_CONTEXT_LIBRARY})
    target_link_libraries   (henson-chai PRIVATE ${Boost_CONTEXT_LIBRARY})
    target_link_libraries   (henson-exec PRIVATE ${Boost_CONTEXT_LIBRARY})
    set_target_properties   (henson-hwl  PROPERTIES COMPILE_FLAGS "-DUSE_BOOST")
    set_target_properties   (henson-chai PROPERTIES COMPILE_FLAGS "-DUSE_BOOST")
    set_target_properties   (henson-exec PROPERTIES COMPILE_FLAGS "-DUSE_BOOST")
else()
    target_link_libraries   (henson-hwl  PRIVATE coro)
    set_target_properties   (henson-hwl  PROPERTIES COMPILE_FLAGS "${CORO_TYPE}")
    target_link_libraries   (henson-chai PRIVATE coro)
    set_target_properties   (henson-chai PROPERTIES COMPILE_FLAGS "${CORO_TYPE}")
    target_link_libraries   (henson-exec PRIVATE coro)
    set_target_properties   (henson-exec PROPERTIES COMPILE_FLAGS "${CORO_TYPE}")
endif()

add_dependencies            (henson-chai AlwaysCheckGit)
add_dependencies            (henson-exec AlwaysCheckGit)


add_custom_command          (OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/examples/run_test.sh
                             COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/examples/run_test.sh ${CMAKE_CURRENT_BINARY_DIR}/examples/run_test.sh
                             MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/examples/run_test.sh)
add_custom_target           (run_test.sh ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/examples/run_test.sh)

# Helper to compile Henson executables
function (add_henson_executable name sources)
    if                  (APPLE)
        add_executable  (${name} ${sources})
    else                ()
        #add_executable          (${name} ${sources})
        add_library             (${name} SHARED ${sources})
        set_target_properties   (${name} PROPERTIES PREFIX "")
        set_target_properties   (${name} PROPERTIES SUFFIX "")

        #set                      (linker_flags "-pie -Wl,--export-dynamic")
        set                     (linker_flags "-Wl,--export-dynamic")
        set                     (linker_flags "${linker_flags} -Wl,-u,henson_set_contexts,-u,henson_set_namemap")
        set_target_properties   (${name} PROPERTIES LINK_FLAGS ${linker_flags})
    endif               ()
endfunction()

add_subdirectory            (examples/simple)
add_subdirectory            (examples/intercomm)
add_subdirectory            (examples/async)
add_subdirectory            (examples/fair)
add_subdirectory            (examples/storage)
add_subdirectory            (examples/queue)
add_subdirectory            (examples/embed-python)
add_subdirectory            (tools)

if                          (python)
    add_subdirectory        (bindings/python)
endif                       (python)


# henson-chai + henson_lib
install                     (TARGETS henson-chai henson_lib henson-exec
                                RUNTIME DESTINATION bin
                                LIBRARY DESTINATION lib
                                ARCHIVE DESTINATION lib)

if                          (mpi-wrappers)
    # libraries
    install                 (TARGETS henson-pmpi henson-pmpi-static
                                LIBRARY DESTINATION lib
                                ARCHIVE DESTINATION lib)
endif                       (mpi-wrappers)

# headers
foreach                     (h data.h data.hpp context.h context.hpp coro.h
                               procs.hpp coroutine.hpp puppet.hpp python-puppet.hpp
                               time.hpp variant.hpp serialization.hpp)
    install                 (FILES  ${CMAKE_SOURCE_DIR}/include/henson/${h}
                             DESTINATION include/henson)
endforeach                  ()

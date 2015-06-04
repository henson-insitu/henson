project                     (Henson)
cmake_minimum_required      (VERSION 2.8.11)

set                         (CMAKE_POSITION_INDEPENDENT_CODE ON)
set                         (linker_flags "-pie -Wl,--export-dynamic")

# Boost
find_package                (Boost COMPONENTS context)
include_directories         (SYSTEM ${Boost_INCLUDE_DIR})


# MPI
find_package                (MPI REQUIRED)
set                         (mpi_libraries              ${MPI_C_LIBRARIES} ${MPI_CXX_LIBRARIES})
set                         (libraries ${libraries}     ${mpi_libraries})

# DL
find_library                (DL_LIBRARY     dl)
set                         (libraries ${libraries} ${DL_LIBRARY})

include_directories         (${CMAKE_CURRENT_BINARY_DIR}
                             ${CMAKE_CURRENT_SOURCE_DIR}
                             ${CMAKE_CURRENT_SOURCE_DIR}/include
                             SYSTEM ${MPI_INCLUDE_PATH})

add_executable              (simulation simulation.c src/context.cpp)
target_link_libraries       (simulation ${mpi_libraries} ${Boost_CONTEXT_LIBRARY})
set_target_properties       (simulation PROPERTIES LINK_FLAGS ${linker_flags})

add_executable              (analysis analysis.cpp src/context.cpp)
target_link_libraries       (analysis ${mpi_libraries} ${Boost_CONTEXT_LIBRARY})
set_target_properties       (analysis PROPERTIES LINK_FLAGS ${linker_flags})

add_executable              (henson henson.cpp src/format.cc)
target_link_libraries       (henson ${libraries} ${Boost_CONTEXT_LIBRARY})

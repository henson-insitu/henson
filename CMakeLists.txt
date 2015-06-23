project                     (Henson)
cmake_minimum_required      (VERSION 2.8.11)

set                         (CMAKE_POSITION_INDEPENDENT_CODE ON)

# Boost
find_package                (Boost COMPONENTS context)
include_directories         (SYSTEM ${Boost_INCLUDE_DIR})


# MPI
find_package                (MPI REQUIRED)
set                         (mpi_libraries              ${MPI_C_LIBRARIES} ${MPI_CXX_LIBRARIES})
set                         (libraries ${libraries}     ${mpi_libraries})

# DL
set                         (libraries ${libraries} ${CMAKE_DL_LIBS})

include_directories         (${CMAKE_CURRENT_BINARY_DIR}
                             ${CMAKE_CURRENT_SOURCE_DIR}
                             ${CMAKE_CURRENT_SOURCE_DIR}/include
                             SYSTEM ${MPI_INCLUDE_PATH})

add_library                 (henson_lib src/context.cpp)
set_target_properties       (henson_lib PROPERTIES OUTPUT_NAME henson)

add_executable              (simulation simulation.c)
target_link_libraries       (simulation henson_lib ${mpi_libraries} ${Boost_CONTEXT_LIBRARY})

add_executable              (analysis analysis.cpp)
target_link_libraries       (analysis henson_lib ${mpi_libraries} ${Boost_CONTEXT_LIBRARY})

add_executable              (henson henson.cpp src/format.cc)
target_link_libraries       (henson ${libraries} ${Boost_CONTEXT_LIBRARY})

if                          (NOT APPLE)
   set                      (linker_flags "-pie -Wl,--export-dynamic")
   set_target_properties    (simulation PROPERTIES LINK_FLAGS ${linker_flags})
   set_target_properties    (analysis PROPERTIES LINK_FLAGS ${linker_flags})
endif                       (NOT APPLE)


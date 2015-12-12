project                     (Henson)
cmake_minimum_required      (VERSION 2.8.11)

option                      (use_boost          "Use Boost.Context"              OFF)
option                      (python             "Build Python bindings"          ON)

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
set                         (mpi_libraries              ${MPI_C_LIBRARIES} ${MPI_CXX_LIBRARIES})
set                         (libraries ${libraries}     ${mpi_libraries})

# DL
set                         (libraries ${libraries} ${CMAKE_DL_LIBS})

include_directories         (${CMAKE_CURRENT_BINARY_DIR}
                             ${CMAKE_CURRENT_SOURCE_DIR}
                             ${CMAKE_CURRENT_SOURCE_DIR}/include
                             SYSTEM ${MPI_INCLUDE_PATH})

if                          (NOT use_boost)
    set                     (CORO_TYPE "-DCORO_SJLJ")
    if                      (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
        set                 (CORO_TYPE "-DCORO_ASM")
    endif()

    # Libcoro
    add_library             (libcoro src/coro.c)
    set_target_properties   (libcoro PROPERTIES COMPILE_FLAGS ${CORO_TYPE})

    set                     (HENSON_LIB_SOURCES src/coro.c)
endif()

# Format
add_library                 (format    src/format.cc)

set                         (HENSON_LIB_SOURCES src/context.cpp src/data.cpp ${HENSON_LIB_SOURCES})
add_library                 (henson_lib ${HENSON_LIB_SOURCES})
set_target_properties       (henson_lib PROPERTIES OUTPUT_NAME henson)
if                          (use_boost)
    set_target_properties   (henson_lib PROPERTIES COMPILE_FLAGS "-DUSE_BOOST")
else()
    set_target_properties   (henson_lib PROPERTIES COMPILE_FLAGS ${CORO_TYPE})
endif()

add_executable              (henson henson.cpp)
target_link_libraries       (henson format ${libraries})

if                          (use_boost)
    target_link_libraries   (henson     ${Boost_CONTEXT_LIBRARY})
    target_link_libraries   (henson_lib ${Boost_CONTEXT_LIBRARY})
    set_target_properties   (henson PROPERTIES COMPILE_FLAGS "-std=c++11 -DUSE_BOOST")
else()
    target_link_libraries   (henson libcoro)
    set_target_properties   (henson PROPERTIES COMPILE_FLAGS "-std=c++11 ${CORO_TYPE}")
endif()

add_subdirectory            (examples/simple)
add_subdirectory            (examples/intercomm)
add_subdirectory            (examples/async)
add_subdirectory            (examples/fair)
add_subdirectory            (tools)

if                          (python)
    add_subdirectory        (bindings/python)
endif                       (python)

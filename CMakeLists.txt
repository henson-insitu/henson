project                     (Henson)
cmake_minimum_required      (VERSION 2.8.11)

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
set                         (mpi_libraries              ${MPI_C_LIBRARIES} ${MPI_CXX_LIBRARIES})
set                         (libraries ${libraries}     ${mpi_libraries})

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
set                         (MPI_WRAPPER_SOURCES    restrict-world.c)
endif()

# DL
set                         (libraries ${libraries} ${CMAKE_DL_LIBS})

# Threads
find_package                (Threads)
set                         (libraries ${libraries} ${CMAKE_THREAD_LIBS_INIT})

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
    add_library             (coro src/coro.c)
    set_target_properties   (coro PROPERTIES COMPILE_FLAGS ${CORO_TYPE})

    set                     (HENSON_LIB_SOURCES src/coro.c)
endif()

set                         (CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")

# Format
add_library                 (format    src/format.cc)

set                         (HENSON_LIB_SOURCES src/context.cpp src/data.cpp ${MPI_WRAPPER_SOURCES} ${HENSON_LIB_SOURCES})
add_library                 (henson_lib ${HENSON_LIB_SOURCES})
set_target_properties       (henson_lib PROPERTIES OUTPUT_NAME henson)
if                          (use_boost)
    set_target_properties   (henson_lib PROPERTIES COMPILE_FLAGS "-DUSE_BOOST")
else()
    set_target_properties   (henson_lib PROPERTIES COMPILE_FLAGS ${CORO_TYPE})
endif()

add_executable              (henson-hwl  henson-hwl.cpp)
add_executable              (henson-chai henson-chai.cpp)
target_link_libraries       (henson-hwl  format ${libraries})
target_link_libraries       (henson-chai format ${libraries})

if                          (use_boost)
    target_link_libraries   (henson-hwl ${Boost_CONTEXT_LIBRARY})
    target_link_libraries   (henson_lib ${Boost_CONTEXT_LIBRARY})
    target_link_libraries   (henson-chai     ${Boost_CONTEXT_LIBRARY})
    set_target_properties   (henson PROPERTIES COMPILE_FLAGS "-DUSE_BOOST")
    set_target_properties   (henson-chai PROPERTIES COMPILE_FLAGS "-DUSE_BOOST")
else()
    target_link_libraries   (henson-hwl  coro)
    set_target_properties   (henson-hwl  PROPERTIES COMPILE_FLAGS "${CORO_TYPE}")
    target_link_libraries   (henson-chai coro)
    set_target_properties   (henson-chai PROPERTIES COMPILE_FLAGS "${CORO_TYPE}")
endif()


add_custom_command          (OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/examples/run_test.sh
                             COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/examples/run_test.sh ${CMAKE_CURRENT_BINARY_DIR}/examples/run_test.sh
                             MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/examples/run_test.sh)
add_custom_target           (run_test.sh ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/examples/run_test.sh)

add_subdirectory            (examples/simple)
add_subdirectory            (examples/intercomm)
add_subdirectory            (examples/async)
add_subdirectory            (examples/fair)
add_subdirectory            (examples/storage)
add_subdirectory            (tools)

if                          (python)
    add_subdirectory        (bindings/python)
endif                       (python)

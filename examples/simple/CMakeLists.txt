add_henson_executable       (simulation simulation.c)
target_link_libraries       (simulation henson_lib ${mpi_libraries})

add_henson_executable       (analysis analysis.cpp)
target_link_libraries       (analysis henson_lib ${mpi_libraries})

add_custom_target           (simple.hwl ALL ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/simple.hwl
                             ${CMAKE_CURRENT_BINARY_DIR}/simple.hwl DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/simple.hwl)

add_custom_target           (simple.chai ALL ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/simple.chai
                             ${CMAKE_CURRENT_BINARY_DIR}/simple.chai DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/simple.chai)

add_custom_target           (simple-sched.chai ALL ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/simple-sched.chai
                             ${CMAKE_CURRENT_BINARY_DIR}/simple-sched.chai DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/simple-sched.chai)

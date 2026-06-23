add_henson_executable       (pmpi-world-check pmpi-world-check.c)
target_link_libraries       (pmpi-world-check henson_lib ${mpi_libraries})

add_custom_target           (pmpi-world.hwl ALL ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/pmpi-world.hwl
                             ${CMAKE_CURRENT_BINARY_DIR}/pmpi-world.hwl DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/pmpi-world.hwl)

add_custom_target           (pmpi-world.chai ALL ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/pmpi-world.chai
                             ${CMAKE_CURRENT_BINARY_DIR}/pmpi-world.chai DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/pmpi-world.chai)

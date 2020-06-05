add_henson_executable       (exchange exchange.c)
target_link_libraries       (exchange henson_lib ${mpi_libraries})

add_custom_target           (exchange.chai ALL ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/exchange.chai
                             ${CMAKE_CURRENT_BINARY_DIR}/exchange.chai DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/exchange.chai)

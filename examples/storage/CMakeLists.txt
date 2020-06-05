add_henson_executable       (storage storage.cpp)
target_link_libraries       (storage henson_lib fmt ${mpi_libraries})

add_henson_executable       (compute compute.cpp)
target_link_libraries       (compute henson_lib fmt ${mpi_libraries})

add_custom_target           (storage.chai ALL ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/storage.chai
                             ${CMAKE_CURRENT_BINARY_DIR}/storage.chai DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/storage.chai)

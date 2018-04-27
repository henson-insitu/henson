add_executable              (storage storage.cpp)
target_link_libraries       (storage henson_lib fmt ${mpi_libraries})

add_executable              (compute compute.cpp)
target_link_libraries       (compute henson_lib fmt ${mpi_libraries})

if                          (NOT APPLE)
   set                      (linker_flags "-pie -Wl,--export-dynamic")
   set                      (linker_flags "${linker_flags} -Wl,-u,henson_set_contexts,-u,henson_set_namemap")
   set_target_properties    (storage PROPERTIES LINK_FLAGS ${linker_flags})
   set_target_properties    (compute PROPERTIES LINK_FLAGS ${linker_flags})
endif                       (NOT APPLE)

add_custom_target           (storage.chai ALL ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/storage.chai
                             ${CMAKE_CURRENT_BINARY_DIR}/storage.chai DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/storage.chai)

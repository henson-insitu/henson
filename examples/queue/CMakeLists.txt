add_executable              (exchange exchange.c)
target_link_libraries       (exchange henson_lib ${mpi_libraries})

if                          (NOT APPLE)
   set                      (linker_flags "-pie -Wl,--export-dynamic")
   set                      (linker_flags "${linker_flags} -Wl,-u,henson_set_contexts,-u,henson_set_namemap")
   set_target_properties    (exchange PROPERTIES LINK_FLAGS ${linker_flags})
endif                       (NOT APPLE)

add_custom_target           (exchange.chai ALL ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/exchange.chai
                             ${CMAKE_CURRENT_BINARY_DIR}/exchange.chai DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/exchange.chai)

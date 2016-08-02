add_executable              (simulation simulation.c)
target_link_libraries       (simulation henson_lib ${mpi_libraries})

add_executable              (analysis analysis.cpp)
target_link_libraries       (analysis henson_lib ${mpi_libraries})

if                          (NOT APPLE)
   set                      (linker_flags "-pie -Wl,--export-dynamic")
   set                      (linker_flags "${linker_flags} -Wl,-u,henson_set_contexts,-u,henson_set_namemap")
   set_target_properties    (simulation PROPERTIES LINK_FLAGS ${linker_flags})
   set_target_properties    (analysis   PROPERTIES LINK_FLAGS ${linker_flags})
endif                       (NOT APPLE)

add_custom_target           (simple.hwl ALL ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/simple.hwl
                             ${CMAKE_CURRENT_BINARY_DIR}/simple.hwl DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/simple.hwl)

add_custom_target           (simple.chai ALL ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/simple.chai
                             ${CMAKE_CURRENT_BINARY_DIR}/simple.chai DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/simple.chai)

add_custom_target           (simple_sched.chai ALL ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/simple_sched.chai
                             ${CMAKE_CURRENT_BINARY_DIR}/simple_sched.chai DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/simple_sched.chai)

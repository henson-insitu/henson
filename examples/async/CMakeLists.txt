add_executable              (send-async send-async.cpp)
target_link_libraries       (send-async henson_lib ${mpi_libraries} ${Boost_CONTEXT_LIBRARY})

add_executable              (receive-async receive-async.cpp)
target_link_libraries       (receive-async henson_lib ${mpi_libraries} ${Boost_CONTEXT_LIBRARY})

if                          (NOT APPLE)
   set                      (linker_flags "-pie -Wl,--export-dynamic")
   set                      (linker_flags "${linker_flags} -Wl,-u,henson_set_contexts,-u,henson_set_namemap")
   set_target_properties    (send-async      PROPERTIES LINK_FLAGS ${linker_flags})
   set_target_properties    (receive-async   PROPERTIES LINK_FLAGS ${linker_flags})
endif                       (NOT APPLE)

add_custom_command          (OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/async.hwl
                             COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/async.hwl ${CMAKE_CURRENT_BINARY_DIR}/async.hwl
                             MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/async.hwl)
add_custom_target           (async.hwl ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/async.hwl)

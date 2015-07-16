add_executable              (send send.cpp)
target_link_libraries       (send henson_lib ${mpi_libraries} ${Boost_CONTEXT_LIBRARY})

add_executable              (receive receive.cpp)
target_link_libraries       (receive henson_lib ${mpi_libraries} ${Boost_CONTEXT_LIBRARY})

if                          (NOT APPLE)
   set                      (linker_flags "-pie -Wl,--export-dynamic")
   set                      (linker_flags "${linker_flags} -Wl,-u,henson_set_contexts,-u,henson_set_namemap")
   set_target_properties    (send      PROPERTIES LINK_FLAGS ${linker_flags})
   set_target_properties    (receive   PROPERTIES LINK_FLAGS ${linker_flags})
endif                       (NOT APPLE)

add_custom_command          (OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/intercomm.hwl
                             COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/intercomm.hwl ${CMAKE_CURRENT_BINARY_DIR}/intercomm.hwl
                             MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/intercomm.hwl)
add_custom_target           (intercomm.hwl ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/intercomm.hwl)

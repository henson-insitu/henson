add_henson_executable       (send send.cpp)
target_link_libraries       (send henson_lib fmt ${mpi_libraries})

add_henson_executable       (receive receive.cpp)
target_link_libraries       (receive henson_lib fmt ${mpi_libraries})

add_henson_executable       (timer timer.cpp)
target_link_libraries       (timer henson_lib fmt ${mpi_libraries})

if                          (NOT APPLE)
   set                      (linker_flags "-pie -Wl,--export-dynamic")
   set                      (linker_flags "${linker_flags} -Wl,-u,henson_set_contexts,-u,henson_set_namemap")
   set_target_properties    (send           PROPERTIES LINK_FLAGS ${linker_flags})
   set_target_properties    (receive        PROPERTIES LINK_FLAGS ${linker_flags})
   set_target_properties    (timer          PROPERTIES LINK_FLAGS ${linker_flags})
endif                       (NOT APPLE)

# tools
install                     (TARGETS send receive RUNTIME DESTINATION share/henson)

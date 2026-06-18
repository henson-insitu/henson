add_henson_executable       (send send.cpp)
target_link_libraries       (send henson_lib fmt ${mpi_libraries})

add_henson_executable       (receive receive.cpp)
target_link_libraries       (receive henson_lib fmt ${mpi_libraries})

add_henson_executable       (timer timer.cpp)
target_link_libraries       (timer henson_lib fmt ${mpi_libraries})

# tools
install                     (TARGETS send receive RUNTIME DESTINATION share/henson)

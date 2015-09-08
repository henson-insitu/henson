add_executable              (send send.cpp)
target_link_libraries       (send henson_lib format ${mpi_libraries} ${Boost_CONTEXT_LIBRARY})
set_target_properties       (send PROPERTIES COMPILE_FLAGS "-std=c++11")

add_executable              (receive receive.cpp)
target_link_libraries       (receive henson_lib format ${mpi_libraries} ${Boost_CONTEXT_LIBRARY})
set_target_properties       (receive PROPERTIES COMPILE_FLAGS "-std=c++11")

add_executable              (timer timer.cpp)
target_link_libraries       (timer henson_lib format ${mpi_libraries} ${Boost_CONTEXT_LIBRARY})
set_target_properties       (timer PROPERTIES COMPILE_FLAGS "-std=c++11")

if                          (NOT APPLE)
   set                      (linker_flags "-pie -Wl,--export-dynamic")
   set                      (linker_flags "${linker_flags} -Wl,-u,henson_set_contexts,-u,henson_set_namemap")
   set_target_properties    (send       PROPERTIES LINK_FLAGS ${linker_flags})
   set_target_properties    (receive    PROPERTIES LINK_FLAGS ${linker_flags})
   set_target_properties    (timer      PROPERTIES LINK_FLAGS ${linker_flags})
endif                       (NOT APPLE)


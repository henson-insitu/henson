pybind11_add_module         (pyhenson henson.cpp)

target_link_libraries       (pyhenson PRIVATE fmt ${libraries})
if                          (use_boost)
    target_link_libraries   (pyhenson PRIVATE ${Boost_CONTEXT_LIBRARY})
    set_target_properties   (pyhenson PROPERTIES COMPILE_FLAGS "-DUSE_BOOST")
else()
    target_link_libraries   (pyhenson PRIVATE coro)
    set_target_properties   (pyhenson PROPERTIES COMPILE_FLAGS "${CORO_TYPE}")
endif()

add_custom_command          (OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/test.py
                             COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/test.py ${CMAKE_CURRENT_BINARY_DIR}/test.py
                             MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/test.py)
add_custom_target           (test.py ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/test.py)



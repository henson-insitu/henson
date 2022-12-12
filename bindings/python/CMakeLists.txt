list                        (APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
find_package                (MPI4Py)

pybind11_add_module         (pyhenson henson.cpp ${HENSON_BACKWARD_ENABLE})
if                          (henson_backward)
    add_backward                (pyhenson)
endif()

target_link_libraries       (pyhenson PRIVATE fmt ${libraries})
if                          (use_boost)
    target_link_libraries   (pyhenson PRIVATE ${Boost_CONTEXT_LIBRARY})
    set_target_properties   (pyhenson PROPERTIES COMPILE_FLAGS "-DUSE_BOOST")
else()
    target_link_libraries   (pyhenson PRIVATE coro)
    set_target_properties   (pyhenson PROPERTIES COMPILE_FLAGS "${CORO_TYPE}")
endif()

if                          (MPI4Py_FOUND)
    target_compile_definitions  (pyhenson PUBLIC "-DHENSON_MPI4PY")
    target_include_directories  (pyhenson PUBLIC ${MPI4Py_INCLUDE_DIR})
endif()

list                        (APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/ext/pybind11/tools")
find_package                (PythonLibsNew)
string                      (REPLACE ${PYTHON_PREFIX}/ "" PYTHON_SITE_PACKAGES_RELATIVE ${PYTHON_SITE_PACKAGES})

install                     (TARGETS pyhenson
                             LIBRARY DESTINATION ${PYTHON_SITE_PACKAGES_RELATIVE})

add_custom_command          (OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/test.py
                             COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/test.py ${CMAKE_CURRENT_BINARY_DIR}/test.py
                             MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/test.py)
add_custom_target           (test.py ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/test.py)

add_executable              (test_embedded_python test_embedded_python.cpp)
target_link_libraries       (test_embedded_python PRIVATE pybind11::embed fmt)

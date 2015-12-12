# Add a CMake parameter for choosing a desired Python version
set                         (HENSON_PYTHON_VERSION "" CACHE STRING "Python version to use for compiling henson module")

find_package                (PythonLibs   ${HENSON_PYTHON_VERSION} REQUIRED)
find_package                (PythonInterp ${HENSON_PYTHON_VERSION} REQUIRED)

include_directories         (${PYTHON_INCLUDE_DIR} include)

add_library                 (pyhenson SHARED henson.cpp)
target_link_libraries       (pyhenson format ${libraries})
if                          (use_boost)
    target_link_libraries   (pyhenson ${Boost_CONTEXT_LIBRARY})
    set_target_properties   (pyhenson PROPERTIES COMPILE_FLAGS "-std=c++11 -DUSE_BOOST")
else()
    target_link_libraries   (pyhenson libcoro)
    set_target_properties   (pyhenson PROPERTIES COMPILE_FLAGS "-std=c++11 ${CORO_TYPE}")
endif()

foreach                     (m pyhenson)
set_target_properties       (${m} PROPERTIES PREFIX "")
set_target_properties       (${m} PROPERTIES SUFFIX ".so")

# Optimize for a small binary size
if                          (NOT ${U_CMAKE_BUILD_TYPE} MATCHES DEBUG)
  set_target_properties     (${m} PROPERTIES COMPILE_FLAGS "-Os")
endif                       ()

# Strip unnecessary sections of the binary on Linux/Mac OS
if                          (APPLE)
  set_target_properties     (${m} PROPERTIES MACOSX_RPATH ".")
  set_target_properties     (${m} PROPERTIES LINK_FLAGS "-undefined dynamic_lookup -dead_strip")
  if                        (NOT ${U_CMAKE_BUILD_TYPE} MATCHES DEBUG)
      add_custom_command    (TARGET ${m} POST_BUILD COMMAND strip -u -r ${CMAKE_CURRENT_BINARY_DIR}/${m}.so)
  endif                     ()
else                        ()
  if                        (NOT ${U_CMAKE_BUILD_TYPE} MATCHES DEBUG)
    add_custom_command      (TARGET ${m} POST_BUILD COMMAND strip ${CMAKE_CURRENT_BINARY_DIR}/${m}.so)
  endif                     ()
endif                       ()
endforeach                  ()


add_custom_command          (OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/test.py
                             COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/test.py ${CMAKE_CURRENT_BINARY_DIR}/test.py
                             MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/test.py)
add_custom_target           (test.py ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/test.py)

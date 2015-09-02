add_custom_command          (OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/intercomm.hwl
                             COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/intercomm.hwl ${CMAKE_CURRENT_BINARY_DIR}/intercomm.hwl
                             MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/intercomm.hwl)
add_custom_target           (intercomm.hwl ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/intercomm.hwl)

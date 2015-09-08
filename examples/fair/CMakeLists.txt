add_custom_command          (OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/fair.hwl
                             COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/fair.hwl ${CMAKE_CURRENT_BINARY_DIR}/fair.hwl
                             MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/fair.hwl)
add_custom_target           (fair.hwl ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/fair.hwl)

add_custom_command          (OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/async.hwl
                             COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/async.hwl ${CMAKE_CURRENT_BINARY_DIR}/async.hwl
                             MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/async.hwl)
add_custom_target           (async.hwl ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/async.hwl)

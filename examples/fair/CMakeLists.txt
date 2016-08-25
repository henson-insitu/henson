add_custom_target           (fair.hwl ALL ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/fair.hwl
                             ${CMAKE_CURRENT_BINARY_DIR}/fair.hwl DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/fair.hwl)

add_custom_target           (fair.chai ALL ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/fair.chai
                             ${CMAKE_CURRENT_BINARY_DIR}/fair.chai DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/fair.chai)

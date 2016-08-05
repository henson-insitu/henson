add_custom_target           (async.hwl ALL ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/async.hwl
                             ${CMAKE_CURRENT_BINARY_DIR}/async.hwl DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/async.hwl)

add_custom_target           (async.chai ALL ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/async.chai
                             ${CMAKE_CURRENT_BINARY_DIR}/async.chai DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/async.chai)

add_custom_target           (async-sched.chai ALL ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/async-sched.chai
                             ${CMAKE_CURRENT_BINARY_DIR}/async-sched.chai DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/async-sched.chai)

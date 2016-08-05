add_custom_target           (intercomm.hwl ALL ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/intercomm.hwl
                             ${CMAKE_CURRENT_BINARY_DIR}/intercomm.hwl DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/intercomm.hwl)

add_custom_target           (intercomm.chai ALL ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/intercomm.chai
                             ${CMAKE_CURRENT_BINARY_DIR}/intercomm.chai DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/intercomm.chai)


add_custom_target           (intercomm-sched.chai ALL ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/intercomm-sched.chai
                             ${CMAKE_CURRENT_BINARY_DIR}/intercomm-sched.chai DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/intercomm-sched.chai)

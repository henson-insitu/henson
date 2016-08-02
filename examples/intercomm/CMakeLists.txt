add_custom_target           (intercomm.hwl ALL ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/intercomm.hwl
                             ${CMAKE_CURRENT_BINARY_DIR}/intercomm.hwl DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/intercomm.hwl)

add_custom_target           (intercomm.chai ALL ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/intercomm.chai
                             ${CMAKE_CURRENT_BINARY_DIR}/intercomm.chai DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/intercomm.chai)


add_custom_target           (intercomm_sched.chai ALL ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/intercomm_sched.chai
                             ${CMAKE_CURRENT_BINARY_DIR}/intercomm_sched.chai DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/intercomm_sched.chai)

add_custom_target           (intercomm_sched_ext.chai ALL ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/intercomm_sched_ext.chai
                             ${CMAKE_CURRENT_BINARY_DIR}/intercomm_sched_ext.chai DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/intercomm_sched_ext.chai)

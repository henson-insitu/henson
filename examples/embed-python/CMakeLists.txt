foreach                     (t embed-python.chai example.py queue.chai surrogate.py)
    add_custom_target       (${t} ALL ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/${t}
                             ${CMAKE_CURRENT_BINARY_DIR}/${t} DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${t})
endforeach                  ()

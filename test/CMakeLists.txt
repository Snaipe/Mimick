add_custom_target(mimick_tests ALL)

function (add_mimick_test _NAME)
    add_executable (${_NAME} EXCLUDE_FROM_ALL ${ARGN})
    add_dependencies (mimick_tests ${_NAME})
    target_link_libraries (${_NAME} mimick)

    if (USE_QEMU)
        add_test (${_NAME} qemu-${MMK_ARCH} ${_NAME})
    else ()
        add_test (${_NAME} ${_NAME})
    endif ()
endfunction ()

add_mimick_test (malloc malloc.c)

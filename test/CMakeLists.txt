if (NOT MSVC)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pedantic -Werror")
endif ()

add_library (foo SHARED libfoo.c)

add_mimick_test (mmk_test test.c)

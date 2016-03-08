set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pedantic -Werror")

if (UNIX)
    add_mimick_test(strdup strdup.c)
endif ()

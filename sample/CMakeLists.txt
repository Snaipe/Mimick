if (NOT MSVC)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pedantic -Werror")
endif ()

add_subdirectory (strdup)

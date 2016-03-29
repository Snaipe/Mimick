add_library (strdup SHARED strdup.c)
add_executable (strdup_test test.c)
target_link_libraries (strdup_test strdup mimick)

if (NOT MSVC)
    set_source_files_properties (test.c PROPERTIES COMPILE_FLAGS -O0)
endif ()

add_mimick_sample (strdup_test)

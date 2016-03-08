add_library (strdup SHARED strdup.c)
add_executable (strdup_test test.c)
target_link_libraries (strdup_test strdup mimick)

add_mimick_sample (strdup_test)

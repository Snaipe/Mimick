add_library (strdup SHARED strdup.c)
add_executable (strdup_test test.c)
set_source_files_properties (test.c PROPERTIES COMPILE_FLAGS -O0)
target_link_libraries (strdup_test strdup mimick)

add_mimick_sample (strdup_test)

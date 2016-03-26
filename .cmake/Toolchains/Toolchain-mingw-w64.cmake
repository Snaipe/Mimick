# the name of the target operating system
set(CMAKE_SYSTEM_NAME Windows)

if (CMAKE_SYSTEM_PROCESSOR MATCHES "^x86$")
    set (CMAKE_SYSTEM_PROCESSOR i686)
    set (BITS 32)
elseif (CMAKE_SYSTEM_PROCESSOR MATCHES "^x64$")
    set (CMAKE_SYSTEM_PROCESSOR x86_64)
    set (BITS 64)
endif ()

if (WIN32)
    set(CMAKE_FIND_ROOT_PATH "C:/msys64/mingw${BITS}" "C:/msys64/mingw${BITS}/${CMAKE_SYSTEM_PROCESSOR}-w64-mingw32")
    set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM ONLY)
    set(ENV{PATH} "C:\\msys64\\mingw${BITS}\\bin;C:\\msys64\\mingw${BITS}\\${CMAKE_SYSTEM_PROCESSOR}-w64-mingw32\\bin;$ENV{PATH}")
else ()
    set(CMAKE_FIND_ROOT_PATH /usr/${CMAKE_SYSTEM_PROCESSOR}-w64-mingw32)
    set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
endif ()

# which compilers to use for C and C++
set(CMAKE_AR ${CMAKE_SYSTEM_PROCESSOR}-w64-mingw32-gcc-ar CACHE FILEPATH "Archiver")
set(CMAKE_C_COMPILER ${CMAKE_SYSTEM_PROCESSOR}-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER ${CMAKE_SYSTEM_PROCESSOR}-w64-mingw32-g++)
set(CMAKE_RC_COMPILER ${CMAKE_SYSTEM_PROCESSOR}-w64-mingw32-windres)

# adjust the default behaviour of the FIND_XXX() commands:
# search headers and libraries in the target environment, search 
# programs in the host environment
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

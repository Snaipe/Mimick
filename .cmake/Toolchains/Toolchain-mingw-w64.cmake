# the name of the target operating system
SET(CMAKE_SYSTEM_NAME Windows)

if (CMAKE_SYSTEM_PROCESSOR MATCHES "^x86")
    set (CMAKE_SYSTEM_PROCESSOR i686)
    set (BITS 32)
elseif (CMAKE_SYSTEM_PROCESSOR MATCHES "^x64")
    set (CMAKE_SYSTEM_PROCESSOR x86_64)
    set (BITS 64)
endif ()

if (WIN32)
    SET(CMAKE_C_COMPILER "C:/msys64/mingw${BITS}/bin/gcc.exe")
    SET(CMAKE_CXX_COMPILER "C:/msys64/mingw${BITS}/bin/g++.exe")
    SET(CMAKE_RC_COMPILER "C:/msys64/mingw${BITS}/bin/windres.exe")
    SET(CMAKE_FIND_ROOT_PATH "C:/msys64/mingw${BITS}/bin")
else ()
    # which compilers to use for C and C++
    SET(CMAKE_C_COMPILER ${CMAKE_SYSTEM_PROCESSOR}-w64-mingw32-gcc)
    SET(CMAKE_CXX_COMPILER ${CMAKE_SYSTEM_PROCESSOR}-w64-mingw32-g++)
    SET(CMAKE_RC_COMPILER ${CMAKE_SYSTEM_PROCESSOR}-w64-mingw32-windres)

    # here is the target environment located
    SET(CMAKE_FIND_ROOT_PATH /usr/${CMAKE_SYSTEM_PROCESSOR}-w64-mingw32)
endif ()

# adjust the default behaviour of the FIND_XXX() commands:
# search headers and libraries in the target environment, search 
# programs in the host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

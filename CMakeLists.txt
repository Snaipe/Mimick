# Copyright (C) 2016 Franklin "Snaipe" Mathieu.
# Redistribution and use of this file is allowed according to the terms of the MIT license.
# For details see the LICENSE file distributed with Criterion.

cmake_minimum_required (VERSION 2.8)

project (Mimick C)

include (CheckSymbolExists)

set (MODULE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/.cmake/Modules")
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${MODULE_DIR})

if (MSVC)
    enable_language (ASM_MASM)
    if (CMAKE_SYSTEM_PROCESSOR MATCHES "^i[3-7]86")
        set (MMK_ARCH "i386")
        set (MMK_BITS 32)
    else ()
        set (MMK_ARCH "x86_64")
        set (MMK_BITS 64)
    endif ()
else ()
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -Wall -Wextra")

    enable_language (ASM)
    if (CMAKE_SYSTEM_PROCESSOR MATCHES "^i[3-7]86$")
        set (MMK_ARCH "i386")
        set (MMK_BITS 32)
    elseif (CMAKE_SYSTEM_PROCESSOR MATCHES "^x86_64$")
        set (MMK_ARCH "x86_64")
        set (MMK_BITS 64)
    else ()
        message (FATAL_ERROR "Architecture '${CMAKE_SYSTEM_PROCESSOR}' not supported.")
    endif ()
endif ()

check_symbol_exists(_r_debug link.h HAVE__R_DEBUG)
check_symbol_exists(_DYNAMIC link.h HAVE__DYNAMIC)

include_directories(include src)
add_subdirectory (src)

add_library (mimick STATIC ${SOURCE_FILES})

add_subdirectory (test)

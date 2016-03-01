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
    if (CMAKE_GENERATOR MATCHES ".*Win64")
        set (MMK_ARCH "x86_64")
        set (MMK_BITS 64)
    else ()
        set (MMK_ARCH "i386")
        set (MMK_BITS 32)
        set (CMAKE_ASM_MASM_FLAGS "${CMAKE_ASM_MASM_FLAGS} /safeseh")
    endif ()
else ()
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -Wall -Wextra")

    enable_language (ASM)
    if (CMAKE_SYSTEM_PROCESSOR MATCHES "^i[3-7]86$")
        set (MMK_ARCH "i386")
        set (MMK_BITS 32)
    elseif (CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_|x86-|AMD|amd)64$")
        set (MMK_ARCH "x86_64")
        set (MMK_BITS 64)
    else ()
        message (FATAL_ERROR "Architecture '${CMAKE_SYSTEM_PROCESSOR}' is not supported.")
    endif ()
endif ()

option (USE_QEMU "Use QEMU to run the tests" OFF)

if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    set (MMK_EXE_FORMAT elf)
    set (MMK_EXE_FMT_ELF 1)
elseif (CMAKE_SYSTEM_NAME MATCHES "Darwin")
    set (MMK_EXE_FORMAT mach-o)
    set (MMK_EXE_FMT_MACH_O 1)
elseif (CMAKE_SYSTEM_NAME MATCHES "Windows")
    set (MMK_EXE_FORMAT pe)
    set (MMK_EXE_FMT_PE 1)
    add_definitions (-D_CRT_SECURE_NO_WARNINGS)
    add_definitions (-DWIN32_LEAN_AND_MEAN)
elseif (CMAKE_SYSTEM_NAME MATCHES "(Free|Net|Open)BSD")
    set (MMK_EXE_FORMAT elf)
    set (MMK_EXE_FMT_ELF 1)
elseif (CMAKE_SYSTEM_NAME MATCHES "Solaris|SunOS")
    set (MMK_EXE_FORMAT elf)
    set (MMK_EXE_FMT_ELF 1)
else ()
    message (FATAL_ERROR "Platform '${CMAKE_SYSTEM_NAME}' is not supported.")
endif ()

if (MMK_EXE_FMT_ELF)
  check_symbol_exists(_r_debug link.h HAVE__R_DEBUG)
  check_symbol_exists(_DYNAMIC link.h HAVE__DYNAMIC)
endif ()

include_directories(include src)
add_subdirectory (src)

add_library (mimick STATIC ${SOURCE_FILES})

enable_testing ()
add_subdirectory (test)

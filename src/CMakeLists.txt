# Copyright (C) 2016 Franklin "Snaipe" Mathieu.
# Redistribution and use of this file is allowed according to the terms of the MIT license.
# For details see the LICENSE file distributed with Criterion.

if (MSVC)
    set (ASM_FILES src/asm/trampoline-win-${MMK_ARCH}.asm)
else ()
    set (ASM_FILES src/asm/trampoline-${MMK_ARCH}.S)
endif ()

set (SOURCE_FILES ${ASM_FILES}
    src/config.h
    src/mimick.c
    src/plt.h
    src/plt-elf.c
    src/plt-elf.h
    src/trampoline.c
    src/trampoline.h
)

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/config.h.in"
  "${CMAKE_CURRENT_SOURCE_DIR}/config.h"
)

set (SOURCE_FILES ${SOURCE_FILES} PARENT_SCOPE)

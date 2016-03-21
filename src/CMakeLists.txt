# Copyright (C) 2016 Franklin "Snaipe" Mathieu.
# Redistribution and use of this file is allowed according to the terms of the MIT license.
# For details see the LICENSE file distributed with Criterion.

if (MSVC)
    set (ASM_FILES src/asm/trampoline-win-${MMK_ARCH}.asm)
else ()
    set (ASM_FILES src/asm/trampoline-${MMK_ARCH}.S)
endif ()

set (SOURCE_FILES ${ASM_FILES}
    src/assert.h
    src/config.h
    src/matcher.c
    src/mimick.c
    src/mock.h
    src/plt.h
    src/plt-${MMK_EXE_FORMAT}.c
    src/plt-${MMK_EXE_FORMAT}.h
    src/threadlocal.h
    src/trampoline.c
    src/trampoline.h
    src/verify.c
    src/vitals.c
    src/vitals.h
    src/when.c
)

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/config.h.in"
  "${CMAKE_CURRENT_SOURCE_DIR}/config.h"
)

set (SOURCE_FILES ${SOURCE_FILES} PARENT_SCOPE)

# Copyright (C) 2016 Franklin "Snaipe" Mathieu.
# Redistribution and use of this file is allowed according to the terms of the MIT license.
# For details see the LICENSE file distributed with Mimick.

if (MMK_ARCH STREQUAL MMK_ABI)
    set (ASM_ID ${MMK_ARCH})
else ()
    set (ASM_ID ${MMK_ARCH}-${MMK_ABI})
endif ()

set (ASM_FILES src/asm/trampoline-${ASM_ID}${ASM_EXTENSION})

set (SOURCE_FILES ${ASM_FILES}
    src/config.h
    src/core.c
    src/core.h
    src/matcher.c
    src/mock.c
    src/mock.h
    src/plt.h
    src/plt-${MMK_EXE_FORMAT}.c
    src/plt-${MMK_EXE_FORMAT}.h
    src/stub.c
    src/stub.h
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

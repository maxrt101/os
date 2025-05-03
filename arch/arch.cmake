include_guard(GLOBAL)

if (ARCH STREQUAL x86_64)
    include(${ARCH_DIR}/x86_64/x86_64.cmake)
else ()
    message(FATAL_ERROR "Unsupported ARCH '${ARCH}'")
endif ()

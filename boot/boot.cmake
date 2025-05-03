include_guard(GLOBAL)

if (BOOT_METHOD STREQUAL limine)
    include(${BOOT_DIR}/limine/limine.cmake)
else ()
    message(FATAL_ERROR "Unsupported BOOT_METHOD '${BOOT_METHOD}'")
endif ()



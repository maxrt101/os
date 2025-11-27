cmake_minimum_required(VERSION 3.30)

set(TOPDIR           "${CMAKE_CURRENT_LIST_DIR}")
set(ARCH_DIR         "${CMAKE_CURRENT_LIST_DIR}/arch")
set(BOOT_DIR         "${CMAKE_CURRENT_LIST_DIR}/boot")
set(KERNEL_DIR       "${CMAKE_CURRENT_LIST_DIR}/kernel")
set(TOOLCHAIN_DIR    "${CMAKE_CURRENT_LIST_DIR}/toolchain")

set(PROJECT_NAME     OS)
set(BOOT_METHOD      limine)
set(ARCH      	     x86_64)

include(${TOOLCHAIN_DIR}/project.cmake)
include(${TOOLCHAIN_DIR}/compiler.cmake)
include(${TOOLCHAIN_DIR}/util.cmake)

compiler_setup(GCC x86_64 elf)

project(${PROJECT_NAME} C ASM)
project_init()

include(${TOOLCHAIN_DIR}/qemu.cmake)
include(${BOOT_DIR}/boot.cmake)
include(${ARCH_DIR}/arch.cmake)

project_add_inc_dirs(${KERNEL_DIR}/include)
project_add_inc_recursive(${KERNEL_DIR}/include)
project_add_src_recursive(${KERNEL_DIR}/src)

# Generate assembly
#project_add_compile_options(ALL -S -fverbose-asm)

project_add_define(
	"ARCH=${ARCH}"
	"ARCH_INC=<${ARCH}.h>"

	# Features
	"USE_WAIT_BEFORE_TRIGGERING_IRQ=0"

	# Logs/Verbosity
	"SHOW_TIPS=1"
	"KPALLOC_VERBOSITY=0"
)

project_add_link_options(ALL
	-Wl,--cref,-Map=${CMAKE_BINARY_DIR}/${PROJECT_NAME}.map
	-Wl,--print-memory-usage
)

project_finish()

include_guard(GLOBAL)

project_add_inc_dirs(${ARCH_DIR}/x86_64)
project_add_src_recursive(${ARCH_DIR}/x86_64)
project_add_src_files(${ARCH_DIR}/x86_64/irq.S)

project_add_compile_options(ALL
    -ffreestanding
    -fno-stack-check
    -fno-stack-protector
    -fno-omit-frame-pointer
    # -fno-optimize-sibling-calls
    -fPIE
    -m64
    -march=x86-64
    -mno-80387
    -mno-mmx
    -mno-sse
    -mno-sse2
    -mno-red-zone
    -mgeneral-regs-only
  -g
  -O0
)

project_add_link_options(ALL 
    -Wl,-melf_x86_64
    -nostdlib
    -static
    -z max-page-size=0x1000
)

project_add_ld_scripts(${BOOT_DIR}/${BOOT_METHOD}/linker.ld)

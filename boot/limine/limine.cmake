include_guard(GLOBAL)

set(LIMINE_DIR        "${CMAKE_CURRENT_LIST_DIR}/limine")
set(BOOTLOADER_TARGET limine)

check_cmd_dep("xorriso --help" FAIL "try installing it with brew/apt")

project_add_inc_dirs(${LIMINE_DIR})
project_add_src_recursive(${CMAKE_CURRENT_LIST_DIR}/port)

function(__boot_setup)
    add_custom_target(${BOOTLOADER_TARGET}
        make -C ${LIMINE_DIR}
    )

    add_dependencies(${PROJECT_NAME} ${BOOTLOADER_TARGET})

    # FIXME: This looks like shit, but it's 3 AM and I don't want to deal with it right now
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND rm -rf ${CMAKE_BINARY_DIR}/iso_root &&
                mkdir -p ${CMAKE_BINARY_DIR}/iso_root/boot &&
                cp -v ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.elf ${CMAKE_BINARY_DIR}/iso_root/boot/kernel &&
                mkdir -p ${CMAKE_BINARY_DIR}/iso_root/boot/limine &&
                cp -v ${BOOT_DIR}/limine/limine.conf ${CMAKE_BINARY_DIR}/iso_root/boot/limine/ &&
                mkdir -p ${CMAKE_BINARY_DIR}/iso_root/EFI/BOOT &&
                cp -v ${LIMINE_DIR}/limine-bios.sys ${LIMINE_DIR}/limine-bios-cd.bin ${LIMINE_DIR}/limine-uefi-cd.bin ${CMAKE_BINARY_DIR}/iso_root/boot/limine/ &&
                cp -v ${LIMINE_DIR}/BOOTX64.EFI ${CMAKE_BINARY_DIR}/iso_root/EFI/BOOT/ &&
                cp -v ${LIMINE_DIR}/BOOTIA32.EFI ${CMAKE_BINARY_DIR}/iso_root/EFI/BOOT/ &&
                xorriso -as mkisofs -R -r -J -b boot/limine/limine-bios-cd.bin -no-emul-boot -boot-load-size 4 -boot-info-table -hfsplus -apm-block-size 2048 --efi-boot boot/limine/limine-uefi-cd.bin -efi-boot-part --efi-boot-image --protective-msdos-label ${CMAKE_BINARY_DIR}/iso_root -o ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.iso &&
                ${LIMINE_DIR}/limine bios-install ${PROJECT_NAME}.iso
    )
endfunction()

project_add_finish_callback(__boot_setup)
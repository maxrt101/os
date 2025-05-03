include_guard(GLOBAL)

check_cmd_dep("qemu-system-x86_64 --help" WARN "install it if you want qemu emulator targets to run")

function(__qemu_setup)
	add_custom_target(qemu-run-${PROJECT_NAME}
		qemu-system-x86_64 -M q35 -cdrom ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.iso -d int,cpu_reset,guest_errors -monitor stdio -D ${CMAKE_BINARY_DIR}/qemu.log
	)

	add_custom_target(qemu-run-gdb-${PROJECT_NAME}
		qemu-system-x86_64 -M q35 -cdrom ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.iso -d int,cpu_reset,guest_errors -monitor stdio -D ${CMAKE_BINARY_DIR}/qemu.log -s -S
	)
endfunction()

project_add_finish_callback(__qemu_setup)

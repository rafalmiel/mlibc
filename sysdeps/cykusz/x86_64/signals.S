.section .text
.global __mlibc_signal_restore
__mlibc_signal_restore:
	mov $36, %rax
	syscall
	ud2


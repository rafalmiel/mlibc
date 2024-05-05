.section .text
.global _start
_start:
    mov $main, %rsi
    mov %rsp, %rdi
    call __mlibc_entry
.section .note.GNU-stack,"",%progbits


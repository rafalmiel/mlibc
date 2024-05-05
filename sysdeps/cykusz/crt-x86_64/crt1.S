.section .text
.global _start
_start:
    mov $main, %rdi
    mov %rsp, %rsi
    call __mlibc_entry
.section .note.GNU-stack,"",%progbits


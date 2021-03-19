#ifndef SYSCALL_H
#define SYSCALL_H

#include <stdint.h>

#define SYS_READ 0
#define SYS_WRITE 1
#define SYS_OPEN 2
#define SYS_CLOSE 3
#define SYS_CHDIR 4
#define SYS_GET_CWD 5
#define SYS_MKDIR 6
#define SYS_READDIR 7
#define SYS_EXIT 8
#define SYS_SLEEP 9
#define SYS_BIND 13
#define SYS_CONNECT 14
#define SYS_SELECT 15
#define SYS_MOUNT 16
#define SYS_TIME 18
#define SYS_LINK 19
#define SYS_RMDIR 20
#define SYS_UNLINK 21
#define SYS_LINK 22
#define SYS_RENAME 23
#define SYS_FORK 24
#define SYS_EXEC 25
#define SYS_MMAP 27
#define SYS_MUNMAP 28

#define SYS_WAITPID 33
#define SYS_IOCTL 34
#define SYS_SIGACTION 35
#define SYS_SIGRETURN 36


#ifdef __cplusplus
extern "C"{
#endif

__attribute__((always_inline))
static inline long syscalln0(uint64_t call) {
    volatile long ret;
    asm volatile ("syscall" : "=a"(ret)
            : "a"(call)
            : "rcx", "r11", "memory");
    return ret;
}

__attribute__((always_inline))
static long syscalln1(uint64_t call, uint64_t arg0) {
    volatile long ret;
    asm volatile ("syscall" : "=a"(ret)
	    : "a"(call), "D"(arg0)
	    : "rcx", "r11", "memory");
    return ret;
}

__attribute__((always_inline))
static long syscalln2(uint64_t call, uint64_t arg0, uint64_t arg1) {
    volatile long ret;
    asm volatile ("syscall" : "=a"(ret)
	    : "a"(call), "D"(arg0), "S"(arg1)
	    : "rcx", "r11", "memory");
    return ret;
}

__attribute__((always_inline))
static long syscalln3(uint64_t call, uint64_t arg0, uint64_t arg1, uint64_t arg2) {
    volatile long ret;
    asm volatile ("syscall" : "=a"(ret)
            : "a"(call), "D"(arg0), "S"(arg1), "d"(arg2)
            : "rcx", "r11", "memory");
    return ret;
}

__attribute__((always_inline))
static long syscalln4(uint64_t call, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3) {
    volatile long ret;
    register uint64_t arg3r asm("r10") = arg3; // put arg3 in r10
    asm volatile ("syscall" : "=a"(ret)
	    : "a"(call), "D"(arg0), "S"(arg1), "d"(arg2), "r"(arg3r)
	    : "rcx", "r11", "memory");
    return ret;
}

__attribute__((always_inline))
static long syscalln5(uint64_t call, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4) {
    volatile long ret;
    register uint64_t arg3r asm("r10") = arg3; // put arg3 in r10
    register uint64_t arg4r asm("r8") = arg4; // put arg4 in r9
    asm volatile ("syscall" : "=a"(ret)
	    : "a"(call), "D"(arg0), "S"(arg1), "d"(arg2), "r"(arg3r), "r"(arg4r)
	    : "rcx", "r11", "memory");
    return ret;
}

__attribute__((always_inline))
static long syscalln6(uint64_t call, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5) {
    volatile long ret;
    register uint64_t arg3r asm("r10") = arg3; // put arg3 in r10
    register uint64_t arg4r asm("r8") = arg4; // put arg4 in r9
    register uint64_t arg5r asm("r9") = arg5; // put arg5 in r8
    asm volatile ("syscall" : "=a"(ret)
	    : "a"(call), "D"(arg0), "S"(arg1), "d"(arg2), "r"(arg3r), "r"(arg4r), "r"(arg5r)
	    : "rcx", "r11", "memory");
    return ret;
}

#ifdef __cplusplus
}
    __attribute__((always_inline)) static inline long _syscall(uint64_t call) { return syscalln0(call); }
    __attribute__((always_inline)) static inline long _syscall(uint64_t call, uint64_t arg0) { return syscalln1(call, arg0); }
    __attribute__((always_inline)) static inline long _syscall(uint64_t call, uint64_t arg0, uint64_t arg1) { return syscalln2(call, arg0, arg1); }
    __attribute__((always_inline)) static inline long _syscall(uint64_t call, uint64_t arg0, uint64_t arg1, uint64_t arg2) { return syscalln3(call, arg0, arg1, arg2); }
    __attribute__((always_inline)) static inline long _syscall(uint64_t call, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3) { return syscalln4(call, arg0, arg1, arg2, arg3); }
    __attribute__((always_inline)) static inline long _syscall(uint64_t call, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4)  { return syscalln5(call, arg0, arg1, arg2, arg3, arg4); }
    __attribute__((always_inline)) static inline long _syscall(uint64_t call, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5)  { return syscalln6(call, arg0, arg1, arg2, arg3, arg4, arg5); }

    template<typename... T>
    __attribute__((always_inline)) static inline long syscall(uint64_t call, T... args){
        return _syscall(call, (uint64_t)(args)...);
    }
#else
    #define GET_SYSCALL(a0, a1, a2, a3, a4, a5, a6, name, ...) name
    #define syscall(...) GET_SYSCALL(__VA_ARGS__, syscalln6, syscalln5, syscalln4, syscalln3, syscalln2, syscalln1, syscalln0)(__VA_ARGS__) 
#endif

#endif

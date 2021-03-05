#include <cykusz/syscall.h>
#include <stddef.h>
#include <bits/ensure.h>
#include <abi-bits/pid_t.h>
#include <mlibc/debug.hpp>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/thread-entry.hpp>
#include <errno.h>

namespace mlibc{
	int sys_futex_wait(int *pointer, int expected){

		return 0;
	}

	int sys_futex_wake(int *pointer) {

		return 0;
	}

	int sys_tcb_set(void* pointer){

		return 0;
	}

	int sys_vm_map(void *hint, size_t size, int prot, int flags, int fd, off_t offset, void **window) {
		ssize_t res = syscalln6(SYS_MMAP, (uint64_t)hint, size, prot, flags, fd, offset);

		if (res < 0)
			return -res;

		*window = (void*)res;

		return 0;
	}

	int sys_vm_unmap(void* address, size_t size) {
		return syscalln2(SYS_MUNMAP, (uint64_t)address, size);
	}

	int sys_anon_allocate(size_t size, void **pointer) {
		return sys_vm_map(nullptr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0, pointer);
	}

	int sys_anon_free(void *pointer, size_t size) {
		return sys_vm_unmap(pointer, size);
	}

	void sys_libc_panic(){
		syscalln3(SYS_WRITE, 1, (unsigned long long)"PANIC\n", 6);
		for(;;);
	}

	void sys_libc_log(const char* msg){
		syscalln3(SYS_WRITE, 1, (unsigned long long)msg, strlen(msg));
	}

	#ifndef MLIBC_BUILDING_RTDL

	void sys_exit(int status){
		syscalln0(SYS_EXIT);
	}

	pid_t sys_getpid(){
		return 0;
	}
	
	int sys_clock_get(int clock, time_t *secs, long *nanos) {
		return 0;
	}

	int sys_getcwd(char *buffer, size_t size){
		return -1;
	}

	int sys_chdir(const char *path){
		return -1;
	}

	int sys_sleep(time_t* sec, long* nanosec){
		return -1;
	}
	
	uid_t sys_getuid(){
		return -1;
	}

	uid_t sys_geteuid(){
		return -1;
	}

	int sys_setuid(uid_t uid){
		return -1;
	}

	int sys_seteuid(uid_t euid){
		return -1;
	}

	int sys_getgid(){
		return -1;
	}

	int sys_getegid(){
		return -1;
	}

	int sys_setgid(gid_t gid){
		return -1;
	}

	int sys_setegid(gid_t egid){
		return -1;
	}

	void sys_yield(){
		
	}

	int sys_clone(void *entry, void *user_arg, void *tcb, pid_t *tid_out){
		return -1;
	}

	void sys_thread_exit(){
	}

	int sys_waitpid(pid_t pid, int *status, int flags, pid_t *ret_pid){
		volatile int* a = (volatile int*)42;
		*a = 3;
		return 0;
	}
	#endif
} 

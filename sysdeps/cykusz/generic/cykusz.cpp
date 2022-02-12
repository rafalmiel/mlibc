#include <cykusz/syscall.h>
#include <stddef.h>
#include <bits/ensure.h>
#include <abi-bits/pid_t.h>
#include <mlibc/debug.hpp>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/thread-entry.hpp>
#include <errno.h>

#include <frg/vector.hpp>
#include <mlibc/allocator.hpp>

#define ARCH_SET_FS 0x1002

namespace mlibc{

	int sys_futex_tid() {
		return syscalln0(SYS_GETTID);
	}

	int sys_futex_wait(int *pointer, int expected){
		ssize_t res = syscalln2(SYS_FUTEX_WAIT, (uint64_t)pointer, (uint64_t)expected);

		if (res < 0)
			return -res;

		return 0;
	}

	int sys_futex_wake(int *pointer) {
		ssize_t res = syscalln1(SYS_FUTEX_WAKE, (uint64_t)pointer);

		if (res < 0)
			return -res;

		return 0;
	}

	struct SliceParam {
		void* ptr;
		uint64_t len;
	};

	static frg::vector<SliceParam, MemoryAllocator> prepareSlice(char *const arg[]) {
		if (arg == nullptr) {
			return frg::vector<SliceParam, MemoryAllocator>{getAllocator()};
		}

		size_t len = 0;

		while (arg[len] != nullptr) {
			len += 1;
		}

		frg::vector<SliceParam, MemoryAllocator> params{getAllocator()};
		params.resize(len);

		for (size_t i = 0; i < len; ++i) {
			params[i].ptr = (void*)arg[i];
			params[i].len = strlen(arg[i]);
		}

		return params;

	}

	int sys_fork(pid_t *child) {
		ssize_t ret = syscalln0(SYS_FORK);

		if (ret < 0) {
			return -ret;
		}

		*child = ret;

		return 0;
	}

	int sys_execve(const char *path, char *const argv[], char *const envp[]) {
		SliceParam ppath = SliceParam { ptr: (void*)path, len: strlen(path) };
		SliceParam args = SliceParam { ptr: nullptr, len: 0 };
		SliceParam envs = SliceParam { ptr: nullptr, len: 0 };

		uint64_t ap = 0;
		uint64_t ep = 0;

		auto arg_slice = prepareSlice(argv);

		//if (argv != nullptr) {
		//	args.ptr = (void*)arg_slice.data();
		//	args.len = arg_slice.size();
		//	ap = (uint64_t)&args;
		//}

		auto env_slice = prepareSlice(envp);

		//if (envp != nullptr) {
		//	envs.ptr = (void*)env_slice.data();
		//	envs.len = env_slice.size();
		//	ep = (uint64_t)&envs;
		//}

		ssize_t res = syscalln6(SYS_EXEC, (uint64_t)path, strlen(path), (uint64_t)arg_slice.data(), arg_slice.size(), (uint64_t)env_slice.data(), env_slice.size());

		if (res < 0) {
			return -res;
		}

		return 0;
	}

	int sys_tcb_set(void* pointer){
		ssize_t res = syscalln2(SYS_ARCH_PRCTL, ARCH_SET_FS, (uint64_t)pointer);

		if (res < 0) {
			return -res;
		}

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

	int sys_getrlimit(int resource, struct rlimit *limit) {
		ssize_t res = syscalln2(SYS_GETRLIMIT, (uint64_t)resource, (uint64_t)limit);

		if (res < 0)
			return -res;

		return 0;
	}

	void sys_libc_panic(){
		sys_exit(1);
	}

	void sys_libc_log(const char* msg){
		syscalln2(SYS_DEBUG, (unsigned long long)msg, strlen(msg));
	}

	int sys_getrusage(int scope, struct rusage *usage) {
		return 0;
	}

	#ifndef MLIBC_BUILDING_RTDL

	void sys_exit(int status){
		syscalln0(SYS_EXIT);
	}

	pid_t sys_getpid(){
		return syscalln0(SYS_GETPID);
	}
	
	int sys_clock_get(int clock, time_t *secs, long *nanos) {
		*nanos = 0;
		*secs = syscalln0(SYS_TIME);

		return 0;
	}

	int sys_getcwd(char *buffer, size_t size){
		return -1;
	}

	int sys_chdir(const char *path){
		return -1;
	}

	int sys_sleep(time_t* sec, long* nanosec){
		uint64_t to_sleep = *sec + *nanosec;

		syscalln1(SYS_SLEEP, to_sleep);

		return 0;
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

	gid_t sys_getgid(){
		return -1;
	}

	gid_t sys_getegid(){
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
		auto stack = prepare_stack(entry, user_arg, tcb);
                pid_t tid = syscall(SYS_SPAWN_THREAD, __mlibc_start_thread, stack);

                if(tid_out){
                        *tid_out = tid;
                }

                return 0;
	}

	void sys_thread_exit(){
		syscalln0(SYS_EXIT_THREAD);
	}

	int sys_waitpid(pid_t pid, int *status, int flags, pid_t *ret_pid){
		ssize_t ret = syscalln3(SYS_WAITPID, (uint64_t)pid, (uint64_t)status, (uint64_t)flags);

		if (ret < 0) {
			return -ret;
		}

		*ret_pid = ret;

		return 0;
	}

	int sys_kill(int pid, int sig) {
		ssize_t ret = syscalln2(SYS_KILL, (uint64_t)pid, (uint64_t)sig);

		if (ret < 0) {
			return -ret;
		}

		return 0;
	}
	#endif
} 

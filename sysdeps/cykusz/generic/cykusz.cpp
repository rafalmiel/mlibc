#include <cykusz/syscall_internal.h>
#include <stddef.h>
#include <bits/ensure.h>
#include <abi-bits/pid_t.h>
#include <mlibc/debug.hpp>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/thread-entry.hpp>
#include <errno.h>
#include <unistd.h>

#include <frg/vector.hpp>
#include <mlibc/allocator.hpp>

#define ARCH_SET_FS 0x1002

namespace mlibc {

    int sys_futex_tid() {
        return syscall(SYS_GETTID);
    }

    int sys_gettid() {
        return syscall(SYS_GETTID);
    }

    int sys_futex_wait(int *pointer, int expected, const struct timespec *time) {
        (void) time;
        ssize_t res = syscall(SYS_FUTEX_WAIT, pointer, expected);

        if (res < 0)
            return -res;

        return 0;
    }

    int sys_futex_wake(int *pointer) {
        ssize_t res = syscall(SYS_FUTEX_WAKE, pointer);

        SYS_RETURN_ERR(res);

        return 0;
    }

    struct SliceParam {
        void *ptr;
        uint64_t len;
    };

    static frg::vector <SliceParam, MemoryAllocator> prepareSlice(char *const arg[]) {
        if (arg == nullptr) {
            return frg::vector < SliceParam, MemoryAllocator > {getAllocator()};
        }

        size_t len = 0;

        while (arg[len] != nullptr) {
            len += 1;
        }

        frg::vector <SliceParam, MemoryAllocator> params{getAllocator()};
        params.resize(len);

        for (size_t i = 0; i < len; ++i) {
            params[i].ptr = (void *) arg[i];
            params[i].len = strlen(arg[i]);
        }

        return params;

    }

    int sys_fork(pid_t *child) {
        ssize_t ret = syscall(SYS_FORK);

        SYS_RETURN_ERR(ret);

        *child = ret;

        return 0;
    }

    int sys_execve(const char *path, char *const argv[], char *const envp[]) {
        auto arg_slice = prepareSlice(argv);
        auto env_slice = prepareSlice(envp);

        ssize_t res = syscall(SYS_EXEC, path, strlen(path), arg_slice.data(), arg_slice.size(),
                              env_slice.data(), env_slice.size());

        SYS_RETURN_ERR(res);

        return 0;
    }

    int sys_tcb_set(void *pointer) {
        ssize_t res = syscall(SYS_ARCH_PRCTL, ARCH_SET_FS, pointer);

        SYS_RETURN_ERR(res);

        return 0;
    }

    int sys_vm_map(void *hint, size_t size, int prot, int flags, int fd, off_t offset, void **window) {
        ssize_t res = syscall(SYS_MMAP, hint, size, prot, flags, fd, offset);

        SYS_RETURN_ERR(res);

        *window = (void *) res;

        return 0;
    }

    int sys_vm_protect(void *pointer, size_t size, int prot) {
        ssize_t res = syscall(SYS_MPROTECT, pointer, size, prot);

        SYS_RETURN_ERR(res);

        return 0;
    }

    int sys_vm_unmap(void *address, size_t size) {
        return syscall(SYS_MUNMAP, address, size);
    }

    int sys_anon_allocate(size_t size, void **pointer) {
        return sys_vm_map(nullptr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0, pointer);
    }

    int sys_anon_free(void *pointer, size_t size) {
        return sys_vm_unmap(pointer, size);
    }

    int sys_getrlimit(int resource, struct rlimit *limit) {
        ssize_t res = syscall(SYS_GETRLIMIT, resource, limit);

        SYS_RETURN_ERR(res);

        return 0;
    }

    void sys_libc_panic() {
        sys_exit(1);
    }

    void sys_libc_log(const char *msg) {
        syscall(SYS_DEBUG, (unsigned long long) msg, strlen(msg));
    }

    int sys_getrusage(int scope, struct rusage *usage) {
        (void) scope;
        (void) usage;
        return 0;
    }

    void sys_exit(int status) {
        syscall(SYS_EXIT, status);

        __builtin_unreachable();
    }

#ifndef MLIBC_BUILDING_RTLD
    int sys_uname(struct utsname *buf) {
        __ensure(buf);
        mlibc::infoLogger() << "\e[31mmlibc: uname() returns static information\e[39m" << frg::endlog;
        strcpy(buf->sysname, "cykusz");
        strcpy(buf->nodename, "cykusz");
        strcpy(buf->release, "0.1-master");
        strcpy(buf->version, "Cykusz rust os");
        strcpy(buf->machine, "x86_64");
        return 0;
    }

    int sys_getentropy(void *buffer, size_t length) {
        int fd = open("/dev/urandom", O_RDONLY);
        size_t s = read(fd, buffer, length);
        if (s != length) {
            mlibc::infoLogger() << "mlibc: getentropy failed to read full " << length << "bytes. got: " << s << frg::endlog;
        }
        close(fd);
        return 0;
    }

    pid_t sys_getpid() {
        return syscall(SYS_GETPID);
    }

    pid_t sys_getppid() {
        return syscall(SYS_GETPPID);
    }

    pid_t sys_getpgid(pid_t pid, pid_t *pgid) {
        ssize_t res = syscall(SYS_GETPGID, pid);

        SYS_RETURN_ERR(res);

        *pgid = res;

        return 0;
    }

    int sys_clock_get(int clock, time_t *secs, long *nanos) {
        (void) clock;
        *nanos = 0;
        *secs = syscall(SYS_TIME);

        return 0;
    }

    int sys_sleep(time_t *sec, long *nanosec) {
        uint64_t to_sleep = ((uint64_t) * sec) * 1000000000 + *nanosec;

        ssize_t res = syscall(SYS_SLEEP, to_sleep);

        SYS_RETURN_ERR(res);

        return 0;
    }

    uid_t sys_getuid() {
        return -1;
    }

    uid_t sys_geteuid() {
        return -1;
    }

    int sys_setuid(uid_t uid) {
        (void) uid;
        return -1;
    }

    int sys_seteuid(uid_t euid) {
        (void) euid;
        return -1;
    }

    gid_t sys_getgid() {
        return -1;
    }

    gid_t sys_getegid() {
        return -1;
    }

    int sys_setgid(gid_t gid) {
        (void) gid;
        return -1;
    }

    int sys_setegid(gid_t egid) {
        (void) egid;
        return -1;
    }

    int sys_setpgid(pid_t pid, pid_t pgid) {
        ssize_t ret = syscall(SYS_SETPGID, pid, pgid);

        SYS_RETURN_ERR(ret);

        return 0;
    }

    void sys_yield() {
        syscall(SYS_YIELD);
    }

    int sys_clone(void *tcb, pid_t *tid_out, void *stack) {
        (void) tcb;
        ssize_t tid = syscall(SYS_SPAWN_THREAD, __mlibc_start_thread, stack);

        SYS_RETURN_ERR(tid);

        *tid_out = (pid_t) tid;
        return 0;
    }

    void sys_thread_exit() {
        syscalln0(SYS_EXIT_THREAD);

        __builtin_unreachable();
    }

    int sys_waitpid(pid_t pid, int *status, int flags, struct rusage *ru, pid_t *ret_pid) {
        (void) ru;
        ssize_t ret = syscall(SYS_WAITPID, pid, status, flags);

        SYS_RETURN_ERR(ret);

        *ret_pid = ret;

        return 0;
    }

    int sys_kill(int pid, int sig) {
        ssize_t ret = syscall(SYS_KILL, pid, sig);

        SYS_RETURN_ERR(ret);

        return 0;
    }

#endif
} 

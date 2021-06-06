#ifndef MLIBC_ANSI_SYSDEPS
#define MLIBC_ANSI_SYSDEPS

#include <stddef.h>

#include <abi-bits/seek-whence.h>
#include <abi-bits/vm-flags.h>
#include <abi-bits/pid_t.h>
#include <bits/off_t.h>
#include <bits/ssize_t.h>
#include <bits/ansi/time_t.h>
#include <signal.h>
#include <stdarg.h>

namespace [[gnu::visibility("hidden")]] mlibc {

[[noreturn]] void sys_exit(int status);
[[noreturn, gnu::weak]] void sys_thread_exit();

int sys_open(const char *pathname, int flags, int *fd);
[[gnu::weak]] int sys_flock(int fd, int options);

[[gnu::weak]] int sys_open_dir(const char *path, int *handle);
[[gnu::weak]] int sys_read_entries(int handle, void *buffer, size_t max_size,
		size_t *bytes_read);

int sys_read(int fd, void *buf, size_t count, ssize_t *bytes_read);

int sys_write(int fd, const void *buf, size_t count, ssize_t *bytes_written);
[[gnu::weak]] int sys_pread(int fd, void *buf, size_t n, off_t off, ssize_t *bytes_read);

int sys_seek(int fd, off_t offset, int whence, off_t *new_offset);
int sys_close(int fd);

int sys_clock_get(int clock, time_t *secs, long *nanos);
[[gnu::weak]] int sys_clock_getres(int clock, time_t *secs, long *nanos);
[[gnu::weak]] int sys_sleep(time_t *secs, long *nanos);
// In contrast to the isatty() library function, the sysdep function uses return value
// zero (and not one) to indicate that the file is a terminal.
[[gnu::weak]] int sys_isatty(int fd);
[[gnu::weak]] int sys_rmdir(const char *path);
              int sys_unlink(const char *path);
[[gnu::weak]] int sys_rename(const char *path, const char *new_path);
[[gnu::weak]] int sys_renameat(int olddirfd, const char *old_path, int newdirfd, const char *new_path);

              int sys_sigprocmask(int how, const sigset_t *__restrict set,
		sigset_t *__restrict retrieve);
              int sys_sigaction(int, const struct sigaction *__restrict,
		struct sigaction *__restrict);

[[gnu::weak]] int sys_fork(pid_t *child);
[[gnu::weak]] int sys_waitpid(pid_t pid, int *status, int flags, pid_t *ret_pid);
[[gnu::weak]] int sys_execve(const char *path, char *const argv[], char *const envp[]);

[[gnu::weak]] pid_t sys_getpid();
[[gnu::weak]] int sys_kill(int, int);

} //namespace mlibc

#endif // MLIBC_ANSI_SYSDEPS

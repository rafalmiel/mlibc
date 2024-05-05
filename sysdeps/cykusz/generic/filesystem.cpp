#include <cykusz/syscall_internal.h>

#include <sys/ioctl.h>
#include <bits/ensure.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <dirent.h>
#include <limits.h>
#include <fcntl.h>
#include <string.h>

#include <mlibc/all-sysdeps.hpp>
#include <mlibc/debug.hpp>

namespace mlibc {

    int sys_write(int fd, const void *buffer, size_t count, ssize_t *written) {
        long ret = syscall(SYS_WRITE, fd, buffer, count);

        SYS_RETURN_ERR(ret);

        *written = ret;
        return 0;
    }

    int sys_read(int fd, void *buf, size_t count, ssize_t *bytes_read) {
        long ret = syscall(SYS_READ, fd, (uintptr_t) buf, count);

        SYS_RETURN_ERR(ret);

        *bytes_read = ret;
        return 0;
    }

    int sys_readv(int fd, const struct iovec *iovs, int iovc, ssize_t *bytes_read) {
        for(int i = 0; i < iovc; i++) {
            ssize_t intermed = 0;

            if(int e = sys_read(fd, iovs[i].iov_base, iovs[i].iov_len, &intermed); e)
                return e;
            else if(intermed == 0)
                break;

            *bytes_read += intermed;
        }

        return 0;
    }

    int sys_pwrite(int fd, const void *buffer, size_t count, off_t off, ssize_t *written) {
        long ret = syscall(SYS_PWRITE, fd, (uintptr_t) buffer, count, off);

        SYS_RETURN_ERR(ret);

        *written = ret;

        return 0;
    }

    int sys_pread(int fd, void *buf, size_t count, off_t off, ssize_t *bytes_read) {
        long ret = syscall(SYS_PREAD, fd, (uintptr_t) buf, count, off);

        SYS_RETURN_ERR(ret);

        *bytes_read = ret;

        return 0;
    }

    int sys_seek(int fd, off_t offset, int whence, off_t *new_offset) {
        ssize_t res = syscall(SYS_SEEK, fd, offset, whence);

        SYS_RETURN_ERR(res);

        *new_offset = res;

        return 0;
    }


    int sys_open(const char *filename, int flags, mode_t mode, int *fd) {
        (void) mode;
        ssize_t res = syscall(SYS_OPEN, AT_FDCWD, filename, strlen(filename),
                              flags);

        SYS_RETURN_ERR(res);

        *fd = res;

        return 0;
    }

    int sys_openat(int dirfd, const char *filename, int flags, mode_t mode, int *fd) {
        (void) mode;
        ssize_t res = syscall(SYS_OPEN, dirfd, filename, strlen(filename),
                              flags);

        SYS_RETURN_ERR(res);

        *fd = res;

        return 0;
    }

    int sys_close(int fd) {
        ssize_t res = syscall(SYS_CLOSE, fd);

        SYS_RETURN_ERR(res);

        return 0;
    }

    int sys_access(const char *path, int mode) {
        ssize_t ret = syscall(SYS_ACCESS, AT_FDCWD, path, strlen(path),
                              mode, 0);

        SYS_RETURN_ERR(ret);

        return 0;
    }

    int sys_utimensat(int dirfd, const char *pathname, const struct timespec times[2], int flags) {
        ssize_t ret = syscall(SYS_UTIME, dirfd, pathname, pathname ? strlen(pathname) : 0, times, flags);

        SYS_RETURN_ERR(ret);

        return 0;
    }

    int sys_getcwd(char *buffer, size_t size) {
        ssize_t res = syscall(SYS_GETCWD, buffer, size);

        SYS_RETURN_ERR(res);

        return 0;
    }

    int sys_chdir(const char *path) {
        ssize_t res = syscall(SYS_CHDIR, AT_FDCWD, path, strlen(path));

        SYS_RETURN_ERR(res);

        return 0;
    }

    int sys_fchdir(int fd) {
        ssize_t res = syscall(SYS_CHDIR, fd, 0, 0);

        SYS_RETURN_ERR(res);

        return 0;
    }

    int sys_fsync(int fd) {
        ssize_t ret = syscall(SYS_FSYNC, fd);

        SYS_RETURN_ERR(ret);

        return 0;
    }

    int sys_ftruncate(int fd, size_t size) {
        ssize_t ret = syscall(SYS_TRUNCATE, fd, size);

        SYS_RETURN_ERR(ret);

        return 0;
    }

    int sys_faccessat(int dirfd, const char *pathname, int mode, int flags) {
        ssize_t ret = syscall(SYS_ACCESS, dirfd, pathname, strlen(pathname),
                              mode, flags);

        SYS_RETURN_ERR(ret);

        return 0;
    }

    int sys_stat(fsfd_target fsfdt, int fd, const char *path, int flags, struct stat *statbuf) {
        (void) flags;
        ssize_t res;

        switch (fsfdt) {
            case fsfd_target::fd:
                res = syscall(SYS_STAT, fd, 0, 0, statbuf, flags);
                break;
            case fsfd_target::path:
                res = syscall(SYS_STAT, AT_FDCWD, path, strlen(path), statbuf, flags);
                break;
            case fsfd_target::fd_path:
                res = syscall(SYS_STAT, fd, path, strlen(path), statbuf, flags);
                break;
            case fsfd_target::none:
                return EINVAL;
        }

        SYS_RETURN_ERR(res);

        return 0;
    }

    int sys_ioctl(int fd, unsigned long request, void *arg, int *result) {
        ssize_t res = syscall(SYS_IOCTL, fd, request, arg);

        SYS_RETURN_ERR(res);

        *result = res;

        return 0;
    }

    int sys_isatty(int fd) {
        uint64_t grp;
        int res;
        if (sys_ioctl(fd, TIOCGPGRP, &grp, &res) != ENOTTY) {
            return 0;
        } else {
            return ENOTTY;
        }
    }


#ifndef MLIBC_BUILDING_RTLD

    int sys_ttyname(int fd, char *buf, size_t size) {
        (void) fd;
        strncpy(buf, "/dev/tty", size);

        return 0;
    }

#endif

    int sys_tcgetattr(int fd, struct termios *attr) {
        int ret;

        if (int r = sys_ioctl(fd, TCGETS, attr, &ret) != 0) {
            return r;
        }

        return 0;
    }

    int sys_tcsetattr(int fd, int optional_action, const struct termios *attr) {
        int ret;
        int action;

        switch (optional_action) {
            case TCSANOW:
                action = TCSETS;
                break;
            case TCSADRAIN:
                action = TCSETSW;
                break;
            case TCSAFLUSH:
                action = TCSETSF;
                break;
            default:
                action = TCSETS;
                break;
        }

        if (int r = sys_ioctl(fd, action, (void *) attr, &ret) != 0) {
            return r;
        }

        return ret;
    }

    int sys_poll(struct pollfd *fds, nfds_t count, int timeout, int *num_events) {
        ssize_t ret = syscall(SYS_POLL, fds, count, timeout);

        SYS_RETURN_ERR(ret);

        *num_events = ret;

        return 0;
    }

    int sys_mkdir(const char *path, mode_t mode) {
        (void) mode;
        ssize_t ret = syscall(SYS_MKDIR, AT_FDCWD, path, strlen(path));

        SYS_RETURN_ERR(ret);

        return 0;
    }

    int sys_mkdirat(int dirfd, const char *path, mode_t mode) {
        (void) mode;
        ssize_t ret = syscall(SYS_MKDIR, dirfd, path, strlen(path));

        SYS_RETURN_ERR(ret);

        return 0;
    }

    int sys_rmdir(const char *path) {
        ssize_t ret = syscall(SYS_RMDIR, path, strlen(path));

        SYS_RETURN_ERR(ret);

        return 0;
    }

    int sys_link(const char *srcpath, const char *destpath) {
        return sys_linkat(AT_FDCWD, srcpath, AT_FDCWD, destpath, 0);
    }

    int sys_linkat(int srcfd, const char *srcpath, int destfd, const char *destpath, int flags) {
        (void) flags;
        ssize_t res = syscall(SYS_LINK, srcfd, srcpath, strlen(srcpath), destfd, destpath, strlen(destpath));

        SYS_RETURN_ERR(res);

        return 0;
    }

    int sys_unlink(const char *path) {
        ssize_t ret = syscall(SYS_UNLINK, AT_FDCWD, path, strlen(path), 0);

        SYS_RETURN_ERR(ret);

        return 0;
    }

    int sys_unlinkat(int fd, const char *path, int flags) {
        ssize_t ret = syscall(SYS_UNLINK, fd, path, strlen(path), flags);

        SYS_RETURN_ERR(ret);

        return 0;
    }

    int sys_mknodat(int dirfd, const char *path, int mode, int dev) {
        ssize_t ret = syscall(SYS_MKNODE, dirfd, path, strlen(path), mode, dev);

        SYS_RETURN_ERR(ret);

        return 0;
    }

    int sys_mkfifoat(int dirfd, const char *path, mode_t mode) {
        return sys_mknodat(dirfd, path, mode | S_IFIFO, 0);
    }

    int sys_read_entries(int handle, void *buffer, size_t max_size, size_t *bytes_read) {
        ssize_t res = syscall(SYS_READDIR, handle, buffer, max_size);

        SYS_RETURN_ERR(res);

        *bytes_read = res;

        return 0;
    }

    int sys_open_dir(const char *path, int *handle) {
        ssize_t res = syscall(SYS_OPEN, AT_FDCWD, path, strlen(path), O_DIRECTORY);

        SYS_RETURN_ERR(res);

        *handle = res;

        return 0;
    }

    int sys_rename(const char *path, const char *new_path) {
        ssize_t res = syscall(SYS_RENAME, AT_FDCWD, path, strlen(path), AT_FDCWD, new_path, strlen(new_path));

        SYS_RETURN_ERR(res);

        return 0;
    }

    int sys_renameat(int oldfd, const char *path, int newfd, const char *new_path) {
        ssize_t res = syscall(SYS_RENAME, oldfd, path, strlen(path), newfd, new_path, strlen(new_path));

        SYS_RETURN_ERR(res);

        return 0;
    }

    int sys_symlink(const char *target_path, const char *link_path) {
        ssize_t res = syscall(SYS_SYMLINK, target_path, strlen(target_path), AT_FDCWD, link_path, strlen(link_path));

        SYS_RETURN_ERR(res);

        return 0;
    }

    int sys_symlinkat(const char *target_path, int dirfd, const char *link_path) {
        ssize_t res = syscall(SYS_SYMLINK, target_path, strlen(target_path), dirfd, link_path, strlen(link_path));

        SYS_RETURN_ERR(res);

        return 0;
    }

    int sys_readlink(const char *path, void *buffer, size_t max_size, ssize_t *length) {
        ssize_t res = syscall(SYS_READLINK, AT_FDCWD, path, strlen(path), buffer, max_size, length);

        SYS_RETURN_ERR(res);

        return 0;
    }

    int sys_readlinkat(int dirfd, const char *path, void *buffer, size_t max_size, ssize_t *length) {
        ssize_t res = syscall(SYS_READLINK, dirfd, path, strlen(path), buffer, max_size, length);

        SYS_RETURN_ERR(res);

        return 0;
    }

    int sys_dup(int fd, int flags, int *newfd) {
        ssize_t res = syscall(SYS_DUP, fd, flags);

        SYS_RETURN_ERR(res);

        *newfd = res;

        return 0;
    }

    int sys_dup2(int fd, int flags, int newfd) {
        ssize_t res = syscall(SYS_DUP2, fd, newfd, flags);

        SYS_RETURN_ERR(res);

        return 0;
    }

    int sys_fcntl(int fd, int request, va_list args, int *result) {
        ssize_t res = syscall(SYS_FCNTL, fd, request, va_arg(args, uint64_t));

        SYS_RETURN_ERR(res);

        *result = res;

        return 0;
    }

    int sys_pselect(int nfds, fd_set *readfds, fd_set *writefds,
                    fd_set *exceptfds, const struct timespec *timeout, const sigset_t *sigmask, int *num_events) {
        (void) sigmask;
        ssize_t res = syscall(SYS_SELECT, nfds, readfds, writefds,
                              exceptfds, timeout, 0);

        SYS_RETURN_ERR(res);

        *num_events = res;

        return 0;
    }

    int sys_chmod(const char *pathname, mode_t mode) {
        return sys_fchmodat(AT_FDCWD, pathname, mode, 0);
    }

    int sys_fchmod(int fd, mode_t mode) {
        return sys_fchmodat(fd, nullptr, mode, 0);
    }

    int sys_fchmodat(int fd, const char *pathname, mode_t mode, int flags) {
        ssize_t res = syscall(SYS_CHMOD, fd, pathname, pathname ? strlen(pathname) : 0, mode, flags);

        SYS_RETURN_ERR(res);

        return 0;
    }

    int sys_fchownat(int dirfd, const char *pathname, uid_t owner, gid_t group, int flags) {
        (void) dirfd;
        (void) pathname;
        (void) owner;
        (void) group;
        (void) flags;
        mlibc::infoLogger() << "mlibc: sys_fchownat is a stub!" << frg::endlog;
        return 0;
    }

    int sys_umask(mode_t mode, mode_t *old) {
        (void) mode;
        mlibc::infoLogger() << "mlibc: sys_umask is a stub, hardcoding 022!" << frg::endlog;
        *old = 022;
        return 0;
    }

    int sys_madvise(void *addr, size_t length, int advice) {
        (void) addr;
        (void) length;
        (void) advice;
        mlibc::infoLogger() << "mlibc: sys_madvise is a stub!" << frg::endlog;
        return 0;
    }

    int sys_pipe(int *fds, int flags) {
        ssize_t res = syscall(SYS_PIPE, fds, flags);

        SYS_RETURN_ERR(res);

        return 0;
    }
}

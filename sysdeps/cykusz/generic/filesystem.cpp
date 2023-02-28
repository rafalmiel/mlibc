#include <cykusz/syscall.h>

#include <asm/ioctls.h>
#include <bits/ensure.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <dirent.h>
#include <limits.h>
#include <fcntl.h>

#include <mlibc/all-sysdeps.hpp>
#include <mlibc/debug.hpp>

namespace mlibc{

	int sys_write(int fd, const void* buffer, size_t count, ssize_t* written){
		long ret = syscalln3(SYS_WRITE, fd, (uintptr_t)buffer, count);

		if(ret < 0)
			return -ret;

		*written = ret;
		return 0;
	}

	int sys_read(int fd, void *buf, size_t count, ssize_t *bytes_read) {
		long ret = syscalln3(SYS_READ, fd, (uintptr_t)buf, count);

		if(ret < 0)
			return -ret;

		*bytes_read = ret;
		return 0;
	}
	
	int sys_pwrite(int fd, const void* buffer, size_t count, off_t off, ssize_t* written){
		long ret = syscalln4(SYS_PWRITE, fd, (uintptr_t)buffer, count, (uint64_t)off);

		if(ret < 0)
			return -ret;

		*written = ret;

		return 0;
	}
	
	int sys_pread(int fd, void *buf, size_t count, off_t off, ssize_t *bytes_read) {
		long ret = syscalln4(SYS_PREAD, fd, (uintptr_t)buf, count, (uint64_t)off);
		if(ret < 0)
			return -ret;

		*bytes_read = ret;

		return 0;
	}

	int sys_seek(int fd, off_t offset, int whence, off_t *new_offset) {
		ssize_t res = syscalln3(SYS_SEEK, (uint64_t)fd, (uint64_t)offset, (uint64_t)whence);

		if (res < 0)
			return -res;

		*new_offset = res;

		return 0;
	}


	int sys_open(const char* filename, int flags, mode_t mode, int* fd){
		ssize_t res = syscalln4(SYS_OPEN, (uint64_t)AT_FDCWD, (uint64_t)filename, (uint64_t)strlen(filename), (uint64_t)flags);

		if (res < 0)
			return -res;

		*fd = res;

		return 0;
	}

	int sys_openat(int dirfd, const char* filename, int flags, int* fd){
		ssize_t res = syscalln4(SYS_OPEN, (uint64_t)dirfd, (uint64_t)filename, (uint64_t)strlen(filename), (uint64_t)flags);

		if (res < 0)
			return -res;

		*fd = res;

		return 0;
	}

	int sys_close(int fd){
		ssize_t res = syscalln1(SYS_CLOSE, (uint64_t)fd);

		if (res < 0)
			return -res;

		return 0;
	}

	int sys_access(const char *path, int mode) {
		ssize_t ret = syscalln5(SYS_ACCESS, (uint64_t)AT_FDCWD, (uint64_t)path, (uint64_t)strlen(path), (uint64_t)mode, 0);

		if (ret < 0) {
			return -ret;
		}

		return 0;
	}

	int sys_fsync(int fd) {
		ssize_t ret = syscalln1(SYS_FSYNC, (uint64_t)fd);

		if (ret < 0) {
			return -ret;
		}

		return 0;
	}

	int sys_faccessat(int dirfd, const char *pathname, int mode, int flags) {
		ssize_t ret = syscalln5(SYS_ACCESS, (uint64_t)dirfd, (uint64_t)pathname, (uint64_t)strlen(pathname), (uint64_t)mode, (uint64_t)flags);

		if (ret < 0) {
			return -ret;
		}

		return 0;
	}
	
	int sys_stat(fsfd_target fsfdt, int fd, const char *path, int flags, struct stat *statbuf){
		(void) flags;
		ssize_t res;

		if (!path) {
			res = syscalln2(SYS_FSTAT, (uint64_t)fd, (uint64_t)statbuf);
		} else {
			res = syscalln3(SYS_STAT, (uint64_t)path, (uint64_t)strlen(path), (uint64_t)statbuf);
		}

		if (res < 0) {
			return -res;
		}

		return 0;
	}

	int sys_ioctl(int fd, unsigned long request, void *arg, int *result){
		ssize_t res = syscalln3(SYS_IOCTL, (uint64_t)fd, (uint64_t)request, (uint64_t)arg);

		if (res < 0) {
			return -res;
		}

		*result = res;

		return 0;
	}

	int sys_isatty(int fd) {
		if (fd == 0 || fd == 1 || fd == 2) {
			return 0;
		}
		return ENOTTY;
	}

	//#ifndef MLIBC_BUILDING_RTDL

    int sys_tcgetattr(int fd, struct termios *attr) {
        int ret;

        if (int r = sys_ioctl(fd, TCGETS, attr, &ret) != 0) {
            return r;
        }

        return ret;
    }

    int sys_tcsetattr(int fd, int optional_action, const struct termios *attr) {
		int ret;

		switch (optional_action) {
		case TCSANOW:
				optional_action = TCSETS; break;
			case TCSADRAIN:
				optional_action = TCSETS; break;
			case TCSAFLUSH:
				optional_action = TCSETS; break;
			default:
				__ensure(!"Unsupported tcsetattr");
		}

		if (int r = sys_ioctl(fd, optional_action, (void *)attr, &ret) != 0) {
			return r;
		}

		return ret;
    }

	int sys_poll(struct pollfd *fds, nfds_t count, int timeout, int *num_events){
		mlibc::infoLogger() << "mlibc: poll is a stub" << frg::endlog;
		return -1;
	}

	int sys_mkdir(const char* path, mode_t mode) {
		ssize_t ret = syscalln2(SYS_MKDIR, (uint64_t)path, (uint64_t)strlen(path));

		if (ret < 0) {
			return -ret;
		}

		return 0;
	}

	int sys_rmdir(const char* path){
		return -1;
	}

	int sys_link(const char* srcpath, const char* destpath){
		return -1;
	}

	int sys_unlink(const char *path) {
		ssize_t ret = syscalln4(SYS_UNLINK, (uint64_t)AT_FDCWD, (uint64_t)path, (uint64_t)strlen(path), 0);

		if (ret < 0) {
			return -ret;
		}

		return 0;
	}

	int sys_unlinkat(int fd, const char *path, int flags) {
		ssize_t ret = syscalln4(SYS_UNLINK, (uint64_t)fd, (uint64_t)path, (uint64_t)strlen(path), (uint64_t)flags);

		if (ret < 0) {
			return -ret;
		}

		return 0;
	}

	int sys_read_entries(int handle, void *buffer, size_t max_size, size_t *bytes_read){
		return -1;
	}

	int sys_open_dir(const char* path, int* handle){
		return -1;
	}

	int sys_rename(const char* path, const char* new_path){
		return -1;
	}
	
	int sys_readlink(const char *path, void *buffer, size_t max_size, ssize_t *length){
		return -1;
	}

	int sys_dup(int fd, int flags, int* newfd){
		ssize_t res = syscalln2(SYS_DUP, (uint64_t)fd, (uint64_t)flags);

		if (res < 0) {
			return -res;
		}

		*newfd = res;

		return 0;
	}

	int sys_dup2(int fd, int flags, int newfd){
		ssize_t res = syscalln3(SYS_DUP2, (uint64_t)fd, (uint64_t)newfd, (uint64_t)flags);

		if (res < 0) {
			return -res;
		}

		return 0;
	}

	int sys_fcntl(int fd, int request, va_list args, int* result){
		return -1;
	}

	int sys_pselect(int nfds, fd_set* readfds, fd_set* writefds,
		fd_set *exceptfds, const struct timespec* timeout, const sigset_t* sigmask, int *num_events) {
		ssize_t res = syscalln6(SYS_SELECT, (uint64_t)nfds, (uint64_t)readfds, (uint64_t)writefds, 0, (uint64_t)timeout, 0);

		if (res < 0) {
			return -res;
		}

		*num_events = res;

		return 0;
	}

	int sys_chmod(const char *pathname, mode_t mode){
		return -1;
	}
    int sys_madvise(void *addr, size_t length, int advice) {
        mlibc::infoLogger() << "mlibc: sys_madvise is a stub!" << frg::endlog;
        return 0;
    }
	int sys_pipe(int *fds, int flags) {
		ssize_t res = syscalln2(SYS_PIPE, (uint64_t)fds, (uint64_t)flags);

		if (res < 0) {
			return -res;
		}

		return 0;
	}
	//#endif
} 

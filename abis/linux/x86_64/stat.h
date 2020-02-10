#ifndef _ABIBITS_STAT_H
#define _ABIBITS_STAT_H

#include <bits/posix/uid_t.h>
#include <bits/posix/gid_t.h>
#include <bits/posix/off_t.h>
#include <bits/posix/mode_t.h>
#include <bits/posix/dev_t.h>
#include <bits/posix/ino_t.h>
#include <bits/posix/blksize_t.h>
#include <bits/posix/blkcnt_t.h>
#include <bits/posix/nlink_t.h>
#include <bits/ansi/time_t.h>
#include <bits/ansi/timespec.h>

#define S_IFMT 0x0F000
#define S_IFBLK 0x06000
#define S_IFCHR 0x02000
#define S_IFIFO 0x01000
#define S_IFREG 0x08000
#define S_IFDIR 0x04000
#define S_IFLNK 0x0A000
#define S_IFSOCK 0x0C000

#define S_IRWXU 0700
#define S_IRUSR 0400
#define S_IWUSR 0200
#define S_IXUSR 0100
#define S_IRWXG 070
#define S_IRGRP 040
#define S_IWGRP 020
#define S_IXGRP 010
#define S_IRWXO 07
#define S_IROTH 04
#define S_IWOTH 02
#define S_IXOTH 01
#define S_ISUID 04000
#define S_ISGID 02000
#define S_ISVTX 01000

#ifdef __cplusplus
extern "C" {
#endif

struct stat {
	dev_t st_dev;
	ino_t st_ino;
	nlink_t st_nlink;
	mode_t st_mode;
	uid_t st_uid;
	gid_t st_gid;
	unsigned int __pad0;
	dev_t st_rdev;
	off_t st_size;
	blksize_t st_blksize;
	blkcnt_t st_blocks;
	struct timespec st_atim;
	struct timespec st_mtim;
	struct timespec st_ctim;
	long __unused[3];
};

#ifdef __cplusplus
}
#endif

#endif // _ABIBITS_STAT_H

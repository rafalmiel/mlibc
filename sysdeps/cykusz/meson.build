
rtdl_sources = files(
	'generic/cykusz.cpp',
	'generic/filesystem.cpp',
)

libc_sources = files(
	'generic/signals.cpp',
	'generic/cykusz.cpp',
	'generic/filesystem.cpp',
	'generic/sockets.cpp',
	'generic/entry.cpp',
	'generic/thread_entry.S',
	'generic/thread.cpp'
)

if host_machine.cpu_family() == 'x86_64'
	libc_sources += files(
		'x86_64/signals.S',
	)
else
	error('Unknown architecture')
endif

if not no_headers
	install_headers(
		'include/abi-bits/auxv.h',
		'include/abi-bits/ioctls.h',
		'include/abi-bits/access.h',
		'include/abi-bits/seek-whence.h',
		'include/abi-bits/vm-flags.h',
		'include/abi-bits/errno.h',
		'include/abi-bits/fcntl.h',
		'include/abi-bits/in.h',
		'include/abi-bits/resource.h',
		'include/abi-bits/fsblkcnt_t.h',
		'include/abi-bits/signal.h',
		'include/abi-bits/stat.h',
		'include/abi-bits/socket.h',
		'include/abi-bits/suseconds_t.h',
		'include/abi-bits/termios.h',
		'include/abi-bits/time.h',
		'include/abi-bits/blkcnt_t.h',
		'include/abi-bits/blksize_t.h',
		'include/abi-bits/dev_t.h',
		'include/abi-bits/gid_t.h',
		'include/abi-bits/ino_t.h',
		'include/abi-bits/mode_t.h',
		'include/abi-bits/nlink_t.h',
		'include/abi-bits/pid_t.h',
		'include/abi-bits/uid_t.h',
		'include/abi-bits/utsname.h',
		'include/abi-bits/limits.h',
		'include/abi-bits/wait.h',
		'include/abi-bits/clockid_t.h',
		'include/abi-bits/socklen_t.h',
		'include/abi-bits/fsfilcnt_t.h',
		'include/abi-bits/poll.h',
		subdir: 'abi-bits'
	)
	install_headers(
		'include/cykusz/syscall.h',
		subdir: 'cykusz'
	)
endif

if not headers_only
	crt = custom_target('crt0',
		build_by_default: true,
		command: c_compiler.cmd_array() + ['-c', '-o', '@OUTPUT@', '@INPUT@'],
		input: 'crt-x86_64/crt0.S',
		output: 'crt0.o',
		install: true,
		install_dir: get_option('libdir')
	)

	if host_machine.cpu_family() == 'x86_64'
		custom_target('Scrt1',
			build_by_default: true,
			command: c_compiler.cmd_array() + ['-c', '-o', '@OUTPUT@', '@INPUT@'],
			input: 'crt-x86_64/Scrt1.S',
			output: 'Scrt1.o',
			install: true,
			install_dir: get_option('libdir')
		)
	endif
endif


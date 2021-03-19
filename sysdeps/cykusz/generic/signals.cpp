#include <cykusz/syscall.h>
#include <stddef.h>
#include <bits/ensure.h>
#include <abi-bits/pid_t.h>
#include <mlibc/debug.hpp>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/thread-entry.hpp>
#include <errno.h>

extern "C" void __mlibc_signal_restore();

namespace mlibc {
	int sys_sigaction(int number, const struct sigaction * action,
				struct sigaction * saved_action) {

		ssize_t res = syscalln4(SYS_SIGACTION, number, (uint64_t)(action->sa_handler), static_cast<uint64_t>(action->sa_flags), (uint64_t)(__mlibc_signal_restore));
		
		if (res < 0)
			return -res;

		return 0;
	}
}

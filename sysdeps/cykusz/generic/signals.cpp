#include <cykusz/syscall_internal.h>
#include <stddef.h>
#include <bits/ensure.h>
#include <abi-bits/pid_t.h>
#include <mlibc/debug.hpp>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/thread-entry.hpp>
#include <errno.h>

extern "C" void __mlibc_signal_restore();

namespace mlibc {
    int sys_sigaction(int number, const struct sigaction *action,
                      struct sigaction *saved_action) {

#if 0
        mlibc::infoLogger() << "sys_sigaction: signal " << number << frg::endlog;
        mlibc::infoLogger() << "sys_sigaction: size: " << sizeof(*action) << frg::endlog;
        if (action) {
            mlibc::infoLogger() << "sys_sigaction: handler " << (int64_t)action->sa_handler << frg::endlog;
            mlibc::infoLogger() << "sys_sigaction: action " << (int64_t)action->sa_sigaction << frg::endlog;
            mlibc::infoLogger() << "sys_sigaction: mask " << (int64_t)action->sa_mask << frg::endlog;
            mlibc::infoLogger() << "sys_sigaction: flags " << (int64_t)action->sa_flags << frg::endlog;
        }
#endif
        if (action) {
            const_cast<struct sigaction*>(action)->sa_restorer = __mlibc_signal_restore;
        }

        ssize_t res = syscall(
                SYS_SIGACTION,
                number,
                action,
                saved_action);

        SYS_RETURN_ERR(res);

        return 0;
    }

    int sys_sigprocmask(int how, const sigset_t *set, sigset_t *old) {
        uint64_t old_p = 0;

        if (old) {
            old_p = reinterpret_cast<uint64_t>(old);
        }

        ssize_t res = syscall(SYS_SIGPROCMASK, how, set, old_p);

        SYS_RETURN_ERR(res);

        return 0;
    }
}

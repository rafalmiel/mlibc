#ifndef SYSCALL_INTERNAL_H
#define SYSCALL_INTERNAL_H

#include <cykusz/syscall.h>

inline int sc_error(long ret) {
    if (ret < 0)
        return -ret;
    return 0;
}

#define SYS_RETURN_ERR(e) \
    do {                  \
        if (int err = sc_error(e); err) \
            return err;   \
    } while (0)

#endif //SYSCALL_INTERNAL_H

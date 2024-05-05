#include <cykusz/syscall_internal.h>

#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include <mlibc/all-sysdeps.hpp>
#include <mlibc/debug.hpp>

namespace mlibc {
    int sys_socket(int domain, int type, int protocol, int *fd) {
        ssize_t res = syscall(SYS_SOCKET, domain, type, protocol);

        SYS_RETURN_ERR(res);

        *fd = res;

        return 0;
    }

    int sys_socketpair(int domain, int type_and_flags, int proto, int *fds) {
        ssize_t res = syscall(SYS_SOCKETPAIR, domain, type_and_flags, proto, fds);

        SYS_RETURN_ERR(res);

        return 0;
    }

    int sys_bind(int sockfd, const struct sockaddr *addr_ptr, socklen_t addrlen) {
        ssize_t res = syscall(SYS_BIND, sockfd, addr_ptr, addrlen);

        SYS_RETURN_ERR(res);

        return 0;
    }

    int sys_connect(int sockfd, const struct sockaddr *addr_ptr, socklen_t addrlen) {
        ssize_t res = syscall(SYS_CONNECT, sockfd, addr_ptr, addrlen);

        SYS_RETURN_ERR(res);

        return 0;
    }

    int sys_accept(int fd, int *newfd, struct sockaddr *addr_ptr, socklen_t *addr_length, int flags) {
        ssize_t res = syscall(SYS_ACCEPT, fd, addr_ptr, addr_length, flags);

        SYS_RETURN_ERR(res);

        *newfd = res;

        return 0;
    }

    int sys_listen(int fd, int backlog) {
        ssize_t res = syscall(SYS_LISTEN, fd, backlog);

        SYS_RETURN_ERR(res);

        return 0;
    }

    int sys_msg_recv(int sockfd, struct msghdr *hdr, int flags, ssize_t *length) {
        ssize_t res = syscall(SYS_MSGRECV, sockfd, hdr, flags);

        SYS_RETURN_ERR(res);

        *length = res;

        return 0;
    }

    int sys_msg_send(int sockfd, const struct msghdr *hdr, int flags, ssize_t *length) {
        ssize_t res = syscall(SYS_MSGSEND, sockfd, hdr, flags);

        SYS_RETURN_ERR(res);

        *length = res;

        return 0;
    }

    int sys_setsockopt(int fd, int layer, int number, const void *buffer, socklen_t size) {
        ssize_t res = syscall(SYS_SETSOCKOPT, fd, layer, number, buffer, size);

        SYS_RETURN_ERR(res);

        return 0;
    }

    int sys_getsockopt(int fd, int layer, int number, void *__restrict buffer, socklen_t *__restrict size) {
        ssize_t res = syscall(SYS_GETSOCKOPT, fd, layer, number, buffer, size);

        SYS_RETURN_ERR(res);

        return 0;
    }

    int sys_gethostname(char *buffer, size_t bufsize) {
        mlibc::infoLogger() << "mlibc: gethostname always returns cykusz-os" << frg::endlog;
        char name[11] = "cykusz-os\0";
        if (bufsize < 11)
            return ENAMETOOLONG;
        strncpy(buffer, name, 11);
        return 0;
    }
}

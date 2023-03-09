#include <cykusz/syscall.h>

#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include <mlibc/all-sysdeps.hpp>
#include <mlibc/debug.hpp>

namespace mlibc{
    int sys_socket(int domain, int type, int protocol, int *fd){
        ssize_t res = syscalln3(SYS_SOCKET, (uint64_t)domain, (uint64_t)type, (uint64_t)protocol);

        if (res < 0) {
            return -res;
        }

        *fd = res;

        return 0;
    }

    int sys_bind(int sockfd, const struct sockaddr *addr_ptr, socklen_t addrlen){
        ssize_t res = syscalln3(SYS_BIND, (uint64_t)sockfd, (uint64_t)addr_ptr, (uint64_t)addrlen);

        if (res < 0) {
            return -res;
        }

        return 0;
    }

    int sys_connect(int sockfd, const struct sockaddr *addr_ptr, socklen_t addrlen){
        ssize_t res = syscalln3(SYS_CONNECT, (uint64_t)sockfd, (uint64_t)addr_ptr, (uint64_t)addrlen);

        if (res < 0) {
            return -res;
        }

        return 0;
    }

    int sys_accept(int fd, int *newfd, struct sockaddr *addr_ptr, socklen_t *addr_length){
        ssize_t res = syscalln3(SYS_ACCEPT, (uint64_t)fd, (uint64_t)addr_ptr, (uint64_t)addr_length);

        if (res < 0) {
            return -res;
        }

        *newfd = res;

        return 0;
    }

    int sys_listen(int fd, int backlog){
        ssize_t res = syscalln2(SYS_LISTEN, (uint64_t)fd, (uint64_t)backlog);

        if (res < 0) {
            return -res;
        }

        return 0;
    }

    int sys_msg_recv(int sockfd, struct msghdr *hdr, int flags, ssize_t *length){
        ssize_t res = syscalln3(SYS_MSGRECV, (uint64_t)sockfd, (uint64_t)hdr, (uint64_t)flags);

        if (res < 0) {
            return -res;
        }

        *length = res;

        return 0;
    }

    int sys_msg_send(int sockfd, const struct msghdr *hdr, int flags, ssize_t *length){
        ssize_t res = syscalln3(SYS_MSGSEND, (uint64_t)sockfd, (uint64_t)hdr, (uint64_t)flags);

        if (res < 0) {
            return -res;
        }

        *length = res;

        return 0;
    }

    int sys_setsockopt(int fd, int layer, int number, const void *buffer, socklen_t size) {
        ssize_t res = syscalln5(SYS_SETSOCKOPT, (uint64_t)fd, (uint64_t)layer, (uint64_t)number, (uint64_t)buffer, (uint64_t)size);

        if (res < 0) {
            return -res;
        }

        return 0;
    }

    int sys_getsockopt(int fd, int layer, int number, void *__restrict buffer, socklen_t *__restrict size) {
        ssize_t res = syscalln5(SYS_GETSOCKOPT, (uint64_t)fd, (uint64_t)layer, (uint64_t)number, (uint64_t)buffer, (uint64_t)size);

        if (res < 0) {
            return -res;
        }

        return 0;
    }

    int sys_gethostname(char *buffer, size_t bufsize) {
        mlibc::infoLogger() << "mlibc: gethostname always returns cykusz-os" << frg::endlog;
        char name[11] = "cykusz-os\0";
        if(bufsize < 11)
            return ENAMETOOLONG;
        strncpy(buffer, name, 11);
        return 0;
    }
}

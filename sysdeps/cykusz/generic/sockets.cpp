#include <cykusz/syscall.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include <mlibc/all-sysdeps.hpp>

namespace mlibc{
    int sys_socket(int domain, int type, int protocol, int *fd){
	    return -1;
    }

    int sys_bind(int sockfd, const struct sockaddr *addr_ptr, socklen_t addrlen){
	    return -1;
    }

    int sys_connect(int sockfd, const struct sockaddr *addr_ptr, socklen_t addrlen){
	    return -1;
    }

    int sys_accept(int fd, int *newfd){
	    return -1;
    }

    int sys_listen(int fd, int backlog){
	    return -1;
    }

    int sys_msg_recv(int sockfd, struct msghdr *hdr, int flags, ssize_t *length){
	    return -1;
    }

    int sys_msg_send(int sockfd, const struct msghdr *hdr, int flags, ssize_t *length){
	    return -1;
    }

    int sys_setsockopt(int fd, int layer, int number, const void *buffer, socklen_t size){
	    return -1;
    }
}

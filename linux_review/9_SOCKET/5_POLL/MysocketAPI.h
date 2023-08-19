#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>

int SOCKET(int domain, int type, int protocol);

int BIND(int sockfd, const struct sockaddr* addr, socklen_t addrlen);

int LISTEN(int sockfd, int backlog);

int ACCEPT(int sockfd, struct sockaddr* addr, socklen_t* addrlen);

int CONNECT(int sockfd, const struct sockaddr* addr, socklen_t addrlen);

//可以将常用的读写函数进包裹

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>

// 对于套接字的二次包裹，在原生系统函数的基础上，集成一些新的功能 
// 如 错误判断等 
int SOCKET(int domain, int type, int protocol);

int BIND(int sockfd, const struct sockaddr* addr, socklen_t addrlen);

int LISTEN(int sockfd, int backlog);

int ACCEPT(int sockfd, struct sockaddr* addr, socklen_t* addrlen);

int CONNECT(int sockfd, const struct sockaddr* addr, socklen_t addrlen);

//可以将常用的读写函数进包裹

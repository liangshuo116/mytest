#include <MysocketAPI.h>

int SOCKET(int domain, int type, int protocol)
{
	int sockfd;
	if ((sockfd = socket(domain, type, protocol)) == -1)
	{
		perror("Sock Create Failed.\n");
		return sockfd;
	}
	printf("Sock Create Successly, sockfd %d\n", sockfd);
	return sockfd;
}

int BIND(int sockfd, const struct sockaddr* addr, socklen_t addrlen)
{
	int reval;
	if ((reval = bind(sockfd, addr, addrlen)) == -1)
	{
		perror("Bind Call Failed.\n");
		return reval;
	}
	printf("Bind Successly.\n");
	return reval;
}

int LISTEN(int sockfd, int backlog)
{
	int reval;
	if ((reval = listen(sockfd, backlog)) == -1)
	{
		perror("Listen Call Failed.\n");
		return reval;
	}
	printf("Listen Successly.\n");
	return reval;
}
 
int ACCEPT(int sockfd, struct sockaddr* addr, socklen_t* addrlen)
{
	int cfd;
	if ((cfd = accept(sockfd, addr, addrlen)) == -1)
	{
		perror("Accept Call Successly.\n");
		return cfd;
	}
	printf("Accept Successly.\n");
	return cfd;
}
 
int CONNECT(int sockfd, const struct sockaddr* addr, socklen_t addrlen)
{
	int reval;
	if ((reval = connect(sockfd, addr, addrlen)) == -1)
	{
		perror("Connect Call Failed.\n");
		return reval;
	}
	printf("Connect Successly. Connect client sockfd %d\n", sockfd);
	return reval;
}

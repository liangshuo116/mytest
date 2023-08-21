#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <MysocketAPI.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

//#define SERVER_IP "42.193.104.238" 
#define SERVER_IP "192.168.221.174" 
//#define SERVER_PORT 8080
#define SERVER_PORT 54321

int main(void)
{
	char Response_msg[1024]; 	// 服务端回应数据
	char Request_msg[1500]; 	// 客户端请求数据
	char server_ip[16]; 		// 服务端IP
	int recvlen;
	socklen_t Addrlen;

	struct sockaddr_in ServerAddr;
	bzero(&ServerAddr, sizeof(ServerAddr));
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(SERVER_PORT);
	inet_pton(AF_INET, SERVER_IP, &ServerAddr.sin_addr.s_addr);
	//ServerAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

	int Client_fd;
	Client_fd = SOCKET(AF_INET, SOCK_STREAM, 0);
	//请求连接
	int err;
	Addrlen = sizeof(ServerAddr);
	CONNECT(Client_fd, (struct sockaddr*)&ServerAddr, Addrlen);

	//读取服务器端返回数据
	bzero(Response_msg, sizeof(Response_msg));
	recvlen = recv(Client_fd, Response_msg, sizeof(Response_msg), 0); // 阻塞接收数据
	if (recvlen > 0)
	{
		printf("对你说：%s\n", Response_msg);
	}
	
	bzero(Request_msg, sizeof(Request_msg));
	// read默认为阻塞读取，现在改为非阻塞读取
	int len;
	int flags;
	flags = fcntl(STDIN_FILENO, F_GETFL, 0); 	// 将 STD_IN 默认的 flags 选项获取并返回
	flags |= FNONBLOCK; 						// 加入非阻塞标签
	fcntl(STDIN_FILENO, F_SETFL, flags); 		// 将 STDIN_FILENO 设置为非阻塞
again:
	//读取标准输入，而后发送信息（非阻塞）
	while ((len = read(STDIN_FILENO, Request_msg, sizeof(Request_msg))) > 0)
	{
		//向服务器端发送数据
		send(Client_fd, Request_msg, strlen(Request_msg), 0);
		usleep(100000); //等待服务端处置

		//读取服务器端返回数据
		bzero(Response_msg, sizeof(Response_msg));
		recvlen = recv(Client_fd, Response_msg, sizeof(Response_msg), MSG_DONTWAIT); // 非阻塞接收数据
		if (recvlen == -1)
		{
			if (errno == EAGAIN) 	// 未收到服务端数据
			{
				bzero(Request_msg, sizeof(Request_msg)); 	// 清空请求数据，方便下次发送
				continue;
			}
			else
			{
				perror("Recv Call Failed!\n");
				exit(0);
			}
		}
		printf("%s", Response_msg);
		bzero(Request_msg, sizeof(Request_msg));
		bzero(Response_msg, sizeof(Response_msg));
	}
	if (len == -1)
	{
		if (errno == EAGAIN) // 未从标准输入读到数据
		{
			if (recv(Client_fd, Response_msg, sizeof(Response_msg), MSG_DONTWAIT) == -1)
			{
				if (errno == EAGAIN) 	// 未收到服务端发来的数据
					bzero(Request_msg, sizeof(Request_msg));
				else
					perror("Recv Call Failed!\n");
			}
			printf("%s", Response_msg); // 收到服务端发来的数据， 打印
			bzero(Request_msg, sizeof(Request_msg));
			bzero(Response_msg, sizeof(Response_msg));
		}
	}
	goto again; 	// 回到again处，一直循环读取标准输入
	
	close(Client_fd);
	return 0;
}

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <MysocketAPI.h>

//#define SERVER_IP "42.193.104.238" 
#define SERVER_IP "192.168.221.170" 
//#define SERVER_PORT 8080
#define SERVER_PORT 54321

int main(void)
{
	char Response_msg[1024];
	char Request[1500];
	char server_ip[16];
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

	while (shutdown)
	{
		//读取服务器端返回数据
		bzero(Request, sizeof(Request));
		recvlen = recv(Client_fd, Request, sizeof(Request), MSG_DONTWAIT); // 非阻塞接收数据
		if (recvlen > 0)
		{
			printf("对你说：%s\n", Request);
		}
		//向服务器端发送数据
		bzero(Response_msg, sizeof(Response_msg));
		//sprintf(Response_msg, "localtime\n");
		//读取标准输入，而后发送信息
		//fgets(Response_msg, sizeof(Response_msg), stdin);
		read(STDIN_FILENO, Response_msg, sizeof(Response_msg)); //阻塞读标准输入
		send(Client_fd, Response_msg, strlen(Response_msg), 0);
	}
	close(Client_fd);
	return 0;
}

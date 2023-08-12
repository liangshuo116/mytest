#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include <MysocketAPI.h>
#include <time.h>

#define SERVER_IP 		"192.168.221.161"
#define SERVER_PORT 	54321
#define shutdown 		1

int main(void)
{
	char Response_msg[1024]; 	// 服务端回应数据
	char Request_msg[1024]; 	// 客户端请求数据
	char client_ip[16]; 		// 客户端 IP
	int recvlen; 				// 接收数据的大小
	time_t tp; 					// 时间变量
	char time_buffer[1024]; 	// 时间字符串数据
	socklen_t Addrlen; 			// 网络信息结构体大小

	int client_array[1024]; 	// 存储客户端socket的数组
	for (int i = 0; i < 1024; i++)
	{
		client_array[i] = -1;
	}
	// 自定义服务端网络地址信息
	struct sockaddr_in ServerAddr, ClientAddr;
	bzero(&ServerAddr, sizeof(ServerAddr));
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(SERVER_PORT);
	ServerAddr.sin_addr.s_addr = INADDR_ANY; 	// 本机任意IP

	int Server_fd, Client_fd;
	Server_fd = SOCKET(AF_INET, SOCK_STREAM, 0); 						// 1.创建套接字
	BIND(Server_fd, (struct sockaddr*)&ServerAddr, sizeof(ServerAddr)); // 2.绑定网络信息
	LISTEN(Server_fd, 128); 	 // 3.对 socket 链接事件进行监听

	while (shutdown)
	{
		printf("Server Version 0.0.1 Accept Start..\n");
		// 4.阻塞等待客户端连接
		Addrlen = sizeof(ClientAddr);
		Client_fd = ACCEPT(Server_fd, (struct sockaddr*)&ClientAddr, &Addrlen);
		// 成功连接，存入客户端socket数组
		if (Client_fd > 0)
		{
			for (int i = 0; i < 1024; i++)
			{
				if (client_array[i] == -1)
				{
					client_array[i] = Client_fd;
					break;
				}
			}
		}

		//向客户反馈成功信息
		bzero(Response_msg, sizeof(Response_msg));
		bzero(client_ip, sizeof(client_ip));
		inet_ntop(AF_INET, &ClientAddr.sin_addr.s_addr, client_ip, sizeof(client_ip));
		printf("Client ip %s port %d , Connection Successly.\n", client_ip, ntohs(ClientAddr.sin_port)); 	//显示客户端连接信息
		sprintf(Response_msg, "Hi, %s , welcome to Test Server [%s] Version 0.0.1~\n", client_ip, SERVER_IP);
		send(Client_fd, Response_msg, strlen(Response_msg), 0); 	//向客户端发送响应数据
		//读取一次客户端请求数据，读取完毕关闭连接
		bzero(Request_msg, sizeof(Request_msg));
		sleep(10); 	//等待客户端10秒（正常不能这么做）
		if ((recvlen = recv(Client_fd, Request_msg, sizeof(Request_msg), MSG_DONTWAIT)) == 0) 	//非阻塞读取数据，客户端断开
		{
			printf("%s Exit, Close.\n", client_ip);
		}
		if (recvlen == -1)
		{
			if (errno == EAGAIN) //如果缓冲区为空，未读到客户端数据则报错
			{
				printf("Recv %s Failed, error its EAGAIN, close..\n", client_ip);
			}
			else
			{
				perror("Server Version 0.0.1, Recv Failed.\n");
				exit(0);
			}
		}
		// 客户端发送 localtime 请求本地时间
		if (strcmp(Request_msg, "localtime\n") == 0)
		{
			bzero(time_buffer, sizeof(time_buffer));
			tp = time(NULL);
			ctime_r(&tp, time_buffer); 	//计算时间
			send(Client_fd, time_buffer, strlen(time_buffer), 0); 	// 发送给客户端
			printf("Response (%s) local time\n", client_ip);
		}
		else // 否则打印客户端数据
		{
			printf("[%s] 对你说：%s\n", client_ip, Request_msg);
			close(Client_fd);
			printf("Continue...\n");
		}
		close(Client_fd); 				// 关闭连接
		for (int i = 0; i < 1024; i++) 	// 将当前socket从已连接数组中移除
		{
			if (client_array[i] == Client_fd)
			{
				client_array[i] = -1;
				break;
			}
		}
	}
	close(Server_fd); 	// 关闭服务器端套接字
	
	return 0;
}

#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <MysocketAPI.h>
#include <time.h>
#include <string.h>
#include <sys/select.h>

// 多线程结构服务器

//#define SERVER_IP 		"192.168.221.170"
#define SERVER_PORT 	54321
#define shutdown 		1

typedef struct
{
	int 	client_fd; 		// 客户端套接字
	char 	client_ip[16]; 	// 客户端 IP
}Sock_t; 	// 客户端的网络信息

int main(void)
{
	Sock_t socket_list[1024];
	for (int i = 0 ; i < 1024; i++)			// 初始化客户端socket数组
	{
		socket_list[i].client_fd= -1;
	}

	char Response_msg[1024]; 	// 服务端回应数据
	socklen_t Addrlen; 			// 网络信息结构体大小
	struct sockaddr_in ServerAddr, ClientAddr; 	// 自定义服务端网络地址信息
	bzero(&ServerAddr, sizeof(ServerAddr));
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(SERVER_PORT);
	ServerAddr.sin_addr.s_addr = INADDR_ANY; 	// 本机任意IP
	// 获取当前用于连接的本机IP
	char Server_ip[16];
	inet_ntop(AF_INET, &ServerAddr.sin_addr.s_addr, Server_ip, sizeof(Server_ip));

	int Server_fd, Client_fd;
	Server_fd = SOCKET(AF_INET, SOCK_STREAM, 0); 						// 1.创建套接字
	BIND(Server_fd, (struct sockaddr*)&ServerAddr, sizeof(ServerAddr)); // 2.绑定网络信息
	LISTEN(Server_fd, 128); 											// 3.对 socket 链接事件进行监听
	printf("Select Server Version 0.0.1 Accept Start..\n");

	fd_set set, oset; 			// 初始化监听集合，set作为传入的监听集合，oset作为传出的监听集合，传入传出分离
	FD_ZERO(&set); 				// 将监听集合中所有位码初始化为 0
	FD_SET(Server_fd, &set); 	// 监听服务端 Server_fd
	int readycode = 0; 			// select返回值，就绪sockfd数量
	int maxfd = Server_fd; 		// 当前最大文件描述符为Server_fd

	int recvlen; 				// 接收数据的大小
	time_t tp; 					// 时间变量
	char time_buffer[1024]; 	// 时间字符串数据
	char Request_msg[1024]; 	// 客户端请求数据
	char Forward[2048]; 		// 转发数据
	bzero(Request_msg, sizeof(Request_msg));
	bzero(Forward, sizeof(Forward));
	int i;

	// select开始轮询监听
	while (shutdown)
	{
		oset = set; 	// 更新传出集合
		// 传入监听集合
		readycode = select(maxfd + 1, &oset, NULL, NULL, NULL);
		// 循环处理就绪的 sockfd
		if (readycode > 0)
		{
			while (readycode)
			{
				if (FD_ISSET(Server_fd, &oset) == 1)
				{
					// 处理Server_fd就绪

					Addrlen = sizeof(ClientAddr);
					Client_fd = ACCEPT(Server_fd, (struct sockaddr*)&ClientAddr, &Addrlen); 	// 4.阻塞等待客户端连接

					// 连接成功，存储客户端信息	
					for (i = 0; i < 1024; i++)
					{
						if (socket_list[i].client_fd == -1)
						{
							socket_list[i].client_fd = Client_fd;
							bzero(socket_list[i].client_ip, sizeof(socket_list[i].client_ip));
							inet_ntop(AF_INET, &ClientAddr.sin_addr.s_addr, socket_list[i].client_ip, sizeof(socket_list[i].client_ip));
							break;
						}
					}
					// 设置监听
					FD_SET(Client_fd, &set);
					// 判断最大文件描述符，实时更新
					if (Client_fd > maxfd) 
						maxfd = Client_fd;

					// 向客户反馈连接成功信息
					bzero(Response_msg, sizeof(Response_msg));
					printf("Process Pid %d Client ip %s port %d , Connection Successly.\n", getpid(), socket_list[i].client_ip, ntohs(ClientAddr.sin_port)); 	// 显示进程id以及客户端连接信息
					sprintf(Response_msg, "Hi, %s , welcome to Test Server [%s] Version 0.0.1~\n", socket_list[i].client_ip, Server_ip);
					send(socket_list[i].client_fd, Response_msg, strlen(Response_msg), 0); 	// 向客户端发送响应数据
					printf("Send Response_msg to Client:%s, ClientSock:%d\n", socket_list[i].client_ip, socket_list[i].client_fd);

					// socket就绪事件处理完毕，在就绪集合中清零
					FD_CLR(Server_fd, &oset);
				}
				else // 目前不清楚是哪个客户端
				{
					// 获取就绪的 Client_fd
					for (i = 0; i < 1024; i++)
					{
						if (socket_list[i].client_fd != -1)
						{
							if (FD_ISSET(socket_list[i].client_fd, &oset) == 1)
							{
								// 处理Client_fd就绪
								if ((recvlen = recv(socket_list[i].client_fd, Request_msg, sizeof(Request_msg), 0)) > 0) // 接收当前客户端数据
								{
									// 客户端发送 localtime 请求本地时间
									if (strcmp(Request_msg, "localtime\n") == 0)
									{
										bzero(time_buffer, sizeof(time_buffer));
										tp = time(NULL);
										ctime_r(&tp, time_buffer); 	// 计算时间
										send(socket_list[i].client_fd, time_buffer, strlen(time_buffer), 0); 	// 发送给客户端
										printf("Response (%s) local time\n", socket_list[i].client_ip);
									}
									else // 没有请求时间就将客户端数据转发给其他所有已经连接的客户端（群发）
									{
										sprintf(Forward, "[%s] 对你说：%s\n\n", socket_list[i].client_ip, Request_msg);
										printf("%s", Forward);
										for (int j = 0; j < 1024; j++)
										{
											if (socket_list[j].client_fd != -1)
											{
												send(socket_list[j].client_fd, Forward, strlen(Forward), 0);
											}
										}
									}
									// 每次接收完数据清零等待下次接收
									bzero(Request_msg, sizeof(Request_msg)); 				
									bzero(Forward, sizeof(Forward));

									FD_CLR(socket_list[i].client_fd, &oset);
								}
								else if (recvlen == -1) 	// 接收数据失败
								{
									perror("Server Version 0.0.1, Recv Failed.\n");
									close(socket_list[i].client_fd);
								}
								else if (recvlen == 0) 	// 客户端退出
								{	
									printf("%s Exit, Close.\n", socket_list[i].client_ip);
									// 两个删除
									FD_CLR(socket_list[i].client_fd, &set); // 将Client_fd从监听队列中移除，取消对其的监听
									close(socket_list[i].client_fd); 		// 关闭客户端套接字
									socket_list[i].client_fd = -1; 			// 将Client_fd从数组中删除
								}
								break;
							}
						}
					}
				}
				readycode--;
			}

		}
		else if (readycode == -1)
		{
			perror("Select Call Failed!\n");
			exit(0);
		}
	}
	close(Server_fd); 	// 5.关闭服务器端套接字

	return 0;
}

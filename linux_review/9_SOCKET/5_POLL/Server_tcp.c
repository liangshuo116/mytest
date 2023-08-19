#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <MysocketAPI.h>
#include <time.h>
#include <string.h>
#include <poll.h>

// Poll单进程一对多服务器

//#define SERVER_IP 		"192.168.221.170"
#define SERVER_PORT 	54321
#define shutdown 		1
#define MAX 			1024 	// 监听集合可监听socket最大数
/*
poll明明可以监听超过1024个socket，为什么设置4096时就报错呢？

poll的监听集合是一个用户自定义长度的结构体数组，结构体中存储了socket，
因此就不和Select一样定义结构体存储用户网络信息了（socket和IP地址，目前可以不用IP地址）
*/

int main(void)
{
	char Response_msg[1024]; 	// 服务端回应数据
	socklen_t Addrlen; 			// 网络信息结构体大小
	struct sockaddr_in ServerAddr, ClientAddr; 	// 自定义服务端网络地址信息
	bzero(&ServerAddr, sizeof(ServerAddr));
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(SERVER_PORT);
	ServerAddr.sin_addr.s_addr = INADDR_ANY; 	// 本机任意IP
	// 获取当前用于连接的本机IP
	char Server_ip[16];
	char client_ip[16];
	inet_ntop(AF_INET, &ServerAddr.sin_addr.s_addr, Server_ip, sizeof(Server_ip));

	int Server_fd, Client_fd;
	Server_fd = SOCKET(AF_INET, SOCK_STREAM, 0); 						// 1.创建套接字
	BIND(Server_fd, (struct sockaddr*)&ServerAddr, sizeof(ServerAddr)); // 2.绑定网络信息
	LISTEN(Server_fd, 128); 											// 3.对 socket 链接事件进行监听
	printf("Select Server Version 0.0.1 Accept Start..\n");

	struct pollfd listen_array[MAX]; 		// 初始化监听集合，自定义大小为 4096（与select作为区分，select最大只有1024）
	for (int i = 0; i < MAX; i++)
	{
		listen_array[i].fd = -1;
	}
	listen_array[0].fd = Server_fd; 		// 设置要监听的socket
	listen_array[0].events = POLLIN; 		// 设置监听事件，监听读事件
	// 若当前sock就绪，系统会将此sock的就绪事件传出到revents当中，便于使用者判断是否就绪
	int readycode = 0; 			// poll返回值，就绪sockfd数量

	int recvlen; 				// 接收数据的大小
	time_t tp; 					// 时间变量
	char time_buffer[1024]; 	// 时间字符串数据
	char Request_msg[1024]; 	// 客户端请求数据
	char Forward[2048]; 		// 转发数据
	bzero(Request_msg, sizeof(Request_msg));
	bzero(Forward, sizeof(Forward));
	int i;

	// poll开始轮询监听
	while (shutdown)
	{
		// 传入监听集合
		readycode = poll(listen_array, MAX, -1);
		// 循环处理就绪的 sockfd
		if (readycode > 0)
		{
			while (readycode)
			{
				if (listen_array[0].revents == POLLIN)
				{
					// 处理Server_fd就绪

					Addrlen = sizeof(ClientAddr);
					Client_fd = ACCEPT(Server_fd, (struct sockaddr*)&ClientAddr, &Addrlen); 	// 4.阻塞等待客户端连接

					// 连接成功，存储客户端信息	
					for (i = 1; i < MAX; i++)
					{
						if (listen_array[i].fd == -1)
						{
							listen_array[i].fd = Client_fd; 	// 存储客户端socket
							listen_array[i].events = POLLIN; 	// 设置监听事件
							break;
						}
					}

					// 向客户反馈连接成功信息
					inet_ntop(AF_INET, &ClientAddr.sin_addr.s_addr, client_ip, sizeof(client_ip));
					bzero(Response_msg, sizeof(Response_msg));
					printf("Process Pid %d Client ip %s port %d , Connection Successly.\n", getpid(), client_ip, ntohs(ClientAddr.sin_port)); 	// 显示进程id以及客户端连接信息
					sprintf(Response_msg, "Hi, %s , welcome to Test Server [%s] Version 0.0.1~\n", client_ip, Server_ip);
					send(listen_array[i].fd, Response_msg, strlen(Response_msg), 0); 	// 向客户端发送响应数据
					printf("Send Response_msg to Client:%s, ClientSock:%d\n", client_ip, listen_array[i].fd);

					// socket就绪事件处理完毕，将返回的事件值清零，方便下次监听
					listen_array[0].revents = 0;
				}
				else // 目前不清楚是哪个客户端
				{
					// 获取就绪的 Client_fd
					for (i = 1; i < MAX; i++)
					{
						if (listen_array[i].fd != -1)
						{
							if (listen_array[i].revents == POLLIN)
							{
								// 处理Client_fd就绪
								if ((recvlen = recv(listen_array[i].fd, Request_msg, sizeof(Request_msg), 0)) > 0) // 接收当前客户端数据
								{
									// 客户端发送 localtime 请求本地时间
									if (strcmp(Request_msg, "localtime\n") == 0)
									{
										bzero(time_buffer, sizeof(time_buffer));
										tp = time(NULL);
										ctime_r(&tp, time_buffer); 	// 计算时间
										send(listen_array[i].fd, time_buffer, strlen(time_buffer), 0); 	// 发送给客户端
										printf("Response [unknow] local time\n");
									}
									else // 没有请求时间就将客户端数据转发给其他所有已经连接的客户端（群发）
									{
										sprintf(Forward, "[unknow] 对你说：%s\n\n", Request_msg);
										printf("%s", Forward);
										for (int j = 1; j < MAX; j++)
										{
											if (listen_array[j].fd != -1)
											{
												send(listen_array[j].fd, Forward, strlen(Forward), 0);
											}
										}
									}
									// 每次接收完数据清零等待下次接收
									bzero(Request_msg, sizeof(Request_msg)); 				
									bzero(Forward, sizeof(Forward));
									// 处理就绪完毕，返回事件清零，方便下次监听
									listen_array[i].revents = 0;
								}
								else if (recvlen == -1) 	// 接收数据失败
								{
									perror("Server Version 0.0.1, Recv Failed.\n");
									close(listen_array[i].fd);
								}
								else if (recvlen == 0) 		// 客户端退出
								{	
									printf("[unknow] Exit, Close.\n");
									// 两个删除
									close(listen_array[i].fd); 			// 关闭客户端套接字
									listen_array[i].fd = -1; 			// 将Client_fd从监听集合中删除
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
			perror("Poll Call Failed!\n");
			exit(0);
		}
	}
	close(Server_fd); 	// 5.关闭服务器端套接字

	return 0;
}

#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <MysocketAPI.h>
#include <time.h>
#include <string.h>
#include <sys/epoll.h>

// Epoll单进程一对多服务器

/* 
question:
不用将Client_fd从就绪集合中删除吗？
answer:
Epoll会自动更新并管理队列，epoll_wait每次会返回所有的就绪事件，而不仅仅是
一个事件，因此在处理完所有就绪事件后，无需手动删除就绪队列中对应的节点。
 */
//#define SERVER_IP 		"192.168.221.170"
#define SERVER_PORT 	54321
#define shutdown 		1
#define MAX 			4096 	// 监听树，监听序列，就绪序列的可监听socket最大数

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
	printf("Epoll Server Version 0.0.1 Accept Start..\n");

	struct epoll_event Node; 				// 监听节点类型（服务端socket）
	struct epoll_event ready_array[MAX]; 	// 就绪序列
	int epfd; 								// 监听树（红黑树）描述符
	// 初始化服务端监听节点
	Node.data.fd = Server_fd;
	Node.events = EPOLLIN;
	epfd = epoll_create(MAX); 				// 创建监听树，节点数为MAX
	epoll_ctl(epfd, EPOLL_CTL_ADD, Server_fd, &Node); 	// 将服务端socket添加进监听树
	int readycode = 0; 			// epoll_wait返回值，就绪sockfd数量

	int recvlen; 				// 接收数据的大小
	time_t tp; 					// 时间变量
	char time_buffer[1024]; 	// 时间字符串数据
	char Request_msg[1024]; 	// 客户端请求数据
	char Forward[2048]; 		// 转发数据
	bzero(Request_msg, sizeof(Request_msg));
	bzero(Forward, sizeof(Forward));

	// Epoll开始监听
	while (shutdown)
	{
		readycode = epoll_wait(epfd, ready_array, MAX, -1);
		// 循环处理就绪的 sockfd
		if (readycode > 0)
		{
			int i = 0;
			while (i < readycode)
			{
				if (ready_array[i].data.fd == Server_fd)
				{
					if (ready_array[i].events = EPOLLIN)
					{
						// 处理Server_fd就绪

						Addrlen = sizeof(ClientAddr);
						Client_fd = ACCEPT(Server_fd, (struct sockaddr*)&ClientAddr, &Addrlen); 	// 4.阻塞等待客户端连接

						// 连接成功，存储客户端信息	
						// 将客户端socket存入监听树
						Node.data.fd = Client_fd;
						Node.events = EPOLLIN;
						epoll_ctl(epfd, EPOLL_CTL_ADD, Client_fd, &Node);

						// 向客户反馈连接成功信息
						inet_ntop(AF_INET, &ClientAddr.sin_addr.s_addr, client_ip, sizeof(client_ip));
						bzero(Response_msg, sizeof(Response_msg));
						printf("Process Pid %d Client ip %s port %d , Connection Successly.\n", getpid(), client_ip, ntohs(ClientAddr.sin_port)); 	// 显示进程id以及客户端连接信息
						sprintf(Response_msg, "Hi, %s , welcome to Test Server [%s] Version 0.0.1~\n", client_ip, Server_ip);
						send(Client_fd, Response_msg, strlen(Response_msg), 0); 	// 向客户端发送响应数据
						printf("Send Response_msg to Client:%s, ClientSock:%d\n", client_ip, Client_fd);
					}
				}
				else // 目前不清楚是哪个客户端
				{
					// 获取就绪的 Client_fd
					if (ready_array[i].events == EPOLLIN)
					{
						// 处理Client_fd就绪
						if ((recvlen = recv(ready_array[i].data.fd, Request_msg, sizeof(Request_msg), 0)) > 0) // 接收当前客户端数据
						{
							// 客户端发送 localtime 请求本地时间
							if (strcmp(Request_msg, "localtime\n") == 0)
							{
								bzero(time_buffer, sizeof(time_buffer));
								tp = time(NULL);
								ctime_r(&tp, time_buffer); 	// 计算时间
								send(ready_array[i].data.fd, time_buffer, strlen(time_buffer), 0); 	// 发送给客户端
								printf("Response [unknow] local time\n");
							}
							else // 没有请求时间就将客户端数据转发给其他所有已经连接的客户端（群发）
							{
								sprintf(Forward, "[unknow] 对你说：%s\n\n", Request_msg);
								printf("%s", Forward);

								for (int j = 0; j < MAX; j++)
								{
									if (ready_array[j].data.fd != Server_fd)
									{
										send(ready_array[j].data.fd, Forward, strlen(Forward), 0);
									}
								}

							}
							// 每次接收完数据清零等待下次接收
							bzero(Request_msg, sizeof(Request_msg)); 				
							bzero(Forward, sizeof(Forward));
						}
						else if (recvlen == -1) 	// 接收数据失败
						{
							perror("Server Version 0.0.1, Recv Failed.\n");
							close(ready_array[i].data.fd);
						}
						else if (recvlen == 0) 		// 客户端退出
						{	
							printf("[unknow] Exit, Close.\n");

							close(ready_array[i].data.fd); 									// 关闭客户端套接字
							epoll_ctl(epfd, EPOLL_CTL_DEL, ready_array[i].data.fd, NULL); 	// 将Client_fd从监听树中删除
						}
						break;
					}
				}
				i++;
			}

		}
		else if (readycode == -1)
		{
			perror("Epoll Call Failed!\n");
			exit(0);
		}
	}
	close(Server_fd); 	// 5.关闭服务器端套接字

	return 0;
}

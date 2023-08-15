#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include <MysocketAPI.h>
#include <time.h>
#include <pthread.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>

// 多线程结构服务器

int socket_list[1024]; 		// 连接的客户端socket数组
pthread_mutex_t list_lock; 	// 客户端socket数组锁

//#define SERVER_IP 		"192.168.221.170"
#define SERVER_PORT 	54321
#define shutdown 		1

typedef struct
{
	int client_fd; 		// 客户端套接字
	char client_ip[16]; 	// 客户端 IP
}Thread_arg; 	// 线程需要传入的参数

// 线程工作（一对一处理客户端业务）
// 加锁，转发功能
void* Thread_job(void* arg)
{
	Thread_arg targ = *(Thread_arg*)arg;
	pthread_detach(pthread_self()); 	// 修改为分离态线程，线程执行完毕自动回收

	// 每个线程负责的变量
	int recvlen; 				// 接收数据的大小
	time_t tp; 					// 时间变量
	char time_buffer[1024]; 	// 时间字符串数据
	char Request_msg[1024]; 	// 客户端请求数据
	char Forward[2048]; 		// 转发数据
	bzero(Request_msg, sizeof(Request_msg));
	bzero(Forward, sizeof(Forward));

	// 当前一个线程对应一个客户端，可以阻塞读取数据
	while ((recvlen = recv(targ.client_fd, Request_msg, sizeof(Request_msg), 0)) > 0) // 循环接收当前对应客户端数据
	{
		// 客户端发送 localtime 请求本地时间
		if (strcmp(Request_msg, "localtime\n") == 0)
		{
			bzero(time_buffer, sizeof(time_buffer));
			tp = time(NULL);
			ctime_r(&tp, time_buffer); 	// 计算时间
			send(targ.client_fd, time_buffer, strlen(time_buffer), 0); 	// 发送给客户端
			printf("Response (%s) local time\n", targ.client_ip);
		}
		else // 没有请求时间就将客户端数据转发给其他所有已经连接的客户端（群发）
		{
			sprintf(Forward, "[%s] 对你说：%s\n\n", targ.client_ip, Request_msg);
			printf("%s", Forward);
			pthread_mutex_lock(&list_lock);
			for (int i = 0; i < 1024; i++)
			{
				if (socket_list[i] != -1)
				{
					send(socket_list[i], Forward, strlen(Forward), 0);
				}
			}
			pthread_mutex_unlock(&list_lock);
		}
		// 每次接收完数据清零等待下次接收
		bzero(Request_msg, sizeof(Request_msg)); 				
	}
	if (recvlen == -1) 	// 接收数据失败
	{
		perror("Server Version 0.0.1, Recv Failed.\n");
		close(targ.client_fd);
		pthread_exit(NULL);
	}
	if (recvlen == 0) 	// 客户端退出
	{	
		printf("%s Thread tid 0x%x Exit, Close.\n", targ.client_ip, (unsigned int)pthread_self());
		for (int i = 0; i < 1024; i++)
		{
			pthread_mutex_lock(&list_lock);
			if (socket_list[i] == targ.client_fd)
			{
				socket_list[i] = -1;
				pthread_mutex_unlock(&list_lock);
				break;
			}
			pthread_mutex_unlock(&list_lock);
		}
		
		close(targ.client_fd);
		pthread_exit(NULL); 	// 客户端退出，线程退出
	}

	pthread_exit(NULL);
}

int main(void)
{
	for (int i = 0 ; i < 1024; i++)			// 初始化客户端socket数组
	{
		socket_list[i] = -1;
	}
	pthread_mutex_init(&list_lock, NULL); 	// 初始化互斥锁
	
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
	
	int Server_fd;
	Server_fd = SOCKET(AF_INET, SOCK_STREAM, 0); 						// 1.创建套接字
	BIND(Server_fd, (struct sockaddr*)&ServerAddr, sizeof(ServerAddr)); // 2.绑定网络信息
	LISTEN(Server_fd, 128); 											// 3.对 socket 链接事件进行监听

	pthread_t tid;
	int err;
	Thread_arg targ;
	while (shutdown)
	{
		printf("Server Version 0.0.1 Accept Start..\n");
		Addrlen = sizeof(ClientAddr);
		targ.client_fd = ACCEPT(Server_fd, (struct sockaddr*)&ClientAddr, &Addrlen); 	// 4.阻塞等待客户端连接

		// 连接成功，获取客户端信息	
		bzero(targ.client_ip, sizeof(targ.client_ip));
		inet_ntop(AF_INET, &ClientAddr.sin_addr.s_addr, targ.client_ip, sizeof(targ.client_ip));
		
		// 存储客户端套接字
		pthread_mutex_lock(&list_lock);
		for (int i = 0; i < 1024; i++)
		{
			if (socket_list[i] == -1)
			{
				socket_list[i] = targ.client_fd;
				break;
			}
		}
		pthread_mutex_unlock(&list_lock);
		
		// 创建普通线程
		if ((err = pthread_create(&tid, NULL, Thread_job, (void*)&targ)) > 0)
		{
			printf("Master 0x%x Create Thread Failed! error: %s\n", (unsigned int)pthread_self(), strerror(err));
			exit(0);
		}

		// 父进程负责向客户反馈连接成功信息
		bzero(Response_msg, sizeof(Response_msg));
		printf("Master 0x%x Client ip %s port %d , Connection Successly.\n", (unsigned int)pthread_self(), targ.client_ip, ntohs(ClientAddr.sin_port)); 	// 显示父进程id以及客户端连接信息
		sprintf(Response_msg, "Hi, %s , welcome to Test Server [%s] Version 0.0.1~\n", targ.client_ip, Server_ip);
		send(targ.client_fd, Response_msg, strlen(Response_msg), 0); 	// 向客户端发送响应数据
		printf("Send Response_msg to Client:%s, ClientSock:%d\n", targ.client_ip, targ.client_fd);	
	}
	close(Server_fd); 	// 5.关闭服务器端套接字
	pthread_mutex_destroy(&list_lock); // 回收互斥锁

	return 0;
}

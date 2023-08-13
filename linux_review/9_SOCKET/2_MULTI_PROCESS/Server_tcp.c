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

// 多进程模型
/*
增加一个线程去回收子进程，捕捉SIGCHLD信号后调用wait函数
*/

#define SERVER_IP 		"192.168.221.161"
#define SERVER_PORT 	54321
#define shutdown 		1

// TODO:捕捉函数（SIGCHLD）
void Sig_do(int signum)
{ 
	// 捕捉到一个信号就尽可能多次回收，将当前可回收的进程全部回收（信号不能排队）
	int zpid;
	while ((zpid = waitpid(-1, NULL, WNOHANG)) > 0)
	{
		printf("Process Server Version 0.0.1 Wait Thread 0x%x , zombie pid %d\n", (unsigned int)pthread_self(), zpid);
	}
}

// TODO:线程工作（回收僵尸进程）
void* Thread_job(void* arg)
{
	pthread_detach(pthread_self()); 	// 修改为分离态线程，线程执行完毕自动回收
	
	// 捕捉设定
	struct sigaction act, old_act; 		// 信号行为结构体
	act.sa_handler = Sig_do; 			// 行为选择接口（捕捉函数地址）
	act.sa_flags = 0;
	sigemptyset(&act.sa_mask); 			// 临时屏蔽字，不用时初始化为 0
	sigaction(SIGCHLD, &act, &old_act); // 使用act修改信号 SIGCHLD 行为，传出此信号原有行为到 old_act 中
	// 解除对 SIGCHLD 的屏蔽
	sigprocmask(SIG_SETMASK, &act.sa_mask, NULL);

	// 等待信号
	while (1)
		sleep(1);

	pthread_exit(NULL);
}

int main(void)
{
	char Response_msg[1024]; 	// 服务端回应数据
	char client_ip[16]; 		// 客户端 IP
	socklen_t Addrlen; 			// 网络信息结构体大小
	pid_t pid; 					// 子进程

	struct sockaddr_in ServerAddr, ClientAddr; 	// 自定义服务端网络地址信息
	bzero(&ServerAddr, sizeof(ServerAddr));
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(SERVER_PORT);
	ServerAddr.sin_addr.s_addr = INADDR_ANY; 	// 本机任意IP

	int Server_fd, Client_fd;
	Server_fd = SOCKET(AF_INET, SOCK_STREAM, 0); 						// 1.创建套接字
	BIND(Server_fd, (struct sockaddr*)&ServerAddr, sizeof(ServerAddr)); // 2.绑定网络信息
	LISTEN(Server_fd, 128); 											// 3.对 socket 链接事件进行监听
	
	// TODO:自定义信号屏蔽字，屏蔽SIGCHLD，防止普通线程刚一创建还未捕捉而被忽略（SIGCHLD是为数不多默认行为为忽略的信号）。
	sigset_t set, old_set; 		// 定义信号集
	sigemptyset(&set); 			// 全部位码置 0
	sigaddset(&set, SIGCHLD); 	// 将信号SIGCHLD对应位码置 1
	sigprocmask(SIG_SETMASK, &set, &old_set); 	// 替换信号屏蔽字，SIG_SETMASK直接覆盖
	
	// TODO:创建普通线程回收僵尸进程（父进程通过信号被动回收）
	pthread_t tid;
	int err;
	if ((err = pthread_create(&tid, NULL, Thread_job, NULL)) > 0)
	{
		printf("Master 0x%x Create Thread Failed! error: %s\n", (unsigned int)pthread_self(), strerror(err));
		exit(0);
	}
	 									
	while (shutdown)
	{
		printf("Server Version 0.0.1 Accept Start..\n");
		Addrlen = sizeof(ClientAddr);
		Client_fd = ACCEPT(Server_fd, (struct sockaddr*)&ClientAddr, &Addrlen); 	// 4.阻塞等待客户端连接
		
		// 连接成功，获取客户端信息	
		bzero(client_ip, sizeof(client_ip));
		inet_ntop(AF_INET, &ClientAddr.sin_addr.s_addr, client_ip, sizeof(client_ip));

		// 父进程accept成功，创建子进程
		pid = fork();

		if (pid > 0)
		{
			// 父进程负责向客户反馈连接成功信息
			bzero(Response_msg, sizeof(Response_msg));
			printf("Ppid %d Client ip %s port %d , Connection Successly.\n", getpid(), client_ip, ntohs(ClientAddr.sin_port)); 	// 显示父进程id以及客户端连接信息
			sprintf(Response_msg, "Hi, %s , welcome to Test Server [%s] Version 0.0.1~\n", client_ip, SERVER_IP);
			send(Client_fd, Response_msg, strlen(Response_msg), 0); 	// 向客户端发送响应数据
			printf("Send Response_msg to Client:%s\n", client_ip);	
		}
		else if (pid == 0) 	// 子进程工作
		{
			// 子进程负责的变量
			int recvlen; 				// 接收数据的大小
			time_t tp; 					// 时间变量
			char time_buffer[1024]; 	// 时间字符串数据
			char Request_msg[1024]; 	// 客户端请求数据
			bzero(Request_msg, sizeof(Request_msg));
			
			// 读取一次客户端请求数据，读取完毕关闭连接
			// 当前一个进程对应一个客户端，可以阻塞读取数据
			while ((recvlen = recv(Client_fd, Request_msg, sizeof(Request_msg), 0)) > 0) // 循环接收当前对应客户端数据
			{
				// 客户端发送 localtime 请求本地时间
				if (strcmp(Request_msg, "localtime\n") == 0)
				{
					bzero(time_buffer, sizeof(time_buffer));
					tp = time(NULL);
					ctime_r(&tp, time_buffer); 	// 计算时间
					send(Client_fd, time_buffer, strlen(time_buffer), 0); 	// 发送给客户端
					printf("Response (%s) local time\n", client_ip);
				}
				else // 没有请求时间就打印客户端数据
				{
					printf("[%s] 对你说：%s\n", client_ip, Request_msg);
				}
				// 每次接收完数据清零等待下次接收
				bzero(Request_msg, sizeof(Request_msg)); 				
			}
			if (recvlen == -1) 	// 接收数据失败
			{
				perror("Server Version 0.0.1, Recv Failed.\n");
				exit(0);
			}
			if (recvlen == 0) 	// 客户端退出
			{	
				printf("%s Cpid %d Exit, Close.\n", client_ip, getpid());
				exit(0); 	// 客户端退出，进程也退出
			}
			close(Client_fd); 				// 关闭连接
		}
	}
	close(Server_fd); 	// 5.关闭服务器端套接字
	
	return 0;
}

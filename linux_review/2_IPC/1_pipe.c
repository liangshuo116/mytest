#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define msg "hello, pipe success!"
int main()
{
	int fds[2]; // 存储文件描述符（对管道缓冲区进行读写）
	pipe(fds); 	// 创建管道
	
	pid_t pid;
	pid = fork();
	if (pid > 0)
	{	
		close(fds[0]); // 父进程关闭用于读的文件描述符
		write(fds[1], msg, strlen(msg)); // 写操作
		printf("Parent %d send msg  success.\n", getpid());
		close(fds[1]); // 关闭用于写的文件描述符
		
		wait(NULL); // 阻塞回收
	}
	else if (pid == 0)
	{
		close(fds[1]); // 子进程关闭用于写的文件描述符
		char buffer[1024]; // 接缓冲区数据
		bzero(buffer, sizeof(buffer)); // 初始化为 0
		read(fds[0], buffer, sizeof(buffer)); // 读操作
		printf("Child %d resd msg: %s\n", getpid(), buffer);
		close(fds[0]); // 关闭用于读的文件描述符

		exit(0); // 子进程退出
	}
	else
	{
		perror("Fork Call Failed.\n");
		exit(0);
	}

	return 0;
}

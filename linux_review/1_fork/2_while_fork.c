#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// 多进程模型
int main()
{
	pid_t pid;
	int i;
	// 循环创建进程
	for (i = 0; i < 3; i++)
	{
		pid = fork();
		// 遇到子进程就跳出，防止创建多余的子进程
		if (pid == 0) break;
	}

	if (pid > 0)
	{
		printf("Parent pid %d Create Success!\n", getpid()); 	// 获取进程 pid
		while (1)
			sleep(1);
	}
	else if (pid == 0)
	{
		if (i == 0)
		{
			printf("Child pid %d ppid %d i (%d)...游泳\n", getpid(), getppid(), i); // 获取自己的pid 父进程 ppid
			while (1)
				sleep(1);
		}
		else if (i == 1)
		{
			printf("Child pid %d ppid %d i (%d)...跑步\n", getpid(), getppid(), i); // 获取自己的pid 父进程 ppid
			while (1)
				sleep(1);
		}
		else if (i == 2)
		{
			printf("Child pid %d ppid %d i (%d)...击剑\n", getpid(), getppid(), i); // 获取自己的pid 父进程 ppid
			while (1)
				sleep(1);
		}
	}
	else
	{
		perror("Fork Call Failed.\n");
		exit(0);
	}

	return 0;
}

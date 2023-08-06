#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// 进程重载
int main()
{
	pid_t pid;
	pid = fork();

	if (pid > 0)
	{
		printf("Parent pid %d Running...\n", getpid());
		while (1)
			sleep(1);
	}
	else if (pid == 0)
	{
		printf("Child pid %d Running...\n", getpid());
		sleep(5);
		// 重载 功能为shell命令 firefox www.baidu.com
		execlp("firefox", "firefox", "www.baidu.com", NULL);
	}
	else
	{
		perror("Fork Call Failed.\n");
		exit(0);
	}

	return 0;
}

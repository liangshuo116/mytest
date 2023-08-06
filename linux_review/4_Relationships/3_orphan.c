#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
	pid_t pid;
	pid = fork();

	if (pid > 0)
	{
		printf("Parent pid %d Exit...\n", getpid());
		// 引发段错误使父进程崩溃，子进程变为孤儿进程
		char* str = "abc";
		str[0] = 'x';
		while (1)
			sleep(1);
	}
	else if (pid == 0)
	{
		while (1)
		{
			printf("Child pid %d running...\n", getpid());
			sleep(1);
		}
	}
	else
	{
		perror("Fork Call Failed...\n");
		exit(0);
	}
	return 0;
}

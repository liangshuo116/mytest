#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
	pid_t pid;
	int i;
	// 创建3个子进程
	for (i = 0; i < 3; ++i)
	{
		pid = fork();
		if (pid == 0) break;
	}

	if (pid > 0)
	{
		printf("Parent pid %d , pgid %d\n", getpid(), getpgrp());
		while (1)
			sleep(1);
	}
	else if (pid == 0)
	{
		if (i == 2) 	// 最后一个子进程创建新组
		{
			printf("Child pid %d , pgid %d\n", getpid(), getpgrp());
			sleep(5);
			setpgid(getpid(), getpid());
			printf("Child pid %d, Create new group, pgid %d\n", getpid(), getpgrp());
			while (1)
				sleep(1);
		}
		// 其他子进程
		printf("Child pid %d , pgid %d\n", getpid(), getpgrp());
		while (1)
			sleep(1);
	}
	else
	{
		perror("Fork Call Failed.\n");
		exit(0);
	}
	return 0;
}

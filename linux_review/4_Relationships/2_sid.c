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
		printf("Parent pid %d , pgid %d, sid %d\n", getpid(), getpgrp(), getsid(getpid()));
		while (1)
			sleep(1);
	}
	else if (pid == 0)
	{
		printf("Child pid %d , pgid %d, sid %d\n", getpid(), getpgrp(), getsid(getpid()));
		if (i == 2) 	// 最后一个子进程脱离终端，创建新会话
		{
			setsid(); 	// 立即为调用进程创建新组并成立新会话（脱离终端）
			printf("Child pid %d, new pgid %d, new sid %d\n", getpid(), getpgrp(), getsid(getpid()));
		}
		// 其他子进程
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

// 对子进程进行退出校验
int main()
{
	pid_t pid;
	int i;
	for (i = 0; i < 2; ++i)
	{
		pid = fork();
		if (pid == 0)
			break;
	}

	if (pid > 0)
	{
		printf("Parent Running...\n");

		pid_t zpid;  	// 僵尸进程 pid
		int status; 	// 退出信息
		while ((zpid = waitpid(-1, &status, WNOHANG)) != -1)
		{
			if (zpid > 0) 	// 子进程结束，回收成功
			{
				if (WIFEXITED(status))
				{
					printf("Parent %d：回收成功，子进程为正常退出，退出码/返回值：%d\n", getpid(), WEXITSTATUS(status));
				}
				if (WIFSIGNALED(status))
				{
					printf("Parent %d：回收成功，子进程为异常退出，杀死子进程的信号编号为：%d\n", getpid(), WTERMSIG(status));
				}
			}
			else if (zpid == 0)
			{
				// 无工作
			}
		}
	}
	else if (pid == 0)
	{
		if (i == 0) 	// 子进程 0 直接退出
		{
			sleep(5);
			printf("Child i %d pid %d Exit...\n", i, getpid());
			exit(0);
		}
		else if (i == 1) 	// 子进程 1 一直循环
		{
			while (1) 	// 用 kill -11 pid 来杀死这个子进程
			{
				printf("Child i %d pid %d While...\n", i, getpid());
				sleep(1);
			}
		}
	}
	else
	{
		perror("Fork Call Failed...\n");
		exit(0);
	}

	return 0;
}

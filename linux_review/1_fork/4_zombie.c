#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

// 回收僵尸进程
int main()
{
	pid_t pid;
	pid = fork();

	if (pid > 0)
	{
		printf("Parent Running...\n");
		pid_t zpid;
		//zpid = wait(NULL); 	// 阻塞回收函数，子进程不结束，父进程就会卡在这等待子进程
		while ((zpid = waitpid(pid, NULL, WNOHANG)) != -1) // 非阻塞回收函数，循环判断子进程是否结束
		{
			if (zpid > 0) // 回收成功，打印僵尸进程 pid
			{
				printf("Process Wait Success, zombie pid %d\n", zpid);
			}
			else if (zpid == 0) // 子进程未结束，父进程继续自己的工作
			{
				printf("Parent Running...\n");
				sleep(1);
			}
		}

		while (1)
			sleep(1);
	}
	else if (pid == 0)
	{
		printf("Child pid %d Running...\n", getpid());
		sleep(5); 	// 睡眠5秒，方便观察
		exit(0); 	// 子进程先于父进程退出，产生僵尸进程
	}
	else
	{
		perror("Fork Call Failed...\n");
		exit(0);
	}

	return 0;
}

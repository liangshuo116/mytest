#include <PROCESS_COPY.h>

int PROCESS_WAIT()
{
	int status; 	// 退出信息
	pid_t zpid; 	// 僵尸进程
	while ((zpid = waitpid(-1, &status, WNOHANG)) != -1)
	{
		if (zpid > 0) 	// 回收成功
		{
			if (WIFEXITED(status)) // 正常退出
			{
				printf("Parent %d Wait Zombie Pid %d Successly. 正常退出，退出码：%d\n", getpid(), zpid, WEXITSTATUS(status));
			}
			if (WIFSIGNALED(status)) // 异常退出
			{
				printf("Parent %d Wait Zombie Pid %d Successly. 异常退出，杀死进程的信号：%d\n", getpid(), zpid, WTERMSIG(status));
			}
		}
	}
	return 0;
}
				

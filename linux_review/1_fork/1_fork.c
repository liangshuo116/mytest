#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// 创建子进程模板
int main()
{
	pid_t pid;
	pid = fork(); 	// 创建子进程
	
	if (pid > 0) 	// 调用fork，在父进程返回子进程 pid
	{
		printf("Parent Running...\n"); 	// Parent code
	}
	else if (pid == 0) 	// 在子进程中返回 0
	{
		printf("Child Running...\n"); 	// Child code
		exit(0); 	//子进程工作完毕就退出，不能让子进程执行工作区之外的代码
		// while (1) sleep(1); 或者不让子进程出来
	}
	else
	{
		// 错误处理（标准出错，默认使用标准输出，打印到屏幕上）
		perror("Fork Call Failed.\n"); 	
		exit(0);
	}

	printf("Running...\n");
	while (1)
	{
		sleep(1);
	}
	return 0;
}

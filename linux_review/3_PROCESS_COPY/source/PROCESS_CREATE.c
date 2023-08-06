#include <PROCESS_COPY.h>

int PROCESS_CREATE(const char* Sfile, const char* Dfile, int block_size, int prono)
{
	// 父进程按进程数量创建子进程
	pid_t pid;
	int i;
	for (i = 0; i < prono; i++)
	{
		pid = fork();
		// 如果是子进程，就退出，防止创建多余子进程
		if (pid == 0) break;
	}

	// 父子进程工作
	if (pid > 0)
	{
		printf("Parent %d Waitting..\n", getpid());
		// 父进程等待回收子进程
		PROCESS_WAIT();
	}
	else if (pid == 0)
	{
		// 整型参数转为字符串
		char STR_POS[100]; 			// 块索引
		char STR_BLOCKSIZE[100]; 	// 块大小
		bzero(STR_POS, sizeof(STR_POS));
		bzero(STR_BLOCKSIZE, sizeof(STR_BLOCKSIZE));
		int POS;
		POS = i * block_size;
		sprintf(STR_POS, "%d", POS);
		sprintf(STR_BLOCKSIZE, "%d", block_size);

		printf("Child Process %d POS[%d] BLOCKSIZE[%d]\n", getpid(), POS, block_size);
		// 子进程重载，向 COPY 功能程序传参
		execlp("/home/sure/colin_data/linux_review/PROCESS_COPY/moudle/COPY", "COPY", Sfile, Dfile, STR_POS, STR_BLOCKSIZE, NULL);
	}
	else
	{
		perror("Fork Call Failed!\n");
		exit(0);
	}

	return 0;
}

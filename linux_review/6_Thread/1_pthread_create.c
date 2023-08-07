#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

// 创建线程

void* Thread_job(void* arg)
{
	// 普通线程的代码区
	int i = *(int*)arg;
	// 打印线程 tid（十六进制）更像是地址，好看，十进制的数太大且是无符号整型，可能是负数。
	printf("Thread Running tid = 0x%x , i = %d...\n", (unsigned int)pthread_self(), i);
	while (1)
		sleep(1);
}

int main()
{
	// main 函数是主线程的代码区
	pthread_t tid[2];
	int i;
	for (i = 0; i < 2; i++)
	{
		pthread_create(&tid[i], NULL, Thread_job, (void*)&i);
		// 地址传递，若无间隔，可能i计算太快，传给线程的i可能相同
		usleep(10000);
	}

	printf("Master Thread tid = 0x%x Create Success...\n", (unsigned int)pthread_self());
	while (1)
		sleep(1);
}

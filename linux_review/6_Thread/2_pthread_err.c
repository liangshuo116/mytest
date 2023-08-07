#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

// 线程的错误处理（创建线程数大于最大数量）

void* Thread_job(void* arg)
{
	// 无工作，睡眠
	while (1)
		sleep(1);
}

int main()
{
	pthread_t tid;
	int err;
	int flag = 0;
	// 循环创建线程，直到超过最大线程数，报错
	while (1)
	{
		if ((err = pthread_create(&tid, NULL, Thread_job, NULL)) > 0)
		{
			// 线程的错误处理
			printf("Thread Create Failed...error: %s\n", strerror(err));
			exit(0);
		}
		printf("Thread number %d\n", ++flag);
		flag++;
	}
	return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

// 回收线程

// 线程工作
void* Thread_job(void* arg)
{
	// 循环打印三次
	int flag = 3;
	while (flag--)
	{
		printf("Thread tid 0x%x Running...flag = %d\n", (unsigned int)pthread_self(), flag);
		sleep(1);
	}
	// 线程返回值
	return (void*)8;
}

int main()
{
	// 线程创建
	pthread_t tid;
	int err;
	if ((err = pthread_create(&tid, NULL, Thread_job, NULL)) > 0)
	{
		printf("Create Thread Failed! error: %s\n", strerror(err));
		exit(0);
	}
	printf("Master 0x%x Create Thread 0x%x Success.\n", (unsigned int)pthread_self(), (unsigned int)tid);
	
	// 线程回收（阻塞回收，等待线程执行完毕）
	void* retval;
	if ((err = pthread_join(tid, &retval)) > 0)
	{
		printf("Master 0x%x Join Thread Failed! error: %s\n", (unsigned int)pthread_self(), strerror(err));
		exit(0);
	}

	printf("Master 0x%x Join Thread Success.\n", (unsigned int)pthread_self());
	printf("Thread 0x%x retval: %ld\n", (unsigned int)tid, (long int)retval);

	return 0;
}

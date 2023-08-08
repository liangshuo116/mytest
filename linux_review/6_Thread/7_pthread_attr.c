#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

// 通过修改线程属性，直接创建储分离线程

void* Thread_job(void* arg)
{
	printf("Thread 0x%x Running...\n", (unsigned int)pthread_self());
	while (1)
		sleep(1);

	return NULL;
}

int main()
{
	// 定义线程属性
	pthread_attr_t Attr;
	pthread_attr_init(&Attr); // 初始化属性，默认为 JOINABLE 回收态，栈大小为 8M
	// 查看默认属性中的退出状态，并返回到 detachstate 变量中
	int detachstate;
	pthread_attr_getdetachstate(&Attr, &detachstate);
	if (detachstate == PTHREAD_CREATE_JOINABLE)
	{
		printf("Thread DFL Attr, ExitState is JOINABLE.\n");
	}
	else
	{
		printf("Thread DFL Attr, ExitState is DETACHED.\n");
	}
	// 设置属性中的退出状态
	pthread_attr_setdetachstate(&Attr, PTHREAD_CREATE_DETACHED);
	printf("Set the ExitState to DETACHED Success...\n");

	// 创建分离态线程
	pthread_t tid;
	int err;
	if ((err = pthread_create(&tid, &Attr, Thread_job, NULL)) > 0)
	{
		printf("Master 0x%x Create DETACHED Thread Failed!, error: %s\n", (unsigned int)pthread_self(), strerror(err));
		pthread_attr_destroy(&Attr);
		exit(0);
	}
	printf("Master 0x%x Create DETACHED Thread 0x%x Success!\n", (unsigned int)pthread_self(), (unsigned int)tid);

	// 尝试回收
	void* retval;
	if ((err = pthread_join(tid, &retval)) > 0)
	{
		printf("Master 0x%x Join Thread Failed!, error: %s\n", (unsigned int)pthread_self(), strerror(err));
	}

	// 回收线程属性
	pthread_attr_destroy(&Attr);

	return 0;
}

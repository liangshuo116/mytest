#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

// 分离态线程

void* Thread_job(void* arg)
{
	// 将自己设置为分离态
	pthread_detach(pthread_self());
	printf("Thread 0x%x DETACHED.\n", (unsigned int)pthread_self());

	while (1)
		sleep(1);
	
	pthread_exit(NULL);
}

int main()
{
	pthread_t tid;
	int err;
	if ((err = pthread_create(&tid, NULL, Thread_job, NULL)) > 0)
	{
		printf("Master 0x%x Create Thread Failed! err: %s\n", (unsigned int)pthread_self(), strerror(err));
		exit(0);
	}

	sleep(3);
	// 回收普通线程（回收已经为分离态的线程会出错）
	int* retval;
	// tid 指向的线程系统不允许其他线程进行回收，因此 tid 无效，错误为参数无效
	if ((err = pthread_join(tid, (void*)&retval)) > 0)
	{
		printf("Master 0x%x Join Thread 0x%x Failed! err: %s\n", (unsigned int)pthread_self(), (unsigned int)tid, strerror(err));
		exit(0);
	}
	printf("Master 0x%x Join Thread 0x%x Success... Retval = %ld\n", (unsigned int)pthread_self(), (unsigned int)tid, (long int)retval);
	
	return 0;
}

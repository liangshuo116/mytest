#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <string.h>

// 死锁（有bug）
int num1 = 0;
int num2 = 0;
pthread_mutex_t lock1;
pthread_mutex_t lock2;

// 线程工作
void* Thread_job(void* arg)
{
	pthread_detach(pthread_self()); 	// 设置为分离态线程
	printf("Thread 0x%x\n", (unsigned int)pthread_self());

	// 对 num1 上锁后请求 num2
	int i = *(int*)arg;
	if (i == 0) // 线程1
	{
		pthread_mutex_lock(&lock1); 	// 上锁
		++num1;
		printf("Thread 0x%x set lock to num1, sleep 10s\n", (unsigned int)pthread_self());
		sleep(10);
		++num2;
		printf("Thread 0x%x set lock to num2 Success.\n", (unsigned int)pthread_self());
		pthread_mutex_unlock(&lock1); 	// 解锁
	}
	else 		// 线程2
	{
		pthread_mutex_lock(&lock2); 	// 上锁
		++num2;
		printf("Thread 0x%x set lock to num2, sleep 10s\n", (unsigned int)pthread_self());
		sleep(10);
		++num1;
		printf("Thread 0x%x set lock to num1 Success.\n", (unsigned int)pthread_self());
		pthread_mutex_unlock(&lock2); 	// 解锁
	}
	pthread_exit(NULL); 				// 退出线程
}

int main()
{
	pthread_mutex_init(&lock1, NULL); // 初始化线程互斥锁
	pthread_mutex_init(&lock2, NULL); // 初始化线程互斥锁

	// 创建两个线程
	pthread_t tid[2];
	int err;
	for (int i = 0; i < 2; ++i)
	{
		if ((err = pthread_create(&tid[i], NULL, Thread_job, (void*)&i)) > 0)
		{
			printf("Master 0x%x Create Thread Failed! err: %s\n", (unsigned int)pthread_self(), strerror(err));
		}
		usleep(10000);
	}
	printf("Master 0x%x Create Thread Success...\n", (unsigned int)pthread_self());
	printf("Process id %d\n", getpid());
	sleep(30);
	
	pthread_mutex_destroy(&lock1); 	// 释放互斥锁
	pthread_mutex_destroy(&lock2); 	// 释放互斥锁

	return 0;
}

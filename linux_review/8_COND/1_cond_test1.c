#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <fcntl.h>
/*
	两个保安分别在白天和夜晚巡逻，保安A白天巡逻，夜晚挂起，保安B夜晚巡逻，白天挂起。
	利用条件变量结合互斥锁模拟完成操作。
*/
int info; 				// 共享变量，0 表示为夜晚，1 表示为白天
pthread_cond_t day; 	// 白天 条件变量，夜晚保安挂起在此条件变量中
pthread_cond_t night; 	// 夜晚 条件变量，白天保安挂起在此条件变量中
pthread_mutex_t lock; 	// 线程互斥锁

void* Thread_A(void* arg) 	// 白天巡逻，晚上休息
{
	pthread_detach(pthread_self());
	while (1)
	{
		pthread_mutex_lock(&lock); 		// 上锁
		// 如果info是晚上，休息，挂起到晚上条件变量中
		if (info == 0)
			pthread_cond_wait(&night, &lock);
			// wait 函数挂起时解锁，在唤醒时上锁，保证线程虽然被挂起，但是其他线程依然可以访问资源
		
		// 白天巡逻中
		printf("Thread 0x%x info = %d, 白天巡逻中...\n", (unsigned int)pthread_self(), info);
		info = 0; 						// 巡逻完毕，天黑了
		pthread_mutex_unlock(&lock); 	// 解锁
		
		pthread_cond_signal(&day); 		// 唤醒晚上保安，晚上保安在白天条件变量中
	}
	pthread_exit(NULL);
}
void* Thread_B(void* arg) 	// 夜晚巡逻，白天休息
{
	pthread_detach(pthread_self());
	while (1)
	{
		pthread_mutex_lock(&lock); 		// 上锁
		// 如果info是白天，休息，挂起到白天条件变量中
		if (info == 1)
			pthread_cond_wait(&day, &lock);
		
		// 夜晚巡逻中
		printf("Thread 0x%x info = %d, 夜晚巡逻中...\n", (unsigned int)pthread_self(), info);
		info = 1; 						// 巡逻完毕，天亮了
		pthread_mutex_unlock(&lock); 	// 解锁
		
		pthread_cond_signal(&night); 	// 唤醒白天保安，白天保安在夜晚条件变量中
	}
	pthread_exit(NULL);
}

int main()
{
	// 初始化条件变量
	// 初始化互斥锁
	if (pthread_cond_init(&day, NULL) != 0 || pthread_cond_init(&night, NULL) != 0 || pthread_mutex_init(&lock, NULL) != 0)
	{
		perror("Cond or Lock init Failed!\n");
		exit(0);
	}

	// 创建线程 A，B
	pthread_t tid[2];
	pthread_create(&tid[0], NULL, Thread_A, NULL);
	pthread_create(&tid[1], NULL, Thread_B, NULL);
	while (1)
		sleep(1);

	// 回收条件变量
	pthread_cond_destroy(&day);
	pthread_cond_destroy(&night);
	// 回收互斥锁
	pthread_mutex_destroy(&lock);

	return 0;
}

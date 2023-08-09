#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

// 测试两个线程访问全局变量，添加互斥锁以避免发生访问冲突
int number = 0; 	// 全局变量

pthread_mutex_t lock; 	// 定义锁变量
//pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER; // 静态初始化

void* Thread_job(void* arg)
{
	int flags = 5000;
	int temp; // 定义一个中间值，放大冲突
	pthread_detach(pthread_self());
	// 每个线程 ++number 5000次
	for (flags; flags > 0; flags--)
	{
		pthread_mutex_lock(&lock); 		// 上锁
		temp = number;
		printf("Thread 0x%x ++Number = %d\n", (unsigned int)pthread_self(), ++temp);
		number = temp;
		pthread_mutex_unlock(&lock); 	// 解锁
	}

	pthread_exit(NULL);
}

int main()
{
	pthread_t tid[2];
	pthread_mutex_init(&lock, NULL); // 互斥锁初始化（动态初始化）

	int i;
	// 创建两个线程
	for (i = 0; i < 2; ++i)
	{
		pthread_create(&tid[i], NULL, Thread_job, NULL);
	}

	while (1)
		sleep(1);

	pthread_mutex_destroy(&lock); 	// 使用完毕释放互斥锁

	return 0;
}

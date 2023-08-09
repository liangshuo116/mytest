#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

// 测试两个线程访问全局变量，发生访问冲突，导致结果异常
int number = 0; 	// 全局变量
// 最终结果应是10000，但是结果错误
void* Thread_job(void* arg)
{
	int flags = 5000;
	int temp; // 定义一个中间值，放大冲突
	pthread_detach(pthread_self());
	// 每个线程 ++number 5000次
	for (flags; flags > 0; flags--)
	{
		temp = number;
		printf("Thread 0x%x ++Number = %d\n", (unsigned int)pthread_self(), ++temp);
		number = temp;
	}

	pthread_exit(NULL);
}

int main()
{
	pthread_t tid[2];
	int i;
	// 创建两个线程
	for (i = 0; i < 2; ++i)
	{
		pthread_create(&tid[i], NULL, Thread_job, NULL);
	}

	while (1)
		sleep(1);

	return 0;
}

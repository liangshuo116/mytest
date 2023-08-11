#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

pthread_mutex_t lock; 		// 互斥锁
pthread_cond_t Not_Full; 	// 条件变量，生产者挂起（队列不满了，唤醒）
pthread_cond_t Not_Empty; 	// 条件变量，消费者挂起（队列不空了，唤醒）

// 任务块
typedef struct
{
	void* (*business)(void*); 	// 函数指针
	void* arg; 					// 函数参数
}Task_t;

// 任务队列（环形队列）
typedef struct
{
	Task_t* list; 				// 任务队列地址
	int FRONT; 					// 头部
	int REAR; 					// 尾部
	int CUR; 					// 元素数量
	int MAX; 					// 最大容量
}Container_t;

// 任务函数（任务缓存）
void* Test_job(void* arg)
{
	// 工作
	printf("Customer 0x%x 获取任务成功，执行完毕...\n", (unsigned int)pthread_self());
}

// 消费者（获取并执行任务）
void* Customer(void* arg)
{
	pthread_detach(pthread_self());
	// 可以重复获取任务，执行任务
	Task_t Gettask;
	Container_t* ct = NULL;
	ct = (Container_t*)arg;
	printf("Customer 0x%x Wait Business Success..\n", (unsigned int)pthread_self());
	while (1)
	{
	pthread_mutex_lock(&lock); 		// 上锁
	// 队列空，挂起并解锁资源（被唤醒并上锁）
	// 使用while判断，避免错误唤醒（被唤醒后没有判断是否是当前线程应该做的任务，导致多个线程干了同一件事）
	while (ct->CUR == 0)
		pthread_cond_wait(&Not_Empty, &lock);
	// 访问任务队列
	// 获取任务
	Gettask.business = ct->list[ct->REAR].business;
	Gettask.arg = ct->list[ct->REAR].arg;
	// 将该任务从队列移除
	ct->REAR = (ct->REAR + 1) % ct->MAX;
	--(ct->CUR);
	pthread_mutex_unlock(&lock); 	// 解锁
	// 队列不满，唤醒生产者
	pthread_cond_signal(&Not_Full);
	// 执行任务
	Gettask.business(Gettask.arg);
	}
	pthread_exit(NULL);
}

// 生产者的任务（将任务投递到队列）
void Producer(Container_t* ct, Task_t bs)
{
	pthread_mutex_lock(&lock); 		// 上锁
	// 队列满，挂起并解锁资源（被唤醒上锁）
	while (ct->CUR == ct->MAX)
		pthread_cond_wait(&Not_Full, &lock);
	// 访问任务队列
	// 将任务添加到队列
	ct->list[ct->FRONT].business = bs.business;
	ct->list[ct->FRONT].arg = bs.arg;
	// 队列元素数量增加
	++(ct->CUR);
	ct->FRONT = (ct->FRONT + 1) % ct->MAX;
	pthread_mutex_unlock(&lock); 	// 解锁
	// 队列不空，唤醒消费者
	pthread_cond_signal(&Not_Empty);

	printf("Producer 0x%x Add Business Success...\n", (unsigned int)pthread_self());
}

// 主线程作为唯一的生产者，创建普通线程作为消费者
int main()
{
	// 初始化锁，条件变量
	if (pthread_mutex_init(&lock, NULL) != 0 || pthread_cond_init(&Not_Empty, NULL) != 0 || pthread_cond_init(&Not_Full, NULL) != 0)
	{
		perror("Init Mutex or Cond Failed.\n");
		exit(0);
	}
	// 创建任务队列
	Container_t* ct = NULL;
	ct = (Container_t*)malloc(sizeof(Container_t));
	ct->list = (Task_t*)malloc(sizeof(Task_t) * 100);
	ct->FRONT = 0;
	ct->REAR = 0;
	ct->CUR = 0;
	ct->MAX = 100;
	// 创建线程（消费者）10个消费者，传参为 任务队列指针
	pthread_t tid[10];
	int err;
	for (int i = 0; i < 10; ++i)
	{
		if ((err = pthread_create(&tid[i], NULL, Customer, (void*)ct)) > 0)
		{
			printf("Producer 0x%x Create Customer Failed! err: %s\n", (unsigned int)pthread_self(), strerror(err));
		}
	}
	// 生产者线程向队列中添加任务（调用函数，全写在主函数太乱）
	Task_t settask;
	settask.business = Test_job;
	for (int i = 0; i < 8; ++i)
	{
		Producer(ct, settask);
	}
	sleep(8);
	for (int i = 0; i < 4; ++i)
	{
		Producer(ct, settask);
	}
	sleep(4);
	for (int i = 0; i < 2; ++i)
	{
		Producer(ct, settask);
	}
	sleep(2);

	while (1)
		sleep(1);

	return 0;
}

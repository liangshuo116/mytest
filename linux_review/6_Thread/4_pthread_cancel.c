#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

// 线程取消

// 线程工作
void* Thread_job(void* arg)
{
	while (1)
	{
		// 若没有调用 printf sleep 等函数发生系统调用，就不会检测 Cancel 事件，线程也就不会退出。
		// printf("Thread tid 0x%x Running...", (unsigned int)pthread_self());
		// sleep(1);

		// 解决方法：pthread_testcacel() 函数，调用它会发生一次系统调用，目的是检测处理 Cancel 事件，没有其他附加功能。
		//（使用时循环检测，因为不知道某线程发送 Cancel 事件的时间）
		pthread_testcancel();
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

	sleep(3);
	// 线程取消
	if ((err = pthread_cancel(tid)) > 0)
	{
		printf("Master 0x%x send Cancel to Thread 0x%x Failed! error: %s\n", (unsigned int)pthread_self(), (unsigned int)tid, strerror(err));
		exit(0);
	}
	printf("Master 0x%x send Cancel to Thread 0x%x Success...\n", (unsigned int)pthread_self(), (unsigned int)tid);

	// 线程回收（阻塞回收，等待线程执行完毕）
	void* retval;
	if ((err = pthread_join(tid, &retval)) > 0)
	{
		printf("Master 0x%x Join Thread Failed! error: %s\n", (unsigned int)pthread_self(), strerror(err));
		exit(0);
	}

	printf("Master 0x%x Join Thread Success.\n", (unsigned int)pthread_self());
	// 回收被取消的线程，其返回值为 -1
	// 因此编写线程工作时，不能用 -1 作为线程退出码，避免跟 cancel 混淆。
	printf("Thread 0x%x retval: %ld\n", (unsigned int)tid, (long int)retval);

	return 0;
}

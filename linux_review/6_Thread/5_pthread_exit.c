#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

// 线程退出

void* Thread_job(void* arg)
{
	printf("Thread Running...\n");
	pthread_exit((void*)16); 	// 退出线程
}

int main()
{
	pthread_t tid;
	void* retval;
	pthread_create(&tid, NULL, Thread_job, NULL);   // 创建线程
	pthread_join(tid, &retval); 					// 回收线程
	printf("Master Join Thread Success, return val = %ld\n", (long int)retval);

	return 0;
}

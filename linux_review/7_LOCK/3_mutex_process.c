#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>

// 进程互斥锁，两个进程访问共享资源，解决访问冲突

typedef struct
{
	pthread_mutex_t lock; 	// 共享锁
	int number; 		// 共享数据
}shared_t;

int main()
{
	shared_t* ptr = NULL; 	// 映射内存地址
	// 使用mmap映射，让两个进程共享数据
	int fd = open("PROCESS_LOCK", O_RDWR | O_CREAT, 0664);
	ftruncate(fd, sizeof(shared_t)); // 截断文件
	ptr = mmap(NULL, sizeof(shared_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	close(fd);

	ptr->number = 0;

	pthread_mutexattr_t attr; 					// 定义锁属性
	pthread_mutexattr_init(&attr); 				// 初始化锁属性（默认为线程互斥锁 PTHREAD_PROCESS_PRIVATE）
	pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED); // 修改锁属性为进程互斥锁 PTHREAD_PROCESS_SHARED
	pthread_mutex_init(&(ptr->lock), &attr); 	// 使用自定义属性初始化互斥锁
	
	pid_t pid; 			// 进程id
	// 循环创建两个子进程
	int i;
	for (i = 0; i < 2; ++i)
	{
		pid = fork();
		if (pid == 0) break;
	}

	if (pid > 0) 		// 父进程
	{
		printf("Parent %d Waitting...\n", getpid());
		// 回收子进程
		pid_t zpid;
		while ((zpid = waitpid(-1, NULL, WNOHANG)) != -1)
		{
			if (zpid > 0)
				printf("Parent %d Wait Zombie Pid %d\n", getpid(), zpid);
		}
		//wait(NULL);
	}
	else if (pid == 0) 	// 子进程
	{
		// 对全局变量 number 进行++操作，一个子进程执行 5000次
		int flags;
		for (flags = 5000; flags > 0; --flags)
		{
			pthread_mutex_lock(&(ptr->lock)); 		// 上锁
			printf("Child %d ++number = %d\n", getpid(), ++(ptr->number));
			pthread_mutex_unlock(&(ptr->lock)); 	// 解锁
		}
		exit(0);
	}
	else
	{
		perror("Parent Call Fork Failed!\n");
		exit(0);
	}

	munmap(ptr, sizeof(shared_t)); 				// 释放映射
	pthread_mutex_destroy(&(ptr->lock)); 		// 释放互斥锁
	pthread_mutexattr_destroy(&attr); 			// 释放互斥锁属性

	return 0;
}

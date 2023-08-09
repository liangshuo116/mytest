#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

// 文件读写锁
// 当前进程给文件进行写锁解锁操作，未解锁时其他线程不能对当前文件进行写锁操作

int main()
{
	int fd;
	fd = open("1_access_err.c", O_RDWR); 	// 打开文件
	struct flock oldlock;
	struct flock newlock;
	fcntl(fd, F_GETLK, &oldlock); 			// 查看文件默认的锁属性，查看是否被上锁
	//printf("%d\n", oldlock.l_type);
	//if (oldlock.l_type == F_WRLCK)
	//{
	//	printf("Process_1 %d 1_access_err.c DFL lock type = F_WRLCK\n", getpid());
	//}
	//else if (oldlock.l_type == F_RDLCK)
	//{
	//	printf("Process_1 %d 1_access_err.c DFL lock type = F_RDLCK\n", getpid());
	//}
	//else if (oldlock.l_type == F_UNLCK)
	{
		printf("Process_1 %d 1_access_err.c DFL lock type = F_UNLCK\n", getpid());
		// 如果是解锁状态
		// 设置文件锁属性
		newlock.l_type = F_WRLCK; 		// 锁类型为写锁
		newlock.l_whence = SEEK_SET; 	// 上锁位置：起始位置
		newlock.l_start = 0; 			// 相对位置：0，在 l_whence 的基础上向后偏移，一般不使用
		newlock.l_len = 0; 				// 锁长度：0 默认锁整个文件
		fcntl(fd, F_SETLKW, &newlock); 	// 上写锁
		printf("Process %d 1_access_err.c set Write Lock Success...\n", getpid());
		
		sleep(10); // 锁10s
	}
	fcntl(fd, F_SETLKW, &oldlock); 		// 解锁（阻塞解锁）
	printf("Process %d 1_access_err.c Unlock...\n", getpid());

	return 0;
}

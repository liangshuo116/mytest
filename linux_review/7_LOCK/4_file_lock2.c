#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

// 文件读写锁
// 当前进程给已上锁的文件进行写锁操作，不能上锁就阻塞等待

int main()
{
	int fd;
	fd = open("1_access_err.c", O_RDWR); 	// 打开文件
	struct flock newlock;
	
	// 设置文件锁属性
	newlock.l_type = F_WRLCK; 		// 锁类型为写锁
	newlock.l_whence = SEEK_SET; 	// 上锁位置：起始位置
	newlock.l_start = 0; 			// 相对位置：0，在 l_whence 的基础上向后偏移，一般不使用
	newlock.l_len = 0; 				// 锁长度：0 默认锁整个文件
	printf("init Lock finish.\n");
	fcntl(fd, F_SETLKW, &newlock); 	// 上写锁
	printf("Process %d 1_access_err.c set Write Lock Success...\n", getpid());
		
	// 解锁
	newlock.l_type = F_UNLCK;
	fcntl(fd, F_SETLKW, &newlock); 	//（阻塞解锁）
	printf("Process %d 1_access_err.c Unlock...\n", getpid());

	return 0;
}

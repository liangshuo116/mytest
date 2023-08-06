#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#define MSG "hello, fifo success!"
int main()
{
	// 写端
	// 打开管道文件
	int fd = open("FIFO", O_WRONLY);
	write(fd, MSG, strlen(MSG)); // 向管道缓冲区写数据
	printf("PWrite %d success...\n", getpid());
	close(fd); // 关闭文件描述符

	return 0;
}

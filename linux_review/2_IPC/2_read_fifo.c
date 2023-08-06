#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

int main()
{
	// 读端
	// 打开管道文件（只读）
	int fd = open("FIFO", O_RDONLY);
	char buffer[1024]; // 接缓冲区数据
	bzero(buffer, sizeof(buffer));
	read(fd, buffer, sizeof(buffer)); // 读取管道缓冲区数据
	printf("PRead %d read success, MSG: %s\n",getpid(), buffer);
	close(fd);

	return 0;
}

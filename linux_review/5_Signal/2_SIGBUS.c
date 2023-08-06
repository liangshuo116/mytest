#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

// 硬件异常产生信号，系统通过信号杀死违规进程
// 总线错误（核心已转储）
int main()
{
	int fd;
	int mapsize = 4096;
	fd = open("BUSFILE", O_RDWR);

	int* ptr = NULL;
	/*
	映射失败，申请了映射内存，但是没有与映射文件建立关联，
	后续访问映射内存时会失败，产生总线错误，进程终止。
	*/
	if ((ptr = mmap(NULL, mapsize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED)
	{
		perror("Map Failed,\n");
		exit(0);
	}

	ptr[0] = 0x34333231;
	munmap(ptr, mapsize);
	close(fd);

	return 0;
}

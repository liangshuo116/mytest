#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

// 对映射文件进行共享映射，而后修改映射内容，观察该修改是否能同步到文件中
int main()
{
	int fd = open("MAPFILE", O_RDWR); // 打开MAPFILE文件
	int fsize;
	fsize = lseek(fd, 0, SEEK_END); // 获取映射文件大小
	int* ptr = NULL;
	// 申请共享映射内存
	if ((ptr = mmap(NULL, fsize, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0)) == MAP_FAILED)
	{
		perror("Map Failed.\n");
		exit(0);
	}
	close(fd);

	ptr[0] = 0x34333231; // 修改映射内存前四字节，改为1234
	printf("mmap Change Success, it's done.\n");
	munmap(ptr, fsize); // 释放映射内存

	return 0;
}

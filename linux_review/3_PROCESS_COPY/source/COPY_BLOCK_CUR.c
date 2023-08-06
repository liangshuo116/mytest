#include <PROCESS_COPY.h>

int COPY_BLOCK_CUR(const char* Sfile, int prono)
{
	// 打开文件
	int fd;
	if ((fd = open(Sfile, O_RDONLY)) == -1)
	{
		perror("错误：COPY_BLOCK_SIZE OPEN Failed.\n");
		exit(0);
	}
	// 获取文件大小
	int fsize;
	fsize = lseek(fd, 0, SEEK_END);
	// 计算每个进程所需处理的数据量，分块，返回块大小
	if (fsize % prono == 0)
	{
		return fsize / prono;
	}
	else return fsize / prono + 1;
}

#include <THREAD_COPY.h>

int COPY_BLOCK_CUR(const char* Sfile, int prono)
{
	//设置文件描述符
	int fd;
	if ((fd = open(Sfile, O_RDONLY)) == -1)
	{
		printf("error: COPY_BLOCK_CUR, 打开文件失败.\n");
		exit(0);
	}
	//确定文件大小
	int fsize;
	fsize = lseek(fd, 0, SEEK_END);
	//分块，返回块大小
	if (fsize % prono == 0)
	{
		return fsize / prono;
	}
	else return fsize / prono + 1;
}

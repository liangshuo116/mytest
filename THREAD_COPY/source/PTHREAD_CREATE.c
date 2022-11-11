#include <THREAD_COPY.h>

//全局映射变量
char* ptr = NULL;
const char* DFILE;
int pos;
int BLOCKSIZE;

void* pthread_job(void* flags)
{
	char buffer[BLOCKSIZE];
	bzero(buffer, sizeof(buffer));
	pos = *(int*)flags * BLOCKSIZE;
	snprintf(buffer, BLOCKSIZE + 1, "%s", ptr + pos);
	printf("Thread 0x%x pos %d blocksize %d\n", (unsigned int)pthread_self(), pos, BLOCKSIZE);
	//将函数写入目标文件时要注意偏移量
	int Dfd;
	Dfd = open(DFILE, O_RDWR | O_CREAT, 0664);
	lseek(Dfd, pos, SEEK_SET);
	write(Dfd, buffer, strlen(buffer));

	close(Dfd);
	pthread_exit(NULL);
}

int PTHREAD_CREATE(const char* Sfile, const char* Dfile, int prono, int blocksize)
{
	//映射文件
	int fd = open(Sfile, O_RDONLY);
	int fsize;
	//int* ptr = NULL;
	//获取文件大小
	fsize = lseek(fd, 0, SEEK_END);
	
	if ((ptr = mmap(NULL, fsize, PROT_READ, MAP_PRIVATE, fd, 0)) == MAP_FAILED)
	{
		perror("mmap Failed.\n");
		exit(0);
	}
	close(fd);

	//定义线程属性
	pthread_attr_t Attr;
	pthread_attr_init(&Attr);
	//修改默认属性为分离态线程
	pthread_attr_setdetachstate(&Attr, PTHREAD_CREATE_DETACHED);
	
	//创建线程
	pthread_t tid[10];
	int err;
	int flags;
	BLOCKSIZE = blocksize;
	DFILE = Dfile;
	for (flags = 0; flags < prono; flags++)
	{
		pthread_create(&tid[flags], &Attr, pthread_job, (void*)&flags);
		usleep(1000);
		printf("Master Create Thread %d Success.\n", flags);
	}
	pthread_attr_destroy(&Attr);
	munmap(ptr, fsize);
	return 0;
}

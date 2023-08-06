#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>

typedef struct
{
	int id;
	char name[20];
}shared_t;
// 读端（利用mmap实现进程间通信）
int main()
{
	int fd;
	int size = sizeof(shared_t); 	// 映射文件大小
	shared_t* ptr = NULL; 			// shared_t 类型指针
	fd = open("MapFile", O_RDWR | O_CREAT, 0664);
	//ftruncate(fd, size); // 截断文件，修改文件为指定大小

	ptr = mmap(NULL, size, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
	// 初始化映射数据
	//ptr->id = 0;
	//bzero(ptr->name, sizeof(ptr->name));

	while (1) // 读端打印映射数据
	{
		printf("Data id %d , name %s...\n", ptr->id, ptr->name);
		sleep(1); // 每隔一秒打印一次数据
	}

	munmap(ptr, size); 	// 释放映射内存
	close(fd); 			// 关闭文件描述符

	return 0;
}

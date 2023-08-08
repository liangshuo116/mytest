#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>

int CHECK_ARG(int argno, int thrdno, const char* Sfile); // 参数检测
int PROCESS_MMAP(const char* Sfile, ); // 将文件映射到进程中
// 创建线程分配工作

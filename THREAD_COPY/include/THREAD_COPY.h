#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>

//参数校验
int CHECK_ARG(int argc, const char* Sfile, int prono);
//返回块大小blocksize
int COPY_BLOCK_CUR(const char* Sfile, int prono);
//线程创建
int PTHREAD_CREATE(const char* Sfile, const char* Dfile, int prono, int blocksize);

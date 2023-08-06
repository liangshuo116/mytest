#include <PROCESS_COPY.h>

int main(int argc, char** argv)
{
	int prono;
	int blocksize;
	// 进程数量参数可缺省，默认为5
	if (argv[3] == 0)
	{
		prono = 5;
	}
	else prono = argv[3];

	CHECK_ARG(argc, prono, argv[1]); 	// 校验参数
	blocksize = COPY_BLOCK_CUR(argv[1], prono); 	// 分块
	PROCESS_CREATE(argv[1], argv[2], blocksize, prono); // 创建进程
	return 0;
}

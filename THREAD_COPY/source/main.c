#include <THREAD_COPY.h>

int main(int argc, char** argv)
{
	int prono;
	int blocksize;
	if (argv[3] == 0)
	{
		prono = 5;
	}
	else prono = atoi(argv[3]);
	CHECK_ARG(argc, argv[1], prono);
	blocksize = COPY_BLOCK_CUR(argv[1], prono);
	PTHREAD_CREATE(argv[1], argv[2], prono, blocksize);
	return 0;
}

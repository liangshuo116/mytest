#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// 进程测试用例（测试用信号杀死进程）
int main()
{
	while (1)
	{
		printf("Running..\n");
		sleep(1);
	}
	return 0;
}

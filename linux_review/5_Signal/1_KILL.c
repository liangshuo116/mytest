#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

// 自定义函数触发信号
int main(int argc, char** argv)
{
	// KILL 9 1000 传三个参数
	if (argc < 3) // 判断参数数量
	{
		printf("Error, param error!\n");
		exit(0);
	}
	// 向任意进程发送任意信号
	// 函数触发信号（传入参数为字符串，需转为整型）
	kill(atoi(argv[2]), atoi(argv[1]));
	return 0;
}

#include <PROCESS_COPY.h>

int CHECK_ARG(int argno, int prono, const char* Sfile)
{
	// 判断参数数量（源文件，目标文件，进程数量）
	if (argno < 3)
	{
		printf("错误：参数数量异常，请求检查参数重新启动！\n");
		exit(0);
	}

	// 判断进程数量
	if (prono <= 0 || prono >= 100)
	{
		printf("错误：进程数量异常，进程数不得小于0，大于100！\n");
		exit(0);
	}

	// 判断源文件是否存在
	if (access(Sfile, F_OK) != 0)
	{
		printf("错误：源文件不存在！\n");
		exit(0);
	}
	return 0;
}

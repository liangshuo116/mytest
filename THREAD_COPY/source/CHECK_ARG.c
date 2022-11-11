#include <THREAD_COPY.h>

int CHECK_ARG(int argc, const char* Sfile, int prono)
{
	//判断参数数量
	if (argc < 3)
	{
		printf("error: 参数数量异常，请求检查参数重新启动.\n");
		exit(0);
	}
	//判断源文件是否存在
	if (access(Sfile, F_OK) != 0)
	{
		printf("error: 源文件不存在.\n");
		exit(0);
	}
	//判断进程数量
	if (prono <= 0 || prono >= 100)
	{
		printf("error: 进程数量不允许小于0, 大于100.\n");
		exit(0);
	}
	return 0;
}

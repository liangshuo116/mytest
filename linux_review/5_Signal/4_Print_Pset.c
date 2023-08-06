#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

// 打印未决信号集，查看被屏蔽信号
void Print_Pset(sigset_t pset)
{
	int signum;
	for (signum = 1; signum < 32; signum++)
	{
		if (sigismember(&pset, signum) == 1)
		{
			putchar('1');
		}
		else putchar('0');
	}
	printf("\n");
}

int main()
{
	sigset_t new_set, old_set;
	sigset_t pset;
	sigemptyset(&new_set); // 初始化为 0

	// 将特定信号的位码设置为 1
	sigaddset(&new_set, SIGINT); 	// 2 Ctrl+c杀死前台进程
	sigaddset(&new_set, SIGQUIT); 	// 3 Ctrl+\杀死前台进程，产生一个core文件

	sigprocmask(SIG_SETMASK, &new_set, &old_set); // 替换屏蔽字信号集

	while (1)
	{
		sigpending(&pset); 	// 间隔获取传出进程的未决信号集
		Print_Pset(pset); 	// 遍历打印未决信号集的位码
		sleep(1);
	}
}

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

// 信号屏蔽，对信号SIGINT(2)进行屏蔽
int main()
{
	// 对 Ctrl + c 进行屏蔽
	sigset_t new_set; 	// 信号集类型，自定义创建新的信号屏蔽字
	sigset_t old_set; 	// 用来接收旧的信号屏蔽字
	
	sigemptyset(&new_set);  		// 将新屏蔽字初始化为 0
	sigaddset(&new_set, SIGINT); 	// 将指定的 SIGINT 信号的位码设置为 1

	// 替换进程屏蔽字信号集，覆盖新的，传出保存旧的
	sigprocmask(SIG_SETMASK, &new_set, &old_set); 
	
	// 此时，Ctrl+c 和 kill -2 pid 都不能杀死该进程
	while (1)
		sleep(1);

	return 0;
}

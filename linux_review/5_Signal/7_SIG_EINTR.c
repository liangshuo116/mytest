#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <errno.h>

// 捕捉函数
void Sig_do(int n)
{
	printf("捕捉 %d SIGINT..\n", n);
}
// 信号捕捉会强制中断主函数中所有的阻塞函数
int main()
{
	struct sigaction new_act, old_act;
	new_act.sa_handler = Sig_do;
	new_act.sa_flags = 0;
	sigemptyset(&new_act.sa_mask);
	// 设定捕捉信号
	sigaction(SIGINT, &new_act, &old_act);

	printf("阻塞读取标准输入...\n");
	char buf[1024];
	bzero(buf, sizeof(buf));
	if (read(STDIN_FILENO, buf, sizeof(buf)) == -1)
	{
		if (errno == EINTR)
		{
			printf("主函数 read，被强制中断.\n");
			exit(0);
		}
	}
	return 0;
}

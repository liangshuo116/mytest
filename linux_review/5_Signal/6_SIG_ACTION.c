#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

// 捕捉函数
void Sig_do()
{
	printf("Sig Action...\n");
}
// 捕捉信号 SIGINT
int main()
{
	struct sigaction new_act, old_act; 	// 信号行为结构体
	new_act.sa_handler = Sig_do; 		// 行为接口，捕捉行为（捕捉函数的地址）
	new_act.sa_flags = 0; 				// 默认选项，取决于行为接口
	sigemptyset(&new_act.sa_mask); 		// 临时屏蔽字，不用时置空

	// 捕捉信号 SIGINT，绑定到捕捉函数 Sig_do
	sigaction(SIGINT, &new_act, &old_act); // 捕捉设定完毕

	while (1)
		sleep(1);

	return 0;
}

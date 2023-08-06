#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

// 将信号行为改为忽略行为
int main()
{
	struct sigaction new_act, old_act; 	// 信号行为结构体
	new_act.sa_handler = SIG_IGN; 		// 行为接口，忽略行为
	new_act.sa_flags = 0; 				// 默认选项，取决于行为接口
	sigemptyset(&new_act.sa_mask); 		// 将 sa_mask 初始化为 0

	// 选择信号 SIGINT 忽略，用new_act修改其信号行为，传出此信号原有行为到 old_act 中
	sigaction(SIGINT, &new_act, &old_act);

	while (1)
		sleep(1);
	
	return 0;
}

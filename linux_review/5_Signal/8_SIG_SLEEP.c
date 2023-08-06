#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

// 时序竞态
// 信号捕捉函数：自定义一个sleep函数
/*
如果在 pause() 前添加一个 sleep(3)，就会导致进程永久挂起：
	定时开始，此时进程被系统调用挂起 3s，但是2s后系统发送 SIGALRM信号给进程，此时距进程被唤醒还有 1s，信号处于内核层待处理，3s到，进程唤醒，返回用户层前，检测到有信号未处理，处理 SIGALRM信号，到用户层调用捕捉函数 Sig_alarm，处理完毕执行 SIGRETURN 回到内核层，再返回用户层，从 sleep(3) 即被中断位置继续执行，调用 pause() 函数，因为 SIGALRM 已经被处理，因此进进程收不到信号，pause() 就无法唤醒当前进程，因此永久挂起。
	这就是进程的时序竞态问题的缩影，某个进程或功能非常依赖一个特定消息、事件或信号，但是因为时间上的差异或冲突，导致此功能无法接收到信号，引发一系列后续问题。

解决方法：原子方法
	预先屏蔽 SIGALRM 信号，避免其提前被处理掉;
	将 pause() 替换为 sigsuspend() 挂起的同时，解除信号屏蔽。其余功能一致
	处理时序问题，让其关键操作不可分离，采用原子方式处理。
*/

void Sig_alarm(int n)
{
	// 捕捉到信号，调用函数
	printf("SIGALRM was gotten.\n");
}

unsigned int SLEEP(unsigned int seconds)
{
	// 信号捕捉
	struct sigaction new_act, old_act;
	new_act.sa_handler = Sig_alarm;
	new_act.sa_flags = 0;
	sigemptyset(&new_act.sa_mask);
	sigaction(SIGALRM, &new_act, &old_act);
	
	// 预先屏蔽 SIGALRM 信号
	sigset_t new_set, old_set;
	sigemptyset(&new_set);
	sigaddset(&new_set, SIGALRM);
	sigprocmask(SIG_SETMASK, &new_set, &old_set);
	
	// 定时器，定时 seconds 秒后系统向当前进程发送 SIGALRM 信号
	unsigned int reval;
	reval = alarm(seconds);
	
	sleep(3);
		
	// 立即挂起当前进程，察觉到信号后唤醒当前进程
	//pause();
	// 临时解除对 SIGALRM 信号的屏蔽并挂起进程
	sigsuspend(&new_act.sa_mask);

	// 进程挂起（等待 2 秒后系统发送的 SIGALRM 信号）
	// 返回先前计划的剩余秒数
	printf("reval %d\n", reval); // 0
	return reval;
}

int main()
{
	while (1)
	{
		printf("sleep two seconds.\n");
		SLEEP(2);
		/*
		sleep two seconds.
		SIGALRM was gotten.
		reval 0
		...
		*/
	}
	return 0;
}

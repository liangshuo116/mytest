#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

// 用信号实现进程间通信，交替报数，数值传递，信号携带数据

void Parent_sig(int sigNum, siginfo_t* info, void* arg)
{
	// info 结构体可以接收其他进程信号携带的数据
	printf("Parent Recv Number: %d\n", info->si_int); // 接收数据包的捕捉函数参数
	union sigval val;
	val.sival_int = ++(info->si_int);
	sigqueue(getpid(), SIGUSR1, val);
	usleep(100000);
}
void Child_sig(int sigNum, siginfo_t* info, void* arg)
{
	// info 结构体可以接收其他进程信号携带的数据
	printf("Child Recv Number: %d\n", info->si_int);
	union sigval val;
	val.sival_int = ++(info->si_int);
	sigqueue(getpid(), SIGUSR2, val);
	usleep(100000);
}

int main()
{
	// 捕捉信号
	struct sigaction new_act, old_act;
	new_act.sa_sigaction = Parent_sig;
	new_act.sa_flags = SA_SIGINFO;
	sigemptyset(&new_act.sa_mask);
	sigaction(SIGUSR2, &new_act, &old_act);
	// 防止子进程还未捕捉信号，信号就已递达从而杀死子进程
	// 父进程设置信号屏蔽（SIGUSR1），继承给子进程
	sigset_t new_set, old_set;
	sigemptyset(&new_set);
	sigaddset(&new_set, SIGUSR1);
	sigprocmask(SIG_SETMASK, &new_set, &old_set);

	pid_t pid;
	pid = fork();

	if (pid > 0)
	{
		// 父进程先发送一次信号，并携带数据
		union sigval val; // 发送的联合体数据包
		val.sival_int = 1;
		printf("Parent Send Number: %d\n", val.sival_int);
		sigqueue(pid, SIGUSR1, val); // 不用kill函数发送信号，不能携带数据
		// 等待信号
		while (1)
			sleep(1);
	}
	else if (pid == 0)
	{
		// 捕捉信号
		struct sigaction new_act, old_act;
		new_act.sa_sigaction = Child_sig;
		new_act.sa_flags = SA_SIGINFO;
		sigemptyset(&new_act.sa_mask);
		sigaction(SIGUSR1, &new_act, &old_act);
		// 解除对信号 SIGUSR1 的屏蔽
		sigprocmask(SIG_SETMASK, &new_act.sa_mask, NULL);

		// 等待信号
		while (1)
			sleep(1);
	}
	else
	{
		perror("Fork Call Failed.\n");
		exit(0);
	}
	return 0;
}


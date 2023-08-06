#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>

int Daemon_Job()
{
	int fd;
	time_t tp;
	char time_buf[1024];

	//if ((fd = open("xxx", O_RDWR)) == -1) // 测试标准出错重定向到特定文件
	if ((fd = open("time.log", O_RDWR | O_CREAT, 0664)) == -1)
	{
		perror("time.log Open Failed.\n");
		exit(0);
	}
	// 每隔 3 秒，向 time.log 中写入系统时间，后台执行
	while (1)
	{
		bzero(time_buf, sizeof(time_buf));
		tp = time(NULL); 			// 获取时间种子
		ctime_r(&tp, time_buf); 	// 将时间转换为字符串并写入缓冲区
		write(fd, time_buf, strlen(time_buf)); // 写入文件
		sleep(5); 	// 5 秒循环（如果没有这个间隔时间，磁盘会被瞬间写满，造成其他进程崩溃）
	}
}

void Daemon_Create()
{
	pid_t pid;
	// 创建出错输出文件
	int err_fd;
	if ((err_fd = open("ERROR_FILE", O_RDWR | O_CREAT, 0664)) == -1)
	{
		perror("Open Call Failed.\n");
	}

	pid = fork(); 	// 1.父进程创建子进程
	if (pid > 0)
	{
		exit(0); 	// 2.父进程退出
	}
	else if (pid == 0)
	{
		setsid(); 	// 3.子进程创建新会话，脱离控制终端
		// 4.关闭无用的文件描述符（进程创建后，默认打开三个描述符0,1,2）
		close(STDIN_FILENO); 	// 标准输入
		close(STDOUT_FILENO); 	// 标准输出
		dup2(err_fd, STDERR_FILENO); // 标准出错
		// 对 STDERR_FILENO 进行输出重定向，将错误信息写入到特定文件中，而不是屏幕上
		chdir("./"); 	// 5.修改进程的工作路径，改为根目录（当前不是目标主机，传入当前目录，相当于不修改）
		Daemon_Job(); 	// 6.守护进程的核心工作
		// 7.守护进程的退出处理（释放资源），当前工作未创建其他资源，因此这不做处理
	}
	else
	{
		perror("Fork Call Failed.\n");
		exit(0);
	}
}

int main()
{
	Daemon_Create(); 	// 创建守护进程
	return 0;
}

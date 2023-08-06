#include <PROCESS_COPY.c>

int main(int argc, char** argv)
{
	int Sfd, Dfd;
	Sfd = open(argv[1], O_RDONLY); 					// 源文件
	Dfd = open(argv[2], O_RDWR | O_CREAT, 0664); 	// 目标文件
	
	int pos; 		// 拷贝位置
	int blocksize; 	// 拷贝大小
	pos = atoi(argv[3]);
	blocksize = atoi(argv[4]);

	lseek(Sfd, pos, SEEK_SET); 	// 获取偏移量，改变文件的读写指针位置
	lseek(Dfd, pos, SEEK_SET);

	// 从源文件读取
	char buffer[blocksize];
	bzero(buffer, blocksize);

	ssize_t recvlen;
	recvlen = read(Sfd, buffer, blocksize);
	// 写入目标文件
	write(Dfd, buffer, recvlen);

	close(Sfd);
	close(Dfd);
	return 0;
}


#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

int main(void)
{

	char s[] = "abc.txt";
	int i = 0;

	//close(STDOUT_FILENO);//关闭标准输出文件描述符
	int fd1 = open("/dev/pts/0", O_WRONLY);//打开设备文件/dev/pts/1，该设备将做为标准输出文件描述符
	if (fd1 == -1 )
	{
		printf("open error: %s\n", strerror(errno));
		return -1;
	}
	int fd = open(s, O_RDONLY);//
	if (fd == -1)
	{
		printf("open error: %s\n", strerror(errno));
		return -2;
	}

	while(1)
	{
		printf("fd1 = %d, fd = %d:%d\n", fd1, fd, i++);//printf语句将会打印到/dev/pts/1上
		sleep(1);//
	}
	close(fd1);
	close(fd);
	return 0;
}

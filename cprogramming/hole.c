#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

int main()
{
	char *str = "abcdefghijk";
	char *fname = "holefile";
	int fd;
	
	if ( (fd = open(fname, O_WRONLY|O_CREAT)) == -1)
		{
			perror("open error");
			return -1;
		}
	if ((write(fd, str, strlen(str))) != strlen(str))
	{
			perror("write error");
			return -1;
	}
	
	if (lseek(fd, 16385, SEEK_SET) == -1)
		{
			perror("write error");
			return -1;
		}
		
	if ((write(fd, str, strlen(str))) != strlen(str))
	{
			perror("write error");
			return -1;
	}
	
	close(fd);
	return 0;
}
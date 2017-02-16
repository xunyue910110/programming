#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
size_t getfilesize(const char* filepath)
{
	struct stat fs ;
	int ret;
	ret = stat(filepath, &fs);
	if (ret == -1)
	{
		perror("file stat error");
		return -1;
	}
	return fs.st_size;
}

size_t getfilesize2(int fd)
{
	struct stat fs ;
	int ret;
	ret = fstat(fd, &fs);
	if (ret == -1)
	{
		perror("file stat error");
		return -1;
	}
	return fs.st_size;
}

int main(int argc, char*argv[])
{
	size_t filesize = 0;
	const char* filename="/root/install.log";
	filesize = getfilesize(filename);
	//filesize = getfilesize2(1);
	if (filesize == -1)
	{
		perror("get filesize error");
		return -1;
	}
	else
	{
		printf("file %s size : %d \n", filename, filesize);
		//printf("fileid %d size : %d \n", 3, filesize);
	}
	
	struct stat fs ;
	int ret;
	ret = stat(filename, &fs);
	if (ret == -1)
	{
		perror("file stat error");
		return -1;
	}
	printf("blk size: %d", fs.st_blksize);
	printf("blk count: %d", fs.st_blocks);
	
	return 0;
}
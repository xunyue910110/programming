#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#define BUFSIZE 4096
int main()
{
	int bytes;
	int buffer[BUFSIZE];
	
	while((bytes=read(STDIN_FILENO, buffer, BUFSIZE))>0)
	{
		fprintf(stderr, "%d bytes read.\n", bytes);
		if(write(STDOUT_FILENO, buffer, bytes ) != bytes)
		{
			perror("write error");
			exit(-1);
		}
		fprintf(stderr, "%d bytes writed.\n", bytes);
	}
	if (bytes < 0)
	{
		perror("read error");
		exit(-1);
	}
	else{
		//write(STDERR_FILENO, "end of file.", strlen("end of file."));
		fprintf(stderr, "copy completed.\n");
	}
	return 0;
}
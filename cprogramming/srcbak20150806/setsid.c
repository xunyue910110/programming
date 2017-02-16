#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
int setup_daemon(int nochdir, int noclose);

int main(int argc, char *argv[])
{
	setup_daemon(0, 0);
	printf("test ...\n");
	//daemon(0, 0);
	
	for(;;);
	return 0;
}

int setup_daemon(int nochdir, int noclose)
{
	int pid = 0;
	int sid = 0;
	int i = 0;
	pid = fork();
	if (pid == -1)
	{
		perror("fork err");
		return -1;
	}
	if (pid > 0)
	{
		exit(0);
	}
	
	printf("prepared to setsid\n");
	//fflush(stdout);
	
	sid = setsid();
	if (sid == -1)
	{
		perror("setsid error");
		return -2;
	}
	printf("pid: %d\n", getpid());
	if (nochdir == 0)
	{
		chdir("/");
	}
	if (noclose == 0)
	{
		for (i = 0; i < 3; i++)
		{
			close(i);
		}	
		open("/dev/null", O_RDWR);
		dup(0);
		dup(0);
	}
	
	return 0;
}
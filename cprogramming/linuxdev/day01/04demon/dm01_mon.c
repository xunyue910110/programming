#include <sys/types.h>
#include <unistd.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <signal.h>
#include <errno.h>
#include <signal.h>
   #include <sys/stat.h>
       #include <fcntl.h>

   //int daemon(int nochdir, int noclose);
int my_daemon(int nochdir, int noclose)
{
	int i = 0;
	pid_t pid;
	
	pid = fork();
	if (pid == -1)
	{
		perror("fork err");
		exit(0);
	}
	

	if (pid > 0)
	{
		exit(0);
	}
	printf("pid:%d \n", getpid());
	pid = setsid();
	if (pid == -1)
	{
		perror("fork err");
		exit(0);
	}
	
	if (nochdir != 0)
		chdir("/");
	
	if (noclose != 0)
	{
		for (i=0; i<3; i++)
		{
			close(i);
		}
	}

	open("/dev/null", O_RDWR); 
	dup(0);
	dup(1);
	
	
	while(1)
	{
		sleep(1);
	}
	
	printf("hello\n");
	return 0;
}


int main()
{
		my_daemon(1, 1);
		return 0;
}





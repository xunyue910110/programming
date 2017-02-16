#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#define ERR_EXIT(m) \
	do \
	{	\
		perror(m); \
		exit(EXIT_FAILURE); \
	}while(0)
		
int main(int argc, char *argv[])
{
	pid_t pid;
	pid = fork();
	if (pid == -1)
		ERR_EXIT("fork error");
	if (pid == 0)
	{
		sleep(3);
		printf("this is child\n");
		//exit(100);
		abort();
	}
	
	int ret;
	printf("this is parent\n");
	int status;
	ret = wait (&status);
	if (ret == -1)
	{
		ERR_EXIT("wait error");
	}
	printf("ret = %d, pid = %d\n", ret, pid);
	if (WIFEXITED(status))
	{
		printf("child process exit code: %d\n", WEXITSTATUS(status));
		
	}
	else 
	{
		printf("child process exit abnormally\n");
		if (WIFSIGNALED(status))
		{
			printf("the signal number: %d\n", WTERMSIG(status));
		}
	}
	return 0;
}
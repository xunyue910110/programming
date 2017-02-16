#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

int main()
{
	pid_t pid;
	//signal(SIGCHLD, SIG_IGN);
	printf("before fork pid: %d\n", getpid());
	int abc = 10;
	
	pid = fork();
	if (pid == -1)
	{
		printf("pid < 0 err.\n");
		return -1;
	}
	if (pid > 0	)
	{
		abc ++;
		printf("parent pid: %d, child pid: %d\n", getpid(), pid);
		printf("abc: %d\n", abc);
		sleep(30);
	}
	else if (pid == 0)
	{
		abc++;
		printf("child pid: %d, parent pid: %d\n", getpid(), getppid());
		printf("abc: %d\n", abc);
	}
	
	printf("fork after...\n");
	return 0;
}
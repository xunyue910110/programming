#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>

void TestFunc()
{
	printf("fffff\n");
}
int main()
{
	int procnum = 0;
	int loopnum = 0;
	printf("pls enter your procnum :\n");
	scanf("%d", &procnum);
	printf("pls enter your loopnum :\n");
	scanf("%d", &loopnum);
	
	pid_t pid = 0;
	pid_t ret = 0;
	int i = 0;
	int j = 0;
	for (i = 0; i< procnum; i++	)
	{
		pid = fork();
		if (pid == -1 )
		{
			return -1;
		}
		if (pid == 0)
		{
			for (j = 0; j< loopnum; j++)
			{
				TestFunc();
			}
			
			exit(0);
		}
	
	}
	while (1)
	{
		ret = waitpid(0, NULL, 0);
		if (ret == -1)
		{
			if (errno == EINTR)
			{
				continue;
			}
			perror("ret break");
			break;
		}
		printf("child %d process exit.\n", ret);
	}
	printf("parent process exit.\n");
	return 0;
}

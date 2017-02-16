#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <errno.h>

int main01()
{
	pid_t pid;
	//signal(SIGCHLD, SIG_IGN);
	printf("before fork pid: %d\n", getpid());
	int abc = 10;
	int ret = 0;
	char *argv[] = {"/bin/ls", "-lrt", NULL};
	char *envp[] = {"aaa=1", "bbb=2", NULL};
	
	pid = vfork();
	if (pid == -1)
	{
		printf("pid < 0 err.\n");
		return -1;
	}
	if (pid > 0	)
	{
		abc++;
		printf("parent pid: %d, child pid: %d\n", getpid(), pid);
		//sleep(10);
		printf("In parent,abc: %d\n", abc);
	}
	else if (pid == 0)
	{
		//sleep(10);
		printf("child pid: %d, parent pid: %d\n", getpid(), getppid());
		printf("In child abc: %d\n", abc);
		//ret = execve("/bin/ls", argv, NULL);
		execle("./environ", NULL, envp);
		if (ret == -1)
		{
			perror("execve:");
			return 0;
		}
		
		printf("execve............\n");
	}
	
	printf("fork after...\n");
	return 0;
}

int main()
{
    mysystem("ls -l |wc -l");
    return 0;
}

int mysystem(const char * command)
{
    pid_t pid;
    int ret = 0;
	int status = 0;
	
	if (command == NULL)
	{
		return -1;
	}

    pid = fork();
    if (pid == -1)
    {
		perror("vfork error");
		return -1;
    }
	
	if (pid == 0)
	{
		execl("/bin/sh", "sh", "-c", command, NULL);
	}
	
	while ((ret = waitpid(pid, &status, 0)) < 0)
	{
		if (errno == EINTR)
			continue;
		status = -1;
		break;
	}
	printf("parent process exit.\n");
}

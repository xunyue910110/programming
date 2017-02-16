#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>


int main()
{
	pid_t pid;
	signal(SIGCHLD, SIG_IGN);
	int fd  = 0;
	int nbytes = 0;
	printf("before fork pid: %d\n", getpid());
	
	fd = open("./1.txt", O_RDWR|O_CREAT);
	if (fd == -1)
	{
		perror("open");
		return -1;
	}
	pid = fork();
	if (pid == -1)
	{
		printf("pid < 0 err.\n");
		return -1;
	}
	if (pid > 0	)
	{
		printf("parent pid: %d, child pid: %d\n", getpid(), pid);
		write(fd, "someting good", 8);
	}
	else if (pid == 0)
	{
		
		printf("child pid: %d, parent pid: %d\n", getpid(), getppid());
		write(fd, "otherthing", 7);
	}
	
	close(fd);
	printf("fork after...\n");
	return 0;
}
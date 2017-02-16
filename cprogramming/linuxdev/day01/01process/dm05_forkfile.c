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


int main(void )
{
	pid_t pid;
	int fd; 
//	signal(SIGCHLD, SIG_IGN);
	printf("befor fork pid:%d \n", getpid());
	
	/*
	RETURN VALUE
       open() and creat() return the new file descriptor, or -1 if an error occurred  (in  which  case,  errno  is  set
       appropriately).
       */

	
	fd  = open("./1.txt", O_RDWR);
	if (fd == -1)
	{
		perror("myopen");
		return -1;
	}


	pid = fork(); //errno
	if (pid == -1)	
	{
		perror("tile");
		return -1;
	}
	if (pid > 0)
	{
		printf("parent: pid:%d \n", getpid());
		//#include <unistd.h>

       //ssize_t write(int fd, const void *buf, size_t count);
       const char *p = "parent";
       int i;
        for(i=0; i<6; i++)
		    write(fd, ("parnet"+i), 1);
    
		close(fd);
        wait(NULL);
	
		//sleep(20);
	}
	else if (pid == 0)
	{

		printf("child: %d, parent: %d \n", getpid(), getppid());
        const char *p="child";
        int i;
        for(i=0; i < 5; i++)
		    write(fd, &"child"[i], 1);

		close(fd);
		exit(0);
	}

	printf("fork after....\n");
	return 0;
}









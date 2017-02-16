#include "apue.h"
#include <unistd.h>
#include <stdio.h>

int		glob = 6;		/* external variable in initialized data */
char	buf[] = "a write to stdout\n";

int
main(void)
{
	int		var;		/* automatic variable on the stack */
	pid_t	pid;

	var = 88;
	if (fwrite(buf, 1,sizeof(buf)-1, stdout) != sizeof(buf)-1)
		err_sys("write error");
  //printf("%s", buf);
	printf("before fork\n");	/* we don't flush stdout */

	if ((pid = fork()) < 0) {
		err_sys("fork error");
	} else if (pid == 0) {		/* child */
		glob++;					/* modify variables */
		var++;
	} else {
		sleep(2);				/* parent */
	}

	printf("pid = %d, glob = %d, var = %d\n", getpid(), glob, var);
	_exit(0);
}

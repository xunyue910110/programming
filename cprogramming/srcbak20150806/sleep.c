#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <errno.h>
void handler(int num)
{
	printf("recv signal number: %d\n", num);
	if (num == SIGQUIT)
	{
		printf("recv SIGQUIT");
		//exit(0);
	}
	else if (num == SIGINT)
	{
		printf("recv SIGINT");
		//exit(0);
	}
}
int main(int argc, char *argv[])
{
	printf("main...begin.\n");
	signal(SIGINT, handler);
	//signal(SIGQUIT, handler);
	
	//sleep(10);
	int n = 10;
	
	/*do
	{
		n = sleep(n);
	}while (n > 0);
	*/
	
	while ( n > 0)
	{
		n = sleep(n);
	}
	printf("main...end.\n");
	return 0;
}
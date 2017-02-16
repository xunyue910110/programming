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
		sigset_t uset;
		sigemptyset(&uset);
		sigaddset(&uset, SIGINT);
		sigprocmask(SIG_UNBLOCK, &uset, NULL);
		//exit(0);
	}
	else if (num == SIGINT)
	{
		printf("recv SIGINT");
		//exit(0);
	}
}

void printsigset(sigset_t *set)
{
	int i = 0;
	for (i = 1; i<= 64; i++)
	{
		if (sigismember(set, i))
			putchar('1');
		else
			putchar ('0');
	}
}
int main(int argc, char *argv[])
{
	sigset_t bset;
	sigset_t pset;
	
	printf("main...begin.\n");
	__sighandler_t dft = signal(SIGINT, handler);
	signal(SIGQUIT, handler);
	
	sigemptyset(&bset);
	sigaddset (&bset, SIGINT);
	
	//int sigprocmask(int how, const sigset_t *set, sigset_t *oldset);
	sigprocmask(SIG_BLOCK, &bset, NULL);
	signal(SIGINT, SIG_DFL);
	
	while (1)
	{
		printf("hello....\n");
		sleep(1);
		sigpending(&pset);
		printsigset(&pset);
	}
	
	printf("main...end.\n");
	return 0;
}
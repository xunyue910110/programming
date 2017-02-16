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

		exit(0);
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

void mysigaction(int sig, siginfo_t *info, void *p)
{
  printf("recv %d\n", sig);
  long myint = 0;
  //myint = info->si_value.sival_int;
 // myint = info->si_int;
	myint = *(long*)info->si_ptr;
  //printf("recved number is %d, %d\n", info->si_value.sival_int, myint);
  printf("recved number is %d\n", myint);
}
   
int main(int argc, char *argv[])
{
	sigset_t bset;
	sigset_t pset;
	
	printf("main...begin.\n");
/*       int sigaction(int signum, const struct sigaction *act,
                     struct sigaction *oldact);
	    struct sigaction {
               void     (*sa_handler)(int);
               void     (*sa_sigaction)(int, siginfo_t *, void *);
               sigset_t   sa_mask;
               int        sa_flags;
               void     (*sa_restorer)(void);
           };

 */

 
	struct sigaction act;
	//act .sa_handler = handler;
	act.sa_sigaction = mysigaction;
	act.sa_flags = SA_SIGINFO;
	sigaction(SIGINT, &act, NULL);
	
	union sigval mys;
	long val = 333333333;
	//mys.sival_int = 222;
	mys.sival_ptr = &val;
	int ret = sigqueue(getpid(), SIGINT, mys);
	
	while (1)
	{
		printf("hello....\n");
		sleep(1);
	}
	
	printf("main...end.\n");
	return 0;
}
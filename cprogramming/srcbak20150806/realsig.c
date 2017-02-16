#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <errno.h>

void myhandler1(int num)
{
	printf("recv sig num: %d\n", num);
	if (num == SIGUSR1)
	{
	sigset_t bset;
	sigemptyset(&bset);
	sigaddset(&bset, SIGINT);
	sigaddset(&bset, SIGRTMIN);
	sigprocmask(SIG_UNBLOCK, &bset, NULL);
	}
}

void myhandler(int num, siginfo_t *info, void *p)
{
	printf("recv sig num: %d\n", num);
	if (num == SIGINT || num == SIGRTMIN)
	{
		int  myint = info->si_int;
		printf("recv data: %d\n", myint);	
	}

	
	if (num == SIGUSR1)
	{
	sigset_t bset;
	sigemptyset(&bset);
	sigaddset(&bset, SIGINT);
	sigaddset(&bset, SIGRTMIN);
	sigprocmask(SIG_UNBLOCK, &bset, NULL);
	}
}

int main()
{
	pid_t pid;
	
	struct sigaction act;
	//act.sa_handler = myhandler;
	act.sa_sigaction = myhandler;
	act.sa_flags = SA_SIGINFO;
	
	if (sigaction(SIGINT, &act, NULL) < 0)
	{
		perror("sigaction error");
		exit(-1);
	}
	
	if (sigaction(SIGRTMIN, &act, NULL) < 0)
	{
		perror("sigaction error");
		exit(-1);
	}	
	if (sigaction(SIGUSR1, &act, NULL) < 0)
	{
		perror("sigaction error");
		exit(-1);
	}
	
	sigset_t bset;
	sigemptyset(&bset);
	sigaddset(&bset, SIGINT);
	sigaddset(&bset, SIGRTMIN);
	sigprocmask(SIG_BLOCK, &bset, NULL);
	
	pid = fork();
	if (pid == -1)
	{
		perror("fork error");
		exit(-2);
		
	}
	if (pid == 0)
	{
		/*int sigqueue(pid_t pid, int sig, const union sigval value);
		           union sigval {
               int   sival_int;
               void *sival_ptr;
           };

		*/
		union sigval mys;
		mys.sival_int = 201;
		int i = 0;
		int ret = 0;
		for (i = 0; i< 3; i++)
		{
			
			ret = sigqueue(getppid(), SIGINT, mys);
			if ( ret != 0)
			{
				perror("send not real singal failed.\n");
				exit (-3);
				
			}
			printf("sending not real signal success\n");
		}

		mys.sival_int = 300;
		for (i = 0; i< 3; i++)
		{
			
			ret = sigqueue(getppid(), SIGRTMIN, mys);
			if ( ret != 0)
			{
				perror("send real singal failed.\n");
				exit (-3);
				
			}
			printf("sending  real signal success\n");
		}
		kill(getppid(), SIGUSR1);
	}
	
	while(1)
	{
		pause();
	}
	
	printf("main end.\n"); 
	return 0;
}
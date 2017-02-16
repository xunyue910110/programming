       #include <sys/types.h>          /* See NOTES */
       #include <sys/socket.h>
       #include <netinet/in.h>
       #include <netinet/ip.h> /* superset of previous */
	   #include <stdio.h>
	   #include <stdlib.h>
	   #include <string.h>
	   #include <signal.h>

/*
       int socket(int domain, int type, int protocol);
	   int bind(int sockfd, const struct sockaddr *addr,
                socklen_t addrlen);

*/

void handler(int sig)
{
	printf("child recived sig : %d\n", sig);
	fprintf(stdout, "child process die.\n");
	exit (0);
}
int main()
{
	int sockfd;
	int connsockfd;
	int ret;
	
	signal(SIGUSR1, handler);
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
	{
		perror("socket error");
		return -1;
	}
	
	
	struct sockaddr_in myadd;
	myadd.sin_family = AF_INET;
	myadd.sin_port = htons(8888);
	myadd.sin_addr.s_addr = inet_addr("192.168.56.6");
	//myadd.sin_addr.in_addr = INADDR_ANY;
	
	/* int setsockopt(int sockfd, int level, int optname,
                      const void *optval, socklen_t optlen); */
	int optval = 1;
	if ( setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
	{
		perror("setsockopt error");
		return -1;
	}
	ret = bind (sockfd, (struct sockaddr *)&myadd, sizeof(myadd));
	if (ret == -1)
	{
		perror("bind error");
		return -1;
	}
	
	/* int listen(int sockfd, int backlog); */
	ret = listen(sockfd, 5);
	if (ret == -1)
	{
		perror("listen error");
		return -1;
	}
	
	/* int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen); */
	struct sockaddr_in peeradd;
	int len = sizeof(peeradd);
	
	
	
	while ( 1)
	{
		connsockfd = accept(sockfd,  (struct sockaddr *)&peeradd, &len);
		if (connsockfd == -1)
		{
			perror("accept error");
			close(sockfd);
			close(connsockfd);
			return -1;
		}
		printf("peer ip address: %s, port: %d\n", inet_ntoa(peeradd.sin_addr), ntohs(peeradd.sin_port));
		
		int pid = fork();
		if (pid == -1)
		{
			perror("fork error");
			exit(-1);
		}
		if ( pid == 0)
		{
			close(sockfd);
			char sendbuf[1024] = {0};
			int nbytes = 0;
			memset(sendbuf, 0, sizeof(sendbuf));
			while (fgets(sendbuf, sizeof(sendbuf), stdin) != NULL)
			{
				nbytes = write(connsockfd, sendbuf, strlen(sendbuf));
				if (nbytes == -1)
				{
					close(connsockfd);
					perror("write error");
					exit(-1);
				}
				
			}
			
		}
		if ( pid > 0)
		{
			char recvbuf[1024] = {0};
			int nbytes = 0;
			while (1)
			{
				memset(recvbuf, 0, sizeof(recvbuf));
				nbytes = read(connsockfd, recvbuf, sizeof(recvbuf));
				if  (nbytes == -1)
				{
					perror("read error");
					exit(-1);
				}
				if (nbytes == 0)
				{
					fprintf(stdout, "peer closed.");
					kill (pid, SIGUSR1);
					sleep(1);
					fprintf(stdout, "parent process die.\n");
					exit(0);
				}
				fprintf(stdout, recvbuf);
			}
		}
		
		
	}
	
	close(connsockfd);
	close(sockfd);
	
	return 0;
}
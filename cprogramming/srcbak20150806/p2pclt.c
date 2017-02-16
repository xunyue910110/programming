       #include <sys/types.h>          /* See NOTES */
       #include <sys/socket.h>
       #include <netinet/in.h>
       #include <netinet/ip.h> /* superset of previous */
	   #include <stdio.h>
	   #include <string.h>
	   #include <stdlib.h>
	   #include <signal.h>
	   
void handler(int sig)
{
	printf("parent recived sig : %d\n", sig);
	sleep(1);
	fprintf(stdout, "parent process die.\n");
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
	
	/*        int connect(int sockfd, const struct sockaddr *addr,
                   socklen_t addrlen);
	*/
	
	struct sockaddr_in srvadd;
	srvadd.sin_family = AF_INET;
	srvadd.sin_port = htons(8888);
	srvadd.sin_addr.s_addr = inet_addr("192.168.56.6");
	//srvadd.sin_addr.in_addr = INADDR_ANY;
	ret = connect (sockfd, (struct sockaddr *)&srvadd, sizeof(srvadd));
	if (ret == -1)
	{
		perror("bind error");
		return -1;
	}
	
	int pid ;
	pid = fork();
	if (pid == -1)
	{
		close(sockfd);
		perror("fork error");
		exit(-1);
	}
	
	if (pid == 0)
	{
		char recvbuf[1024] = {0};
		int nbytes = 0;
		while (1)
		{
			memset(recvbuf, 0, sizeof(recvbuf));
			nbytes = read(sockfd, recvbuf, sizeof(recvbuf));
			if  (nbytes == -1)
			{
				perror("read error");
				exit(-1);
			}
			if (nbytes == 0)
			{
				fprintf(stdout, "peer closed.");
				kill(getppid(), SIGUSR1);
				fprintf(stdout, "child process die.\n");
				exit(0);
			}
			fprintf(stdout, recvbuf);
		}
		
	}
	
	if (pid > 0)
	{
		char sendbuf[1024] = {0};
		int nbytes = 0;
		memset(sendbuf, 0, sizeof(sendbuf));
		while (fgets(sendbuf, sizeof(sendbuf), stdin) != NULL)
		{
			nbytes = write(sockfd, sendbuf, strlen(sendbuf));
			if (nbytes == -1)
			{
				close(sockfd);
				perror("write error");
				exit(-1);
			}
			
		}
	}
	close(sockfd);
	return 0;
}	   
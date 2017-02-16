       #include <sys/types.h>          /* See NOTES */
       #include <sys/socket.h>
       #include <netinet/in.h>
       #include <netinet/ip.h> /* superset of previous */
	   #include <stdio.h>

/*
       int socket(int domain, int type, int protocol);
	   int bind(int sockfd, const struct sockaddr *addr,
                socklen_t addrlen);

*/
int main()
{
	int sockfd;
	int connsockfd;
	int ret;
	
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
	
	/* int listen(int sockfd, int backlog); */
	
	
	//char buf[1024] = {"good morning."};
	char buf[] = "good morning.";
	
	char sendbuf[1024];
	char recvbuf[1024];
	while (fgets(sendbuf, sizeof(sendbuf), stdin) != NULL)
	{
		int ret = write(sockfd, sendbuf, strlen(sendbuf));
		if ( ret == -1)
		{
			perror("read error");
			return -1;
		}
		if ( ret == 0)
		{
			perror("peer socket closed.");
			return -1;
		}
		
		read(sockfd, recvbuf, sizeof(recvbuf));
		fprintf(stdout, recvbuf);
		memset(sendbuf, 0, sizeof(sendbuf));
		memset(recvbuf, 0, sizeof(recvbuf));
		
	}
	
	close(sockfd);
	return 0;
}
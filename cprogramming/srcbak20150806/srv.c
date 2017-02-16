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
			char buf[1024] = {0};
			
			while (1)
			{
				memset(buf, 0 , sizeof(buf));
				int ret = read(connsockfd, buf, sizeof(buf));
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
				fprintf(stdout, buf);
				write(connsockfd, buf, ret);	
			}
			
		}
		if ( pid > 0)
		{
			close(connsockfd);
		}
		
		
	}
	
	close(connsockfd);
	close(sockfd);
	
	return 0;
}
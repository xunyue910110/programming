#include	"unp.h"

void
str_echo(int sockfd)
{
	ssize_t		n;
	char		buf[MAXLINE];
    long x;

again:
	while ( (n = read(sockfd, &x, sizeof(x))) > 0)
    {
        printf("%lx\n", x);
		Writen(sockfd, &x, sizeof(x));
    }

	if (n < 0 && errno == EINTR)
		goto again;
	else if (n < 0)
		err_sys("str_echo: read error");
}

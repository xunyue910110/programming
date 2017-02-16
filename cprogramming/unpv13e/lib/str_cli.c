#include	"unp.h"

long htonl64(long );
void
str_cli(FILE *fp, int sockfd)
{
	char	sendline[MAXLINE], recvline[MAXLINE];

	while (Fgets(sendline, MAXLINE, fp) != NULL) {
        long a = 0x1234567890443322;
        printf("%lx\n", a);
        a = htonl64(a);
        writen(sockfd, &(a), sizeof(a));
		//Writen(sockfd, sendline, strlen(sendline));
		long b;
		read(sockfd,  &b, sizeof(b));
        b = htonl64(b);
        printf("%lx\n", b);

        
		//if (Readline(sockfd, recvline, MAXLINE) == 0)
		//	err_quit("str_cli: server terminated prematurely");

		//Fputs(recvline, stdout);
	}
}

long htonl64(long data)
{
   char *p1 =(char*) &data;
    char *p2 =(char*) (&data+1); 
    p2--;
    char a;
    while (p2 > p1)
    {
        a = *p1; *p1 = *p2; *p2 = a;
        p1 ++;
        p2 --;
    }
    return data;
}

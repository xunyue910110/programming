#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int inet_aton(const char *cp, struct in_addr *inp);
in_addr_t inet_addr(const char *cp);
char *inet_ntoa(struct in_addr in);
int main(int argc, char *argv[])
{
	char *pip = "192.168.56.6";
	struct in_addr myipaddr;
	inet_aton(pip, &myipaddr);
	printf("%x\n", myipaddr.s_addr);
	
	printf("%x\n",inet_addr(pip));
	printf("%s\n", inet_ntoa(myipaddr));
	return 0;
}
	   

int main01(int argc, char *argv[])
{
	unsigned int i = 0x12345678;
	
	char *p = (char *)&i;
	
	if (p[0] ==  0x78)
	{
		printf("little edian.\n");
		
	}
	else 
	{
		printf("big edian.\n");
	}
	
	unsigned int r = htonl(i);
	printf("%x\n", *(char *)&r);
	return 0;
}
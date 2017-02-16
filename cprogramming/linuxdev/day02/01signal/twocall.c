
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int getMem(char *p, char *meml, int *len)
{
	if (meml != NULL)
	{
		memcpy(meml, "abc")
	}
	len = 10;
}

int main()
{
	int len = 0;
	char *p = NULL:
	getMem("aaa", NULL, &len);
	p = (char *)malloc(len);
	getMem("aaa", p, &len);
}
#include <stdio.h>

int main()
{
	char buffer[2];
	
	while((fgets(buffer,2, stdin)) != NULL)
	{
		fputs(buffer, stdout);
	}
	return 0;
}
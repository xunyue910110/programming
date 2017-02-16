#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main()
{
	pid_t pid;
	char buffer[1024];
	int status;
	int ret;

	printf("%% ");
	while( (fgets(buffer, 1024, stdin)) != NULL)
	{
		char *argv[10];
		char *p = buffer;
		char *p2;
		int i = 0;
		int j = 0;
		if(buffer[strlen(buffer)-1] == '\n')
			buffer[strlen(buffer)-1] = '\0';
		
		//char *argv[10] = {"ls", "-lrt", NULL};
		i = 0;
		while(*p==' ') p++;
		while(p2 = strchr(p, ' ') )
		{
			*p2= '\0';
			argv[i++] = p;
			++p2;
			p=p2;
			while(*p==' ') p++;
		}
		argv[i] = p;
		argv[++i] = NULL;
		
		for(j = 0; argv[j]!= NULL; j++)
		{
			fprintf(stderr, "%s\n", argv[j]);
		}
		pid = fork();
		if (pid < 0)
		{
			return -1;
		}
		if (pid == 0)
		{
			ret = execvp(argv[0], argv);
			
			if (ret == -1)
			{
				perror("execlp error");
				return -1;
			}
		}
		else 
		{
			waitpid(pid, &status, NULL);
		}
		printf("%% ");
	}
	
	return 0;
}
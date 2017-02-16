#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

/*
DIR *opendir(const char *name);
DIR *fdopendir(int fd);
*/

/*
       #include <dirent.h>

       struct dirent *readdir(DIR *dirp);
	   */
void usage()
{
	printf("usage: ls dirname");
	exit(-1);
}
int isdir(const char *filename)
{
	struct stat ms;
	int ret = stat(filename, &ms);
	if (ret == -1)
	{
		return 0;
	}
	return S_ISDIR(ms.st_mode);
}
void rescureDir(const char* dirname, int level)
{
	DIR* pd = opendir(dirname);
	struct dirent * pdirent;
	char pathname[1024] ;
	int i;
	if (pd == NULL)
	{
		perror("opendir error");
		return ;
	}
	
	while((pdirent=readdir(pd)) != NULL)
	{
		strcpy(pathname, dirname);
		strcat(pathname, "/");
		strcat(pathname, pdirent->d_name);
		if (strcmp(pdirent->d_name, ".")&&strcmp(pdirent->d_name, ".."))
		{
			/*for(i = 0; i <level; i++)
			{
				printf("-");
			}*/
			printf("%s\n", pathname);
		}
		else{
			continue ;
		}
		

		//printf("path name: %s\n", pathname);
		if (strcmp(pdirent->d_name, ".")&&strcmp(pdirent->d_name, "..")&& isdir(pathname))
		{
			
			rescureDir(pathname, ++level);
		}
	}
	
	closedir(pd);
}
int main(int argc, char *argv[])
{
	const char *dirname = NULL;
	DIR *pd;
	
	
	
	if (argc != 2 )
	{
		usage();
	}
	
	dirname = argv[1];
	rescureDir(dirname,0);
	
	return 0;
}
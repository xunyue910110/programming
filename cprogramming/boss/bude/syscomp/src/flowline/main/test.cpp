#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

int main()
{

   int fd = ::open("/gboss/mddms/work/src/flowline/main/temp/o0770cP0fG0Bn50008rpc4.tmp.append",O_CREAT|O_APPEND,0);

   printf("fd=%d\n",fd);
   ::close(fd);

}

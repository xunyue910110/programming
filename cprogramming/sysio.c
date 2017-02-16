#include <stdio.h>
#include <fcntl.h>

int main()
{

    //FILE *fp = fopen("sysio.dat", "wb");
    int fd = open("sysio.dat", O_WRONLY|O_CREAT|O_EXCL, 0666);
    
    //
    //
    //
    unsigned int i;
    for (i = 0 ; i < 100000; i++)
   //     fwrite(&i, sizeof(i), 1, fp);
        write(fd, &i, sizeof(i));
    if (fd)
    close(fd);
    return 0;
}

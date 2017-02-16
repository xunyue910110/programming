
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main()
{
    int fd1 = open("same.txt", O_RDWR|O_CREAT|O_EXCL|O_TRUNC|O_APPEND, 0666);
    //
    printf("fd1 = %d\n", fd1);
    int fd2 = open("same.txt", O_RDWR|O_CREAT|O_EXCL|O_TRUNC|O_APPEND, 0666);
    //
    printf("fd2 = %d\n", fd2);

    const char *text = "Hello World.";
    if (write( fd1, text, strlen(text)) == -1 )
    {

    }
    if ( -1 == lseek( fd1, -7, SEEK_CUR))
    {

    }
    text = "Linux";
    if ( write (fd2, text, strlen(text)) == -1 )
    {

    }
    text = "XXXXX";
    if ( write (fd1, text, strlen(text)) == -1 )
    {

    }
    return 0;
}

#include <stdio.h>
#include <fcntl.h>

int main(int argc, char*argv[1])
{
    size_t i;
    int fd = open("wlock.txt", O_WRONLY|O_CREAT|O_APPEND, 0666);
    if ( fd == -1)
    {
        perror("open");
        return -1;
    }
    for (i = 0; argv[1][i]; ++i)
    {
        if (write(fd, &argv[1][i], sizeof(argv[1][i])) == -1)
        {   
            perror("write");
            return -1;
        }
        printf("%#x\n", (unsigned char)argv[1][i]) ;
        sleep(1);
    }
}

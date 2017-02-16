#include <unistd.h>
#include <stdio.h>


int main()
{
    if ((lseek(STDIN_FILENO, 0, SEEK_CUR) ) == -1)
    {
        perror("lseek error");
        return -1;
    }
    fprintf(stderr,"can seek\n");
    return 0;
}

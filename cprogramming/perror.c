#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
//extern char *sys_errlist[];
//extern int sys_nerr;
//extern int errno;
int main(int argc, char *argv[])
{
    int n_char = 0, buffer[10];
    printf("n_char = %d \t errno = %d \n", n_char, errno);
    n_char = write(1, "Enter a word ", 14);
    printf("\nn_char = %d\n",n_char);
    n_char = read (0, buffer, 10);
    printf("\nn_char = %d \t errno = %d \n", n_char, errno);

    if (n_char == -1)
    //    perror(argv[0]), exit(1);
        printf("%s\n",strerror(errno)), exit(1);
    n_char = write(0, buffer, n_char);
/*
    int i = 0;
    for (i = 1; i < sys_nerr; i++)
        printf("%d: %s\n",  i, sys_errlist[i]);
*/
    
   /* 
    char buf[16];
    for (errno = 1; errno < 127; errno++)
    {
        sprintf(buf, "%3d ", errno);
        perror(buf);
    }
    */
    return 0;
}


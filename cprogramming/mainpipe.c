#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

void client(int, int);
void server(int, int);

int main()
{
    int pipe1[2], pipe2[2];
    pid_t childpid;

    pipe(pipe1);
    pipe(pipe2);

    if ((childpid = fork()) == 0 )
    {
        close(pipe1[1]);
        close(pipe2[0]);
        server(pipe1[0], pipe2[1]);
        exit(0);
    }

    close(pipe1[0]);
    close(pipe2[1]);
    client(pipe2[0], pipe1[1]); 
    waitpid(childpid, NULL, 0);
    exit(0);
}

void client(int readfd, int writefd)
{
    size_t len;
    ssize_t n;
    char buff[1024];
    fgets(buff, 1024, stdin);
    len = strlen(buff);
    if (buff[len-1] == '\n')
        len--;
    write(writefd, buff, len);

    while( (n = read(readfd, buff, 1024)) > 0)
    {
        write(STDOUT_FILENO, buff, n);
    }
}

void server(int readfd, int writefd)
{
    int fd;
    ssize_t n;
    char buff[1024+1];
    
    if ( (n = read(readfd, buff, 1024) ) == 0)
        exit(-1);
    buff[n] = '\0';
    
    if (( fd = open(buff, O_RDONLY)) < 0)
    {
        snprintf(buff+n, sizeof(buff) -n , ": can't open, %s\n", strerror(errno));
        n = strlen(buff);
        write(writefd, buff, n); 
    }
    else
    {
        while( (n= read(fd, buff, 1024)) > 0)
        {
            write(writefd, buff, n);
        }
        close(fd);
    } 
}
    

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#define FIFO1 "/tmp/fifo.1"
#define FIFO2 "/tmp/fifo.2"

void client(int, int);
void server(int, int);

int main()
{
    int readfd, writefd;
    pid_t childpid;

    if (( mkfifo(FIFO1, 0666) <0))
        perror(""), exit(-1);
    if (( mkfifo(FIFO2, 0666) <0))
        perror(""), unlink(FIFO1),exit(-1);
    if ((childpid = fork()) == 0 )
    {
        readfd = open(FIFO1, O_RDONLY, 0);
        writefd = open(FIFO2, O_WRONLY, 0);
        server(readfd, writefd);
        exit(0);
    }

    writefd = open(FIFO1, O_WRONLY, 0);
    readfd = open(FIFO2, O_RDONLY, 0);
    client(readfd, writefd); 
    waitpid(childpid, NULL, 0);
    close(readfd);
    close(writefd);
    unlink(FIFO1);
    unlink(FIFO2);
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
    

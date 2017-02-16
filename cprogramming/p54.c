#include "local.h"

void main(void)
{
    int n, privatefifo, publicfifo;
    static char buffer[PIPE_BUF];
    struct message msg;

    sprintf(msg.fifo_name, "/tmp/fifo%d", getpid());

    if (mknod(msg.fifo_name, S_IFIFO | 0666, 0) < 0)
        perror("msg.fifo_name"), exit(1);
    
    if ((publicfifo = open(PUBLIC, O_WRONLY)) == -1)
        perror(PUBLIC), exit(2);

    while (1)
    {
        write(fileno(stdout), "\ncmd>", 6);
        memset(msg.cmd_line, 0x0, sizeof(msg.cmd_line));
        n = read(fileno(stdin), msg.cmd_line, B_SIZ);
        //msg.cmd_line[n] = '\0';
        
        if ( !strncmp("quit", msg.cmd_line, n-1))
            break;
        write(publicfifo, (char*)&msg, sizeof(msg));

        if ((privatefifo = open(msg.fifo_name, O_RDONLY)) == -1)
            perror(msg.fifo_name), exit(3);

        while ((n=read(privatefifo, buffer, PIPE_BUF)) > 0)
            write(fileno(stderr), buffer, n);
        close(privatefifo);
    }
    close(publicfifo);
    unlink(msg.fifo_name);
}

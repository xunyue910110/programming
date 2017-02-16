#include "dao.h"
#include "bank.h"
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <unistd.h>

#include <fcntl.h>
#define ID_FILE "acc.dat"

int generateid()
{
    static int x = 100000;
    if ( access(ID_FILE, F_OK) == -1)
    {
        int fd = open(ID_FILE, O_WRONLY|O_CREAT|O_EXCL, 0666);
        if (fd == -1)
        perror("open"),exit(-1);
        write(fd, &x, sizeof(x));
        close(fd);
        return x;
    }

    int fd = open(ID_FILE, O_RDWR);
    if (fd == -1 ) perror("open"),exit(-1);
    /* add write lock */
    read(fd, &x, sizeof(x));
    x++;
    lseek(fd, 0 , SEEK_SET);
    write(fd, &x, sizeof(x));   
    /* unlease write lock */
    close(fd);
    return x;
}

int createUser(struct Account acc)
{
    char filename[20] = { 0};
    sprintf(filename, "%d.dat", acc.id);
    int fd = open(filename, O_WRONLY|O_CREAT|O_EXCL);
    if (fd == -1) perror("open"), exit(-1);
    
    if (write(fd, &acc, sizeof(acc)) < 0)
    exit(-1);
    close(fd);
    //printf("user created successfully.\n")
    return 0;
}

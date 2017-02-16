#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <unistd.h>

#define SHM_SIZE 30
extern etext, edata, end;

int main(void)
{
    pid_t pid;
    int shmid;
    char c, *shm, *s;

    if (( shmid = shmget(IPC_PRIVATE, SHM_SIZE, IPC_CREAT|0666)) < 0)
        perror("shmget faild"), exit(1);
    if (( shm = (char*) shmat(shmid, 0, 0)) == (char *) -1)
        perror("shmat: parent"), exit(2);
    printf("Addresses in parent\n\n");
    printf("shared mem: %X etext; %X edata; %X end: %X\n\n",
        shm, &etext, &edata, &end); 
    s = shm;
    for ( c = 'A'; c <= 'Z'; ++c)
        *s++ =c;
    *s = '\0';

    printf("In parent before fork, memory is: %s\n", shm);
    sleep(5);
    pid = fork();
    switch (pid ) {
        case -1:
            perror( "fork");
            exit(3);
        case 0:
            sleep(10);
            printf ( "Inchild after fork, memory is: %s\n", shm);
            for (; *shm; shm++)
                *shm += 32;
            shmdt(shm);
            exit(0);
        default:
            sleep(15);
            printf("\nIn parent after fork, memory is: %s\n", shm);
            printf("Parent removing shared memory\n");
            shmdt(shm);
            shmctl(shmid, IPC_RMID, (struct shmid_ds*) 0);
            exit(0);
    }
}


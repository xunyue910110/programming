#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/ipc.h>

int main()
{
    int key = ftok(".", 100);
    if (key == -1) perror("ftok"), exit(-1);

    int shmid = shmget(key, 4, 0666|IPC_CREAT|IPC_EXCL);
    if ( shmid == -1 ) perror("shmget"), exit(-1);

    printf("shm create completed.\n");
    void *p = shmat(shmid, 0, 0);
    if ( p == (void*)-1 ) perror("shmat"), exit(-1);
    int *pi = p; 
    *pi = 100;
    shmdt(pi); 
    //shmctl(shmid, IPC_RMID, 0);
}

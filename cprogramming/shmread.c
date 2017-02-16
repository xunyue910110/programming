#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main()
{
    int key = ftok(".", 100);
    if (key == -1) perror("ftok"), exit(-1);
    printf("key is %x\n", key);
    int shmid = shmget(key, 4, 0);
    printf("shmid is %x\n", shmid);
    if (shmid == -1) perror("shmget"), exit(-1);
    printf("shmget successful.\n");
    void *p  = shmat(shmid, 0, 0);
    int *pi = p;
    printf("%d\n", *pi); 
    shmdt(p);
    shmctl (shmid, IPC_RMID, 0);
}

#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <signal.h>

void sigexit(int signo)
{
    
    key_t key = ftok(".", 100);
    int semid = semget(key, 1, IPC_CREAT|0666);
    if (semid == -1) perror("semget"), exit(-1);
    semctl(semid, 0, IPC_RMID);
    exit(0);
}
int main()
{
    signal(SIGINT, sigexit);
    key_t key = ftok(".", 100);
    int semid = semget(key, 1, IPC_CREAT|0666);
    if (semid == -1) perror("semget"), exit(-1);

    semctl(semid, 0, SETVAL, 5);
    printf("semaphore created successfully.\n");
    while(1);
    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
    key_t key = ftok(".", 100);
    int semid = semget(key, 0, 0);
    if (semid == -1) perror("semget"),exit(-1);

    int i ;
    for (i = 0; i< 10; i++)
    {
        pid_t pid = fork();
        if (pid == 0)
        {
            printf("process %d start.\n", i+1);
            struct sembuf op;
            op.sem_num = 0;
            op.sem_op = -1; 
            op.sem_flg = 0;
            semop(semid, &op, 1);
            printf("process %d get the sem.\n", i+1);
            sleep(1);
            op.sem_op = +1;
            semop(semid, &op, 1);
            printf("process %d release the sem.\n",i+1);
            exit(0);
        }
    }
    
    for (i= 0; i < 10; i++)
    {
        wait(NULL);
    }
}

#include "bank.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

static int msgid1;
static int msgid2;

void init()
{
    msgid1 = msgget(key1, IPC_CREAT|IPC_EXCL|0666);
    if (msgid1 == -1) perror("msgget"), exit(-1);

    printf("msg queue1 created successfully.\n");
    msgid2 = msgget(key2, IPC_CREAT|IPC_EXCL|0666);
    if (msgid1 == -1) perror("msgget"), exit(-1);

    printf("msg queue2 created successfully.\n");
    
}

void sig_exit(int signo)
{
    sleep(2);
    msgctl(msgid1, IPC_RMID, 0);
    msgctl(msgid2, IPC_RMID, 0);
    printf("all msg queues deleted successfully.\n");
}
void start()
{
   sleep(2);
    pid_t open_pid = vfork();
    if (open_pid  == -1) perror("vfork"), exit(-1);

    if (open_pid == 0)
        execl("open", "open", NULL);
    else 
    {
        printf("wait child process completed.\n");
        waitpid(open_pid, 0,0);
    } 
}


int main()
{
    printf("Quit, CTRL+C\n");
    signal(SIGINT, sig_exit);
    init();
    start();
    return 0;
}

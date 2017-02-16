#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>

struct Msg{

long mtype;
char buf[20];
};

void handle( int signo)
{
    key_t key = ftok(".", 100);
    int msgid = msgget(key, 0);
    if (msgid == -1) perror("msgget"), exit(-1);
    struct msqid_ds msq; 
    if ( msgctl(msgid, IPC_STAT,  &msq) == -1 )
    {
         perror("msgctl"), exit(-1);
    }
    if ( msq.msg_qnum == 0) 
    {
        msgctl(msgid, IPC_RMID, 0);
        printf("msg queue deleted.\n");
        exit(0); 
    }
}
int main()
{
    key_t key = ftok(".", 100);
    int msgid = msgget(key, IPC_CREAT|0666);
    if (msgid == -1) perror("msgget"), exit(-1);
    signal(SIGINT, handle);
    struct Msg msg1, msg2;
    msg1.mtype = 1;
#include <string.h>
    strcpy(msg1.buf, "zhangfei");
    msg2.mtype = 2;
    strcpy(msg2.buf, "guanyu");

    msgsnd(msgid, &msg1, sizeof(msg1.buf), 0);
    msgsnd(msgid, &msg2, sizeof(msg2.buf), 0);
    while(1) sleep(1);
}

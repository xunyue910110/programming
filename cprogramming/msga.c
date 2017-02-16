#include <sys/ipc.h>
#include <string.h>
#include <stdio.h>
#include <sys/msg.h>

int main()
{
    int key = ftok(".", 200);
    int msgid = msgget(key, 0666|IPC_CREAT);
    if (msgid == -1 ) perror("msgget"), exit(-1);
    printf("msg create successfully.");
    int res = msgsnd(msgid, "hello", 5, 0) ; //0 - block
    if (res == -1 ) perror("msgsnd"), exit(-1);
    printf("send ok.\n"); 
}

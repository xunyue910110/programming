#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>

struct Msg{

long mtype;
char buf[20];
};

int main()
{
    key_t key = ftok(".", 100);
    int msgid = msgget(key, 0);
    if (msgid == -1) perror("msgget"), exit(-1);

    struct Msg msg1, msg2;
    int res = msgrcv(msgid, &msg1, sizeof(msg1.buf), 2, 0);
    printf("received  %d bytes %s\n", res, msg1.buf);
    res = msgrcv(msgid, &msg2, sizeof(msg2.buf), 1, 0);
    printf("received  %d bytes %s\n", res, msg2.buf);

}

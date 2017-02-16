#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>

int main()
{
    int key = ftok(".", 200);
    if (key == -1 ) perror("ftok"), exit(-1);

    int msgid = msgget(key, 0);
    if (msgid == -1) perror("msgget"), exit(-1);

    printf("msg get successfully.\n");
    char text[6]  = {};
    int res = msgrcv(msgid,  text, 6, 0, 0);
    if (res == -1) perror("msgrcv"), exit(-1);
    printf("received %d bytes data: %s\n", res, text);
}

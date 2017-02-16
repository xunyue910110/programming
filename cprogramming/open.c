#include "bank.h"
#include "dao.h"

#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


int main()
{
    int msgid1 = msgget(key1, 0);
    int msgid2 = msgget(key2, 0);
    
while(1)
{
     struct Msg msg;
    struct Account accMove;
    if (-1 == msgrcv(msgid1, &msg, sizeof(msg.acc), 0,0))
    break; 
    if (msg.mtype == M_OPEN)
    {
        int id = generateid();
        accMove = msg.acc;
        accMove.id = id;
        if ( createUser(accMove) == -1)
        {
            printf("open account failed.\n"); 
            msg.mtype = M_FAILED;
        }
        else {
            printf("open account success.\n");
            msg.mtype = M_SUCCESS; 
        }
        msgsnd(msgid2, &msg, sizeof(msg.acc), 0);
    }
    
}
return 0;
}

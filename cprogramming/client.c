#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "bank.h"

static int msgid1;
static int msgid2;

void getID()
{
    msgid1 = msgget(key1, 0);
    if (msgid1 == -1) perror("msgget"), exit(-1);

    msgid2 = msgget(key2, 0);
    if (msgid2 == -1) perror("msgget"), exit(-1);

    
}

void createUser()
{
    struct Account acc;
    char password[10];
    printf("please input username:\n");
    scanf("%s", acc.name);
    while(1){
    printf("please input  password:\n");
    scanf("%s", password);
    printf("please re input password: \n");
    scanf("%s", acc.password);
    if ( strcmp(acc.password, password) )
    {
        printf("2 passwords mismatch, pls retry\n");
        continue;
    }
    else
    {
        break;
    }
    }
    printf("please input amount:\n");
    scanf("%lf", &acc.balance);
    struct Msg msg = {M_OPEN, acc};
    getID();
    msgsnd(msgid1, &msg, sizeof(msg.acc), 0);
    msgrcv(msgid2, &msg, sizeof(msg.acc), 0, 0);

    if (msg.mtype == M_SUCCESS)
    {
        printf("open account successfully.\n");
    }
    else
    {
        printf("open account failed.\n");
    }
}

void destroyUser()
{
}
void mainPage()
{
    while(1){
    printf(" welcome mini ATM \n");
    printf("1 open account\n");
    printf("2 destroy account\n");
    printf("0 exit\n");
    printf("please input the choose:\n");
    int num = 0;
    scanf("%d", &num);
    switch(num){
        case 1: createUser(); break;
        case 2: destroyUser(); break;
        case 0: printf("Thankyout, bye.\n"); return; 
        default: printf("input error\n");
    }
    }
}

int main(int argc, char *argv[])
{
    mainPage();
    return 0;
}

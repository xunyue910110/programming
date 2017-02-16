#include "msglocal.h"

int main(int argc, char * argv[])
{
    int mid, n ;
    MESSAGE msg;
    void process_msg(char *, int);  
    if (argc != 3){
        fprintf(stderr, "Usage: %s msq_id &\n", argv[0]);
        exit(1);
    }

    mid = atoi(argv[1]);
    while (1)
    {
        if ( (n = msgrcv(mid, &msg, sizeof(msg)-sizeof(long), SERVER, 0)) == -1)
        {
            perror("Server: msgrcv");
            exit(2);
        }
        else if ( n == 0)
            break;
        else 
        {
            process_msg (msg.buffer, strlen(msg.buffer));
            msg.msg_to = msg.msg_fm;
            msg.msg_fm = SERVER;
        }
        if (msgsnd(mid, &msg, sizeof(msg)-sizeof(long), 0) == -1)
            perror("Server: msgsnd"), exit(3);
    }
    msgctl(mid, IPC_RMID, (struct msqid_ds*) 0);
    exit(0);
}

void process_msg(char *b, int len)
{
    int i;
    for(i = 0; i < len; i++)
        if ( isalpha(*(b+i)))
            *(b+i) = toupper(*(b+i));
}

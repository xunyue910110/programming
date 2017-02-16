#include "msglocal.h"

int main(void)
{
    key_t key;
    pid_t cli_pid;
    int mid, n;
    MESSAGE msg;
    static char m_key[10];

    cli_pid = getpid();
    if ((key = ftok(".", SEED)) == -1)
        perror("Client: key generation"), exit(1);

    if ((mid = msgget(key, 0)) == -1)
    {
        mid = msgget(key, IPC_CREAT|0660);
        switch(fork()){
            case -1:
                perror("Client: fork");
                exit(3);
            case 0:
                sprintf(m_key, "%d", mid);
                execlp("server", "server", m_key, "&", NULL);
                perror("Client: exec");
                exit(4);
        }
    }

    while(1)
    {
        msg.msg_to = SERVER;
        msg.msg_fm = cli_pid;
        write(fileno(stdout), "\ncmd>", 6);
        memset(msg.buffer, 0x0, BUFSIZ);
        n = read(fileno(stdin), msg.buffer, BUFSIZ);
        if (n == 0)
            break;
        if (msgsnd(mid, &msg, sizeof(msg)-sizeof(long), 0) == -1)
        {
            perror("Client: msgsend");
            exit(5);
        }
        if ((n= msgrcv(mid, &msg, sizeof(msg)-sizeof(long), cli_pid, 0)) != -1)
            write(fileno(stdout), msg.buffer, strlen(msg.buffer));
    }
    msgsnd(mid, &msg, 0, 0);
    exit(0);
}

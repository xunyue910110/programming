#ifndef __BANK_H
#define __BANK_H

extern const int key1;
extern const int key2;
/* message type macro definition */
#define M_OPEN 1
#define M_DESTROY 2
#define M_SAVE 3
#define M_TAKE 4
#define M_QUERY 5
#define M_TRANSF 6
#define M_SUCCESS 7
#define M_FAILED 8

/* account information */
struct Account {
    int id;
    char name[10];
    char password[10];
    double balance;
};

/* message definition */
struct Msg {
    long mtype;
    struct Account acc;
};


#endif

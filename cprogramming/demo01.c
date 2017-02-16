#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
struct user{
char account[10];
char password[10];
};

int main()
{
    sqlite3 *db = NULL;
    char *errmsg = NULL;
    struct user user1;
    printf("帐号:>");
    scanf("%s", user1.account);
    printf("密码:>");
    scanf("%s", user1.password);

    int re = sqlite3_open("./test.db", &db);
    if (re ) 
    {
        fprintf(stderr, "can not connect to the db: %s\n", sqlite3_errmsg(db));
        exit(1);
    }
    printf("connected.\n");
    
    char sql [100];
   // sql = "insert into users values(null, 'jack','zhang');";
    sprintf(sql, "insert into users values(null, '%s', '%s');", 
        user1.account, user1.password);
    re = sqlite3_exec(db, sql, 0, 0, &errmsg);
    if (re )
    {
        printf(" db insert failed: %s\n", errmsg);
        sqlite3_close(db);
        exit(1);
    }
    printf("insert success.\n");
    sqlite3_close(db);
}

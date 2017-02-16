#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int handler(void *data, int columns, char **column_value, char ** column_name)
{
    int i;
    printf("columns: %d\n", columns);
    for (i = 0; i < columns; i++)
    {
        printf("%s: %s\t",column_name[i], column_value[i]);
    }
    printf("\n");
    return 0;
}


int main()
{
    sqlite3 *db = NULL;
    char *errmsg = NULL;

    // step 1: create database.
    int re = sqlite3_open("./message.db", &db);
    if (re ) 
    {
        fprintf(stderr, "can not connect to the db: %s\n", sqlite3_errmsg(db));
        exit(1);
    }
    printf("connected.\n");
    
   // step 2: create tables  
    char sql[200] = "create table msg(msgid integer primary key autoincrement, msgtext text not null, id integer not null);";
    if (sqlite3_exec(db, sql, NULL, NULL, &errmsg))
    {
        printf("table create failed: %s\n", errmsg);  
        exit(1);
    }
    printf("table create success.\n");

    // step 3: insert data. 
    int  i = 0;
    for (i = 0; i < 20; i ++)
    {
        sprintf(sql, "insert into msg values(null, 'message%d', %d);", (i+1), i+1);
        printf("SQL:> %s\n", sql);
        if (sqlite3_exec(db, sql, NULL, NULL, &errmsg))
        {
            printf("insert failed: %s\n", errmsg);
            continue;     
        }  
        else
        {
            printf("insert success.\n");
        } 
    }
    
    // step 4: query data.
    strcpy(sql,  "select * from msg order by msgid;");
    if (sqlite3_exec(db, sql, handler, NULL, &errmsg))
    {
        printf("query failed:%s\n", errmsg);
    }
    else 
    {
        printf("query success.\n");
    }
    
    // step 5: close db.
    sqlite3_close(db);
}

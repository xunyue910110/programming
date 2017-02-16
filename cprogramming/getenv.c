/*
 * get the environment value by provided key , 
 * set the value to the buf and len.
 *
 */
#include <stdio.h>
#include <string.h>
extern char **environ;
int getenv(const char *key, char *buf, int *len)
{
   if (key == NULL || buf == NULL || len == NULL)
        return -1; 
   char ** penv = environ; 
   int keylen = strlen(key); 
    while( *penv)
    {
        if (strncmp(key, *penv, keylen) == 0 && ((*penv) [ keylen] == '='))
        {
            *len = strlen(*penv+keylen+1);
            strncpy(buf, *penv+keylen+1, *len+1);
            //buf[*len] = '\0';
            return 0; 
        }
        penv++;
    }
    return -1;
}

int main()
{
    char *env = "PATH";
    char buf[1024];
    int len;
    int ret = getenv(env, buf, &len);
    if (ret == -1)
    {
        printf("not found\n");
        return -1;
    }
    printf("the value length for the key: %d\n", len);
    printf("the value for the key: %s\n", buf); 
    return 0;
}
    

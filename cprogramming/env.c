
int main(int argc, char **argv, char **env)
{
#include <stdio.h>
#include <stdlib.h>
    extern char ** environ;
    printf("%p, %p\n", env, environ);
    char *value = getenv("LANG");
    printf("value=%s\n", value);
    putenv("VAR=ABC");
    printf("value=%s\n", getenv("VAR"));
    putenv("VAR=123");
    printf("value=%s\n", getenv("VAR"));
    setenv("VAR", "456", 0);
    printf("value=%s\n", getenv("VAR"));
    setenv("VAR", "789", 1);
    printf("value=%s\n", getenv("VAR"));
    return 0;
}

#include "errors.h"

int main(int argc, char *argv[])
{
    int seconds;
    char line[128];
    char message[64];

    while (1){
        printf ("Alarm> "); 
        if (fgets(line, sizeof(line), stdin) == NULL)
            exit(0);
        if (strlen(line) <= 1)
            continue;
        if (sscanf( line, "%d %64[^\n]", &seconds, message ) < 2)
        {
            fprintf(stderr, "Bad command\n");
            exit(1);
        }
        else{
            sleep(seconds);
            printf ("(%d) %s\n", seconds, message);
        }
        
    }
    return 0;
}

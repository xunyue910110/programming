#include <stdio.h>
#include <errno.h>
#include <string.h>
int main()
{
    FILE *file = fopen("/etc/passwd", "r");
    if ( file == NULL)
    {
        printf("%s\n", strerror(errno));
        perror("fopen");
        printf("%m\n");
    }
    
    if (file != NULL)
    fclose(file);
    return 0;
}

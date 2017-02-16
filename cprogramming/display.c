#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define shw_adr(id, i) printf("The id %s \t is at addr: %8X\n", id, &i)

extern int etext, edata, end;
char *cptr = "Hello World\n";
char buffer1[25];

int main(void)
{
    void showit(char* );
    int i =0;

printf("Adr etext: %8X\t Adr eata: %8X\t Adr end: %8X\n\n",
        &etext, &edata, &end);
shw_adr("main", main);
shw_adr("showit", showit);
shw_adr("cptr", cptr);
shw_adr("buffer1", buffer1);
shw_adr("i", i);
    strcpy(buffer1, cptr);
    write(1, buffer1, strlen(buffer1)+1);
    for (; i < 3; ++i)
        showit(cptr);
    return 0;
}

void showit(char* p)
{
    char *buffer2;
    shw_adr("buffer2", buffer2);
    if ((buffer2 =  malloc((strlen(p)+1))) != NULL)
    {
        strcpy(buffer2, p);
        printf("%s", buffer2);
        free(buffer2);
    }
    else
    {
        printf("Allocation failed.\n");
        exit(1);
    }
}

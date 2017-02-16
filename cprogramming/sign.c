#include <stdio.h>

#include <stdlib.h>
#include <string.h>

#define WORDMAX 100

int charcomp(char *x, char *y)
{
    return *x - *y;
}

int main()
{
    char word[WORDMAX], sig[WORDMAX];
    while( scanf("%s", word) != EOF)
    {
        strcpy(sig, word);
        qsort(sig, strlen(sig), sizeof(char), charcomp);
        printf("%s %s\n", sig, word);
    }
    return 0;
}

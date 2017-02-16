#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define LINES 200000
#define MAXWORD 100

int compar_words(const void *p, const void *q)
{
    return strcmp(*(char **)p, *(char **)q);
}

int main()
{
    char word1[100];
    char word2[100];
    int lines = 0;
    char* qqarr[200000] = {0};
    char *s = NULL;
    while(scanf("%s\t%s", word1, word2) != EOF)
    {
        s  = (char*)malloc(strlen(word1)+strlen(word2)+1);
        if (s == NULL)
            return -1;
        qqarr[lines] = s;
        sprintf(qqarr[lines], "%s\t%s" , word1,word2);
         
        lines++;
    }
   
    qsort(qqarr, lines, sizeof(char*), compar_words);
    int i =0;
    while (i < lines)
    {
        printf("%s\n", qqarr[i]);
        i++;
    }
    return 0; 
}

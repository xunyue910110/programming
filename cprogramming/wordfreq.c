#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct node *nodeptr;
typedef struct node {
    char *word;
    int count;
    nodeptr next;
}node;

#define NHASH 45000
#define MULT 31
nodeptr bin[NHASH];

void incword(char *s);
unsigned int hash(char *p)
{
    unsigned int h = 0;
    for (; *p ; p++)
        h = MULT * h + *p;
    return h % NHASH;
}

int main()
{
    int i ;
    char buf[50];
    for (i = 0; i < NHASH; i++)
        bin[i] = NULL;
    while (scanf("%s", buf) != EOF)
        incword(buf);
    nodeptr p;
    for (i = 0; i < NHASH; i++)
        for (p = bin[i]; p != NULL; p = p->next) 
            printf("%s %d\n", p->word, p->count);
    return 0;
}


void incword(char *s)
{
    int h = hash(s);
    nodeptr p;
    for ( p = bin[h] ; p != NULL; p=p->next)
    {
        if (strcmp(s, p->word) == 0)
        {
            p->count++;
            return;
        }
    }
    p =(nodeptr) malloc(sizeof(node));
    if (p == NULL)
        return ;
    p->count = 1;
    p->word = (char *) malloc(strlen(s) + 1);
    if (p->word == NULL)
        return;
    strcpy(p->word, s);
    p->next = bin[h];
    bin[h] = p;
}

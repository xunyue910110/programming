#include <stdio.h>
#include <stdlib.h>

typedef struct node* link;
struct node { int item; link next;};

int main(int argc, char *argv[])
{
    int i, N = atoi(argv[1]), M = atoi(argv[2]);
    
    link t = malloc (sizeof *t);
    link x = t;
    t->item = 1;
    t->next = t;
    link d;

    for (i = 2; i <= N; i++)
    {
        x->next = malloc (sizeof *x);
        x = x->next;
        x->next = t;
        x->item =i;
    }

    while (x != x->next)
    {
        for ( i = 1; i <M; i++)
            x = x->next;
        d = x->next;
        x->next = d ->next;
        printf("%d ", d->item);
        free(d); 
        N--;
    } 

    printf("%d ", x->item);
    return 0;
}

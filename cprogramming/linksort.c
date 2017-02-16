#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//#include <string.h>
//typedef struct node (* link);
typedef struct node { int item; struct node * next;} node , *link;
struct node heada, headb;

int main(int argc, char*argv[])
{
    int N = atoi(argv[1]);
    link t, u, x, a = &heada, b;
    int i;
    srand(time(NULL));

    for (i = 0, t=a; i <N; i++)
    {
        t->next = malloc (sizeof *t);
        t = t->next;
        t->next = NULL;
        t->item = rand() % 100;
    }

    b = &headb; b->next = NULL;
    for ( t= a->next; t!= NULL; t= u)
    {
        u = t->next;
        for ( x = b; x->next != NULL; x = x->next)
        {
            if ( x->next->item > t->item) break;
            
        }
        t->next = x->next;
        x ->next = t;
    }
    
    for ( b= (&headb )->next; b != NULL; b= b->next)
        printf ("%d " , b->item);
    return 0;
}


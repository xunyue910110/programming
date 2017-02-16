#include "myslist.h"
#include "myheader.h"

//typedef struct 

/* create a slist contains a header */
slist* slist_create()
{
    slist* header = (slist*)malloc(sizeof(slist));
    if (header == NULL)
    {
        perror("malloc"),exit(-1);
    }
    header->next = NULL;
    return header;
}

/* destroy the slist, free all node contains header */
/* all node must malloc, if not use slist_destroy2 */
void slist_destroy(slist* header)
{
   slist* p1 = header;
   slist* p2 = NULL;
    while(p1)
    {
        p2=p1->next;
        free(p1);
        p1=p2;
    }
}

void slist_destroy2(slist* header)
{
    free(header);
}

/* clear the slist, free the data node not contains the header */
/* all node must malloc , if not use slist_clear2 */
void slist_clear(slist *header)
{
    slist_destroy(header->next);
    header->next = NULL;
}

void slist_clear2(slist *header)
{
    header->next = NULL;
}
/* get the slist data node length */
int slist_length(slist *slist)
{

    int len = 0;
    typeof(slist) p1 = slist->next;
    while (p1) 
    {
         len++;
         p1 = p1->next;
    }
    return len;
}

/* get the previous node pointer */
slist * slist_findprevious(slist* list,  int pos)
{
    if ( pos < 0 || pos > slist_length(list))
    return NULL;

    slist *p1 = list;
    slist *p2 = NULL;
    while (pos)
    {
        p2 = p1->next;
        p1 = p2; 
        pos--;
    }    
    return p1;
}
 
/* insert the pointer to the list */ 
int slist_insert(slist*list, slist *pdata,  int pos)
{
    slist *p = slist_findprevious(list, pos);
    if ( p == NULL)
    {
        return -1;    
    }
    pdata->next = p->next;
    p->next = pdata;
    return 0; 
}

/* delete element at positions pos , 
 * return the pointer to the node*/
slist *slist_delete(slist*list, int pos)
{
    slist *p = slist_findprevious(list, pos);
    if ( p == NULL)
    {
        return NULL;    
    }
    slist *pd = p->next;
    p->next = p->next->next;
    return pd;
}


slist* slist_get(slist*list, int pos)
{
    slist *p = slist_findprevious(list, pos);
    if ( p == NULL)
    {
        return NULL;    
    }
    slist *pd = p->next;
    return pd; 
}

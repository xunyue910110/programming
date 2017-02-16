#ifndef __MYSLIST_H
#define __MYSLIST_H

typedef struct _tag_slistnode
{
   struct _tag_slistnode *next; 
}slist;

slist* slist_create();

void slist_destroy(slist*);
void slist_destroy2(slist*);
void slist_clear(slist*);
void slist_clear2(slist*);

int slist_length(slist*);
int slist_insert(slist*, slist*, int pos);

slist* slist_get(slist*, int pos);
slist* slist_delete(slist*, int pos);
slist* slist_findprevious(slist* slist,  int pos);

#endif

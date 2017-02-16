#include "FastDelegate.h"
using namespace fastdelegate;


//typedef FastDelegate<void (void)> Callback;

struct DelegateLink
{
    typedef FastDelegate<void (void)> Callback;
    DelegateLink *prev, *next;
    float order;
    CallBack fDelegate;

    DelegateLink(){}
    DelegateLink(const Callback& dlg):
        fDelegate(dlg) {}
    void insert(DelegateLink *node, float order)
    {
        DelegateLink * walk = next;
        while ( order >= walk->order &&
            walk->next != this)
            walk = walkl->next;
        if ( order >= walk->order) 
        {
            node ->prev = walk;
            node ->next = walk->next;
            walk ->next->prev = node;
            walk ->next = node;
        }
        else
        {
            node->prev = walk->prev;
            node ->next = walk;
            walk ->prev->next = node;
            walk->prev = node;
        }
        node ->order = order;
    }
    void unlink()
        {
        next->prev = prev;
        prev->next = next;
        }

};

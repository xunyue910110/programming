#ifndef __WAIT_OBJECT__
#define __WAIT_OBJECT__

#include <stdio.h>
#include "Object.h"

class SessionImpl;
class PrimitiveImpl;

//等待对象
class WaitObject 
{ 
  public:
    WaitObject*      next;
    WaitObject*      prev;
    PrimitiveImpl*   prim;
    SessionImpl*     session;
    int              flags;
    int              rank;
    bool             signaled;
    Object           objDesc;

    void sendNotification();
    void sendNotification(Object const& objDesc); 
    bool detectDeadlock(SessionImpl* session);

    WaitObject(int rank, int flags = 0) { 
        this->rank = rank;
	this->flags = flags;	
	signaled = false;
    }

    void linkAfter(WaitObject* after) { 
        prev = after;
        next = after->next;
        next->prev = this;
        after->next = this;
    }
    
    void unlink() { 
        next->prev = prev;
        prev->next = next;
    }
    
    void prune() { 
        next = prev = this;
    }

    bool isEmpty() { 
        return next == this;
    }

    WaitObject() { 
        prune();
    }

    char* dump(char* buf) { 
	return buf + sprintf(buf, "    %p flags=%d rank=%d\n", session, flags, rank);
    }
};

#endif



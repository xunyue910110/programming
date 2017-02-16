#define RIPC_IMPLEMENTATION

#include "SemaphoreImpl.h"
#include "WaitObject.h"
#include "Server.h"

void SemaphoreImpl::priorityWait(int rank)
{
    RIPC_CS(cs);
    if (count > 0) { 
	count -= 1;
    } else { 
	WaitObject wob(rank);
	server->waitNotification(wob, this);
    }	
}

bool SemaphoreImpl::priorityWait(int rank, unsigned timeout)
{
    RIPC_CS(cs);
    if (count > 0) { 
	count -= 1;
	return true;
    } else { 
	WaitObject wob(rank);
	server->waitNotificationWithTimeout(wob, this, timeout);
	return wob.signaled;
    }
}

void SemaphoreImpl::signal(int n)
{
    RIPC_CS(cs);
    if (n < 0) { 
	RIPC_THROW(InvalidParameterException);
    }
    while (n > 0 && !wobList.isEmpty()) { 	    
	wobList.next->sendNotification();
	wobList.next->unlink();
	n -= 1;
    }
    count += n;
}

void SemaphoreImpl::reset()
{
    RIPC_CS(cs);
    count = 0;
}
   
void SemaphoreImpl::deletePrimitive()
{
    server->deleteSemaphore(this);
}

char* SemaphoreImpl::dump(char* buf)
{
    buf = PrimitiveImpl::dump(buf);
    buf += sprintf(buf, "  Counter: %d\n", count);
    return buf;
}

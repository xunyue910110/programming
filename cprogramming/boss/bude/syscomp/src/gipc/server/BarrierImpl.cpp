#define RIPC_IMPLEMENTATION

#include "BarrierImpl.h"
#include "WaitObject.h"
#include "Server.h"


void BarrierImpl::priorityWait(int rank)
{
    RIPC_CS(cs);
    if (++nBlocked < nSessions) { 
	WaitObject wob(rank);
	server->waitNotification(wob, this);
    } else { 
        wakeupAll();
	nBlocked = 0;
	sequenceNo += 1;
    }
}

bool BarrierImpl::priorityWait(int rank, unsigned timeout)
{
    RIPC_CS(cs);
    bool result = true;
    int sequenceNo = this->sequenceNo;
    if (++nBlocked < nSessions) { 
	WaitObject wob(rank);
	server->waitNotificationWithTimeout(wob, this, timeout);
	if (!wob.signaled) { 
	    if (sequenceNo == this->sequenceNo) { 
		nBlocked -= 1;
	    }
	    result = false;
	}
    } else { 
        wakeupAll();
	nBlocked = 0;
	sequenceNo += 1;
    }
    return result;
}


void BarrierImpl::reset()
{
    RIPC_CS(cs);
    wakeupAll();
    nBlocked = 0;
    sequenceNo += 1;
}
   
void BarrierImpl::deletePrimitive()
{
    server->deleteBarrier(this);
}

char* BarrierImpl::dump(char* buf)
{
    buf = PrimitiveImpl::dump(buf);
    buf += sprintf(buf, "  Expected number of sessions: %d\n  Blocked number of sessions: %d\n", nSessions, nBlocked);
    return buf;
}

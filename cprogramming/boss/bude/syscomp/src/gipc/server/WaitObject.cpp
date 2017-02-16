#define RIPC_IMPLEMENTATION

#include "WaitObject.h"
#include "SessionImpl.h"
#include "Server.h"

void WaitObject::sendNotification()
{
    signaled = true;
    session->notify();
}

void WaitObject::sendNotification(Object const& objDesc) 
{ 
    this->objDesc = objDesc;
    sendNotification();
}

bool WaitObject::detectDeadlock(SessionImpl* session) { 
    return ((flags & Server::TIMED_WAIT) == 0) 
        ? prim->detectDeadlock(this, session) : false;
}

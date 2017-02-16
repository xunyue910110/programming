#define RIPC_IMPLEMENTATION

#include "SessionStub.h"

void LockStub::priorityExclusiveLock(int rank)
{ 
    Request  req;
    Response resp;
    req.opCode = Request::EXCLUSIVE_LOCK;
    req.objectId = id;
    req.rank = rank;
    session->sendAndVerify(req, resp);	
}
	
bool LockStub::priorityExclusiveLock(int rank, unsigned timeout) 
{
    Request  req;
    Response resp;
    req.opCode = Request::TIMED_EXCLUSIVE_LOCK;
    req.objectId = id;
    req.rank = rank;
    req.value = timeout;
    return session->sendAndCheckForOk(req, resp);	
}

void LockStub::prioritySharedLock(int rank)
{ 
    Request  req;
    Response resp;
    req.opCode = Request::SHARED_LOCK;
    req.rank = rank;
    req.objectId = id;
    session->sendAndVerify(req, resp);	
}
	
bool LockStub::prioritySharedLock(int rank, unsigned timeout)
{
    Request  req;
    Response resp;
    req.opCode = Request::TIMED_SHARED_LOCK;
    req.objectId = id;
    req.rank = rank;
    req.value = timeout;
    return session->sendAndCheckForOk(req, resp);	
}
	
void LockStub::unlock()
{ 
    Request  req;
    Response resp;
    req.opCode = Request::UNLOCK;
    req.objectId = id;
    session->sendAndVerify(req, resp);	
}	

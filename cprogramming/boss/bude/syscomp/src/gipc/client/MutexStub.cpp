#define RIPC_IMPLEMENTATION

#include "SessionStub.h"

void MutexStub::priorityLock(int rank) 
{
    Request req;
    Response resp;
    req.objectId = id;
    req.opCode = Request::LOCK_MUTEX;
    req.rank = rank;
    session->sendAndVerify(req, resp);	
}

bool MutexStub::priorityLock(int rank, unsigned timeout) 
{
    Request req;
    Response resp;
    req.objectId = id;
    req.opCode = Request::TIMED_LOCK_MUTEX;
    req.rank = rank;
    req.value = timeout;
    return session->sendAndCheckForOk(req, resp);	
}

void MutexStub::unlock()
{
    Request req;
    Response resp;
    req.objectId = id;
    req.opCode = Request::UNLOCK_MUTEX;
    session->sendAndVerify(req, resp);	
}	

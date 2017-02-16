#define RIPC_IMPLEMENTATION

#include "SessionStub.h"


void QueueStub::priorityGet(int rank, Object& objDesc) 
{
    Request  req;
    Response resp;
    req.objectId = id;
    req.opCode = Request::DEQUEUE;
    req.rank = rank;
    resp.objDesc = objDesc;
    session->sendAndVerify(req, resp, this);
    objDesc = resp.objDesc;
}

bool QueueStub::priorityGet(int rank, Object& objDesc, unsigned timeout) 
{
    Request  req;
    Response resp;
    req.objectId = id;
    req.opCode = Request::TIMED_DEQUEUE;
    req.rank = rank;
    req.value = timeout;
    resp.objDesc = objDesc;
    session->sendAndVerify(req, resp, this);
    if (resp.objDesc.data != NULL && resp.objDesc.size != 0) { 
	objDesc = resp.objDesc;
	return true;
    }
    return false;
}

void QueueStub::put(Object const& objDesc) 
{
    if (objDesc.data == NULL || objDesc.size == 0) { 
	RIPC_THROW(InvalidParameterException);
    }
    Request  req;
    Response resp;
    req.objectId = id;
    req.objDesc = objDesc;
    req.opCode = Request::ENQUEUE;
    session->sendAndVerify(req, resp);
}

void QueueStub::put(void const* obj, size_t objSize) 
{ 
    Object objDesc;
    objDesc.data = (void*)obj;
    objDesc.size = objSize;
    put(objDesc);
}

void QueueStub::broadcast(Object const& objDesc)
{
    if (objDesc.data == NULL || objDesc.size == 0) { 
	RIPC_THROW(InvalidParameterException);
    }
    Request  req;
    Response resp;
    req.objectId = id;
    req.objDesc = objDesc;
    req.opCode = Request::BROADCAST;
    session->sendAndVerify(req, resp);
}

    
void QueueStub::broadcast(void const* obj, size_t objSize) 
{ 
    Object objDesc;
    objDesc.data = (void*)obj;
    objDesc.size = objSize;
    broadcast(objDesc);
}




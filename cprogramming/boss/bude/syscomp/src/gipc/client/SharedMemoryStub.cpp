#define RIPC_IMPLEMENTATION

#include "SessionStub.h"

void SharedMemoryStub::set(Object const& objDesc)
{
    if (objDesc.data == NULL || objDesc.size <= 0) { 
	RIPC_THROW(InvalidParameterException);
    }
    Request  req;
    Response resp;
    req.opCode = Request::SET_OBJECT;
    req.objectId = id;
    req.objDesc = objDesc;
    session->sendAndVerify(req, resp);
    sequenceNo = resp.sequenceNo;
}
	
void SharedMemoryStub::set(void const* obj, size_t objSize) 
{ 
    Object objDesc;
    objDesc.data = (void*)obj;
    objDesc.size = objSize;
    set(objDesc);
}

void SharedMemoryStub::get(Object& objDesc) 
{
    Request req;
    Response resp;
    req.opCode = Request::GET_OBJECT;
    req.objectId = id; 
    resp.objDesc = objDesc;
    session->sendAndVerify(req, resp, this);
    sequenceNo = resp.sequenceNo;
    objDesc = resp.objDesc;    
}

#define RIPC_IMPLEMENTATION

#include "SessionStub.h"

void SemaphoreStub::signal(int count)
{ 
    if (count <= 0) {
	RIPC_THROW(InvalidParameterException);
    }
    Request  req;
    Response resp;
    req.objectId = id;
    req.opCode = Request::SIGNAL_SEMAPHORE;
    req.value = count;
    session->sendAndVerify(req, resp);	
}

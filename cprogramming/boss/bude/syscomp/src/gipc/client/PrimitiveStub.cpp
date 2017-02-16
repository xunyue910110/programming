#define RIPC_IMPLEMENTATION

#include "SessionStub.h"

PrimitiveStub::PrimitiveStub(SessionStub* session, char const* name, Response& resp)
{
    exists = resp.statusCode == Response::ALREADY_EXISTS;
    this->name = name;
    this->session = session;
    this->id = resp.objectId;
    next = session->primitives;
    if (next != NULL) { 
	next->prev = this;
    }
    prev = NULL;
    session->primitives = this;
}
	
PrimitiveStub::~PrimitiveStub()
{
    close();
}


bool PrimitiveStub::alreadyExists() 
{ 
    return exists;
}
    
void PrimitiveStub::priorityWait(int rank) 
{ 
    Request  req;
    Response resp;
    req.objectId = id;
    req.opCode = Request::WAIT;
    req.rank = rank;
    req.sequenceNo = sequenceNo;
    session->sendAndVerify(req, resp);	
}

bool PrimitiveStub::priorityWait(int rank, unsigned timeout)
{
    Request req;
    Response resp;
    req.objectId = id;
    req.opCode = Request::TIMED_WAIT;
    req.rank = rank;
    req.value = timeout;
    req.sequenceNo = sequenceNo;
    session->sendAndVerify(req, resp);
    return resp.statusCode == Response::OK;
}


void PrimitiveStub::reset()
{ 
    Request req;
    Response resp;
    req.objectId = id;
    req.opCode = Request::RESET;
    session->sendAndVerify(req, resp);	
}

void PrimitiveStub::close() 
{
    if (session != NULL && session->socket != NULL) { 
	Request req;
	Response resp;
	req.objectId = id;
	req.opCode = Request::CLOSE_PRIMITIVE;
	session->sendAndVerify(req, resp);	

	if (next != NULL) { 
	    next->prev = prev;
	}
	if (prev != NULL) { 
	    prev->next = next;
	} else { 
	    session->primitives = next;
	}
	session = NULL;
    }
}

char const* PrimitiveStub::getName() 
{
    return name;
}



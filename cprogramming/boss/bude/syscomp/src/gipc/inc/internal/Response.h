#ifndef __RESPONSE_H__
#define __RESPONSE_H__

#include "Object.h"

//用于响应的类定义

class ResponseHeader { 
  public:
    unsigned    size;
    unsigned    objectId;
    unsigned    statusCode;
    unsigned    sequenceNo;
};

class Response : public ResponseHeader { 
  public:
    Object objDesc;

    enum STATUC_CODES { 
	OK,
	ALREADY_EXISTS,
	TIMEOUT_EXPIRED,
	NOT_FOUND,
	NOT_OWNER,
	DEADLOCK,
	INTERRUPTED,
	INVALID_PARAMETER,
	INTERNAL_ERROR
    };
};

#endif











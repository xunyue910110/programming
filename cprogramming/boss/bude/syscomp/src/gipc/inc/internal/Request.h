#ifndef __REQUEST_H__
#define __REQUEST_H__

#include "Object.h"

const int MAX_COMMPACKET_LEN=1024*1;

//发向服务端的请求对象类

class RequestHeader { 
  public:
    unsigned     size;
    unsigned     objectId; 
    unsigned     opCode;
    unsigned     rank;
    unsigned     value;
    unsigned     sequenceNo;
    unsigned     nameLen;
};

class Request : public RequestHeader { 
  public:
    char*        name;
    Object   objDesc;

    Request() { 
	name = NULL;
    }

    //请求类型
    enum OPCODES { 
	WAIT,
	TIMED_WAIT,
	RESET,
	SIGNAL_EVENT,
	SIGNAL_SEMAPHORE,
	PULSE,
	LOCK_MUTEX,
	TIMED_LOCK_MUTEX,
	UNLOCK_MUTEX,
	ENQUEUE,
	BROADCAST,
	DEQUEUE,
	TIMED_DEQUEUE,
	SET_OBJECT,
	GET_OBJECT,
	EXCLUSIVE_LOCK,
	SHARED_LOCK,
	TIMED_EXCLUSIVE_LOCK,
	TIMED_SHARED_LOCK,
	UNLOCK,
	CREATE_SEMAPHORE,
	CREATE_EVENT,
	CREATE_AUTO_EVENT,
	CREATE_MUTEX,
	CREATE_QUEUE,
	CREATE_SHMEM,
	CREATE_LOCK,
	CREATE_BARRIER,
	OPEN_SEMAPHORE,
	OPEN_EVENT,
	OPEN_MUTEX,
	OPEN_QUEUE,
	OPEN_SHMEM,
	OPEN_LOCK,
	OPEN_BARRIER,
	CLOSE_PRIMITIVE,
	CLOSE_SESSION,
	SHOW_INFO,
	SHUTDOWN,
	SESSION_ID
    };

    static const char* const opCodeNames[];

    char const* getName() { 
	return opCodeNames[opCode];
    }
};



class CommPacket {
  public:
	int       opCode;
	int       opNo;
	int       sequenceNo; //-1 结束包
    char      value[MAX_COMMPACKET_LEN];

    enum OP_CODE {
    TIMEOUT= 0,
    REQEST = 1000,
    EXEC_QUERY,
    RESPONSE =2000, 
    QUERY_RESULT 
    };
    
    CommPacket () {
      opCode = 0;
      opNo = 0;
      sequenceNo = -1;
      memset(value,0,sizeof(value));
    }
};

#endif





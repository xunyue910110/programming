#ifndef __COMMBUFFER_H__
#define __COMMBUFFER_H__

#include <string.h>

#include "Request.h"
#include "Response.h"

//通讯缓冲类
class CommBuffer { 
  public:
    void packResponse(Response& resp) 
    { 
	unsigned size = sizeof(ResponseHeader) + resp.objDesc.size;
	resize(size);
	packInt(size);
	packInt(resp.objectId);
	packInt(resp.statusCode);
	packInt(resp.sequenceNo);
	packObj(resp.objDesc);
	resp.objDesc.deallocate();	
    }

    void unpackResponse(Response& resp) 
    { 
	pos = 0;
	resp.size       = unpackInt();
	resp.objectId   = unpackInt();
	resp.statusCode = unpackInt();
	resp.sequenceNo = unpackInt();
	resp.objDesc.size = resp.size - sizeof(ResponseHeader);
    }

    void packRequest(Request& req) 
    { 
	int nameLen = req.name != NULL ? strlen(req.name) + 1 : 0;
	unsigned reqSize = sizeof(RequestHeader) + req.objDesc.size + nameLen;
	resize(reqSize);
	packInt(reqSize);
	packInt(req.objectId);
	packInt(req.opCode);
	packInt(req.rank);
	packInt(req.value);
	packInt(req.sequenceNo);
	packInt(nameLen);
	packStr(req.name, nameLen);
	packObj(req.objDesc);
    }

    void unpackRequest(Request& req) 
    { 
	pos = 0;
	req.size = unpackInt();
	req.objectId = unpackInt();
	req.opCode = unpackInt();
        req.rank = unpackInt();
	req.value = unpackInt();
	req.sequenceNo = unpackInt();
	req.nameLen = unpackInt();
	req.objDesc.size = req.size - sizeof(RequestHeader) - req.nameLen;
    }

    char* getAddr() { 
	return buf;
    }

    size_t getSize() { 
	return pos;
    }

    CommBuffer() { 
	bufSize = sizeof(Request);
	buf = new char[bufSize];
    }

    ~CommBuffer() {
	delete[] buf;
    }

  private:
    char*  buf;
    size_t bufSize;
    int    pos;

    void resize(size_t size) { 
	if (size > bufSize) { 
	    size_t newBufSize = size > bufSize*2 ? size : bufSize*2;
	    delete[] buf;
	    buf = new char[newBufSize];
	    bufSize = newBufSize;
	}
	pos = 0;
    }
	
    void packInt(unsigned val) { 
	char* dst = buf + pos;
	dst[0] = char(val >> 24);
	dst[1] = char(val >> 16);
	dst[2] = char(val >> 8);
	dst[3] = char(val);
	pos += 4;
    }

    void packObj(Object& objDesc) {
	memcpy(buf + pos, objDesc.data, objDesc.size);
	pos += objDesc.size;
    }

    void packStr(char const* str, size_t strLen) {
	memcpy(buf + pos, str, strLen);
	pos += strLen;
    }

    unsigned unpackInt() { 
	unsigned char* src = (unsigned char*)buf + pos;
	pos += 4;
	return (((((src[0] << 8) + src[1]) << 8) + src[2]) << 8) + src[3];
    }
    
};

#endif

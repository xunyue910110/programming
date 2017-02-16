#ifndef __PRIMITIVE_STUB_H__
#define __PRIMITIVE_STUB_H__

#include "Primitive.h"
#include "Response.h"

class SessionStub;

//客户端抽象原语类
class PrimitiveStub : public virtual Primitive 
{ 
    friend class SessionStub;
  public:
    char const* getName();
    bool alreadyExists();
    void priorityWait(int rank);
    bool priorityWait(int rank, unsigned timeout);
    void reset();
    void close();

    PrimitiveStub(SessionStub* session, char const* name, Response& resp);
    ~PrimitiveStub();

  protected:
    bool           exists;
    SessionStub*   session;
    char const*    name;
    unsigned       id;
    unsigned       sequenceNo;
    PrimitiveStub* next;
    PrimitiveStub* prev;
};

#endif    


#ifndef __PRIMITIVE_IMPL_H__
#define __PRIMITIVE_IMPL_H__

#include "Primitive.h"
#include "CriticalSection.h"
#include "WaitObject.h"

class LockObject;

class PrimitiveImpl : public virtual Primitive { 
    friend class SessionImpl;
    friend class WaitObject;
    friend class LockObject;
    friend class HashMap;
    friend class Server;
  public:
    bool          alreadyExists();
    void          close();
    char const*   getName();
    virtual char* dump(char* buf);

  protected:
    virtual void  beginAccess(SessionImpl* session);
    virtual void  endAccess(SessionImpl* session);
    virtual bool  detectDeadlock(WaitObject* wob, SessionImpl* session);
    virtual void  unlock(LockObject* lob);
    virtual void  addWaitObject(WaitObject* wob);

    virtual void  deletePrimitive() = 0;

    virtual void  wakeupAll();

    void copyObject(Object& dst, Object& src);

    PrimitiveImpl(Server* server, char const* name);
    ~PrimitiveImpl();

    PrimitiveImpl*  next;
    PrimitiveImpl*  prev;
    Server*         server;
    char*           name;
    unsigned        accessCount;
    unsigned        id;
    WaitObject      wobList;
    CriticalSection cs;
};

#endif

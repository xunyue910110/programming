#ifndef __SHARED_MEMORY_IMPL_H__
#define __SHARED_MEMORY_IMPL_H__

#include "SharedMemory.h"
#include "PrimitiveImpl.h"

class SharedMemoryImpl : public PrimitiveImpl, public SharedMemory { 
    friend class Server;
  public:
    void  priorityWait(int rank);
    bool  priorityWait(int rank, unsigned timeout);
    void  get(Object& objDesc);
    void  set(Object const& objDesc);
    void  set(void const* obj, size_t objSize);
    void  reset();
    char* dump(char* buf);

    SharedMemoryImpl(Server* server, char const* name, Object const& objDesc, bool copy);
    SharedMemoryImpl(Server* server, char const* name, SessionImpl* session, size_t objSize);
    void deletePrimitive();

  protected:
    void  set(SessionImpl* session, size_t size);

    unsigned   sequenceNo;
    Object objDesc;
};

#endif

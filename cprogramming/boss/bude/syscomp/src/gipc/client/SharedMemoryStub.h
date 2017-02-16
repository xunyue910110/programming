#ifndef __SHARED_MEMORY_STUB_H__
#define __SHARED_MEMORY_STUB_H__

#include "SharedMemory.h"
#include "PrimitiveStub.h"

class SharedMemoryStub : public PrimitiveStub, public SharedMemory { 
  public:
    void set(Object const& obj);
    void set(void const* obj, size_t objSize);
    void get(Object& objDesc);

    SharedMemoryStub(SessionStub* session, char const* name, Response& resp) 
      : PrimitiveStub(session, name, resp) {}
};

#endif

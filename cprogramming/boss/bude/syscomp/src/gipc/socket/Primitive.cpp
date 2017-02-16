#define _IMPLEMENTATION

#include "Primitive.h"

static char const* const PrimitiveTypeName[] = { 
    "MUTEX", 
    "EVENT", 
    "SEMAPHORE", 
    "BARRIER", 
    "LOCK", 
    "SHARED_MEMORY", 
    "QUEUE"
};

char const* Primitive::getTypeName()
{
    return PrimitiveTypeName[getType()];
}

void* Primitive::allocate(size_t size) 
{ 
    return new char[size];
}

void Primitive::deallocate(void* obj, size_t) { 
    delete[] (char*)obj;
}

void Primitive::deallocator(Object const& objDesc) 
{
    objDesc.container->deallocate((void*)objDesc.data, objDesc.size);
}


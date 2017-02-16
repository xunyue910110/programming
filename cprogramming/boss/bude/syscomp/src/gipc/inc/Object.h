

#ifndef __OBJECT_H__
#define __OBJECT_H__

#include <string.h>
#include <assert.h>

class Primitive;


/**
 * \class Object Object.h
 * \brief 底层通信用的数据对象
 * 用于底层通信的数据对象封装.
 *
 */

class Object {    
  public:
    void*  data;
    size_t size;

    Object() { 
	    data = NULL;
	    size = 0;
	    deallocator = NULL;
    }

    Object(void* buf, size_t bufSize) { 
	    data = buf;
	    size = bufSize;
	    deallocator = NULL;
    }

    void assign(Object const& cp) 
    { 
	if (data == NULL) { 
	    data = cp.data;
	    size = cp.size;
	    deallocator = cp.deallocator;
	    container = cp.container;
	} else { 
	    assert(size >= cp.size);
	    size = cp.size;
	    memcpy(data, cp.data, size);
	}
    }

    void deallocate() const 
    { 
	 if (deallocator != NULL) {
	    (*deallocator)(*this);
	  }
    }
    
    // Function Pointor
    // deallocate 
    void (*deallocator)(Object const& obj);
    
    Primitive* container; // container to which object belongs
};

#endif

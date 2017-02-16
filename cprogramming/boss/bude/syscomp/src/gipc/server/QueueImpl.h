#ifndef __QUEUE_IMPL_H__
#define __QUEUE_IMPL_H__

#include "Queue.h"
#include "PrimitiveImpl.h"

class QueueImpl : public PrimitiveImpl, public Queue { 
    friend class Server;
  public:
    class Receipt { 
      public:
	Receipt*         next;
	SessionImpl* session;
    };

    class QueueElement { 
      public:
	QueueElement* next; 
	Object    objDesc;
	Receipt*      receipts;
	unsigned      nReceipts;
	bool          isBroadcast;
	char* dump(char* buf); 
    };
    
    enum QUEUE_WOB_FLAG {
	CHECK_ONLY = 1
    };
    
    void  priorityWait(int rank);
    bool  priorityWait(int rank, unsigned timeout);
    void  priorityGet(int rank, Object& objDesc);
    bool  priorityGet(int rank, Object& objDesc, unsigned timeout);
    void  put(Object const& objDesc);
    void  put(void const* obj, size_t objSize);
    void  broadcast(Object const& objDesc);
    void  broadcast(void const* obj, size_t objSize);
    void  reset();
    char* dump(char* buf);

    QueueImpl(Server* server, char const* name)
    : PrimitiveImpl(server, name) 
    {
	messageList = NULL;
	lastElement = NULL;
	freeElements = NULL;
	freeReceipts = NULL;
    }
    void deletePrimitive();

  protected:
    void put(SessionImpl* session, size_t objSize);
    void broadcast(SessionImpl* session, size_t objSize);
    bool send(QueueElement* element);
    bool nextElementAvailable();
    bool nextElementAvailable(QueueElement* element, WaitObject* wob);
    void endAccess(SessionImpl* session);
    void clear();

    Receipt* allocateReceipt(SessionImpl* session, Receipt* chain) 
    { 
	Receipt* rc = freeReceipts;
	if (rc != NULL) {
	    freeReceipts = rc->next;
	} else {
	    rc = new Receipt;
	}
	rc->next = chain;
	rc->session = session;
	return rc;
    } 

    void deallocateReceipt(Receipt* receipt) 
    { 
	receipt->next = freeReceipts;
	freeReceipts = receipt;
    }

    QueueElement* allocateElement(Object const& objDesc)
    {
	QueueElement* elem = freeElements;
	if (elem != NULL) {
	    freeElements = elem->next;
	} else {
	    elem = new QueueElement();
	}
	elem->next = NULL;
	elem->objDesc = objDesc;
	return elem;
    }

    void deallocateElement(QueueElement* elem)
    {
	elem->next = freeElements;
	freeElements = elem;
    }

    QueueElement* messageList;
    QueueElement* lastElement;
    QueueElement* freeElements;
    Receipt*      freeReceipts;
};

#endif

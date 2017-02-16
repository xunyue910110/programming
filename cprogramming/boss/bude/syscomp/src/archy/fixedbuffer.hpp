#ifndef ARCHY_FIXEDBUFFER_INCLUDE_HEADER801410471041414_841414
#define ARCHY_FIXEDBUFFER_INCLUDE_HEADER801410471041414_841414


#include "mutex.hpp"
#include "semaphore.hpp"

namespace archy {
/**
 * The buffer class represents an IPC service that is built upon a buffer
 * of fixed capacity that can be used to transfer objects between one or
 * more producer and consumer threads.  Producer threads post objects
 * into the buffer, and consumer threads wait for and receive objects from
 * the buffer.  Semaphores are used to to block the buffer from overflowing
 * and indicate when there is data available, and mutexes are used to protect
 * multiple consumers and producer threads from stepping over each other.
 * 
 * The buffer class is an abstract class in that the actual data being
 * buffered is not directly specified within the buffer class itself.  The
 * buffer class should be used as a base class for a class that actually
 * impliments buffering and which may be aware of the data types actually
 * are being buffered.  A template class could be created based on buffer
 * for this purpose.  Another possibility is to create a class derived
 * from both Thread and buffer which can be used to implement message passing
 * threads.
 * 
 * @author David Sugar <dyfet@ostel.com>
 * @short Producer/Consumer buffer for use between threads.
 */

class buffer
{
	mutex lock_head, lock_tail;
	semaphore size_head, size_tail;
	int _size;
	int _used;

protected:
	/**
	 * Invoke derived class buffer peeking method.
	 * @return size of object found.
	 * @param buf pointer to copy contents of head of buffer to.
	 */
	virtual int onpeek(void *buf) = 0;
	/**
	 * Invoke derived class object request from buffer.
	 * @return size of object returned.
	 * @param buf pointer to hold object returned from the buffer.
	 */
	virtual int onwait(void *buf) = 0;
	/**
	 * Invoke derived class posting of object to buffer.
	 * @return size of object posted.
	 * @param buf pointer to object being posted to the buffer.
	 */
	virtual int onpost(void *buf) = 0;

public:
	/**
	 * Create a buffer object of known capacity.
	 * @param capcity is the integer capacity of the buffer.
	 */
	buffer(int capacity);
	/**
	 * In derived functions, may be used to free the actual memory
	 * used to hold buffered data.
	 */
	virtual ~buffer()
		{return;}

	/**
	 * Return the capacity of the buffer as specified at creation.
	 * @return size of buffer.
	 */
	inline int getsize(void)
		{return _size;}
	
	/**
	 * Return the current capacity in use for the buffer.  Free space
	 * is technically getSize() - getUsed().
	 * @return integer used capacity of the buffer.
	 * @see #getSize
	 */
	inline int getused(void)
		{return _used;}

	/**
	 * Let one or more threads wait for an object to become available
	 * in the buffer.  The waiting thread(s) will wait forever if no
	 * object is ever placed into the buffer.
	 * 
	 * @return size of object passed by buffer in bytes.
	 * @param buf pointer to store object retrieved from the buffer.
	 */
	int wait(void *buf);

	/**
	 * Post an object into the buffer and enable a waiting thread to
	 * receive it.
	 * 
	 * @return size of object posted in bytes.
	 * @param buf pointer to object to store in the buffer.
	 */
	int post(void *buf);

	/**
	 * Peek at the current content (first object) in the buffer.
	 * 
	 * @return size of object in the buffer.
	 * @param buf pointer to store object found in the buffer.
	 */
	int peek(void *buf);

	/**
	 * New virtual to test if buffer is a valid object.
	 * @return true if object is valid.
	 */
	virtual bool isvalid(void)
		{return true;}
};

/**
 * A buffer class that holds a known capacity of fixed sized objects defined
 * during creation.
 * 
 * @author David Sugar <dyfet@ostel.com>
 * @short producer/consumer buffer for fixed size objects.
 */
class fixedbuffer : public buffer
{
private:
	char *buf, *head, *tail;
	int objsize;

protected:
	/**
	 * Return the first object in the buffer.
	 * @return predefined size of this buffers objects.
	 * @param buf pointer to copy contents of head of buffer to.
	 */
	int onpeek(void *buf);

	/**
	 * Wait for and return a fixed object in the buffer.
	 * @return predefined size of this buffers objects.
	 * @param buf pointer to hold object returned from the buffer.
	 */
	int onwait(void *buf);

	/**
	 * Post an object of the appropriate size into the buffer.
	 * @return predefined size of this buffers objects.
	 * @param buf pointer to data to copy into the buffer.
	 */
	int onpost(void *buf);	

public:
	/**
	 * Create a buffer of known capacity for objects of a specified
	 * size.
	 * 
	 * @param capacity of the buffer.
	 * @param objsize for each object held in the buffer.
	 */
	fixedbuffer(int capacity, int objsize)throw(thread_resource_error);

	/**
	 * Create a copy of an existing fixed size buffer and duplicate
	 * it's contents.
	 * 
	 * @param fb existing fixedbuffer object.
	 */
	fixedbuffer(const fixedbuffer &fb);

	/**
	 * Destroy the fixed buffer and free the memory used to store objects.
	 */
	virtual ~fixedbuffer();

	fixedbuffer &operator=(const fixedbuffer &fb);

	bool isvalid(void);
};

}// namespace archy

#endif // ARCHY_FIXEDBUFFER_INCLUDE_HEADER801410471041414_841414


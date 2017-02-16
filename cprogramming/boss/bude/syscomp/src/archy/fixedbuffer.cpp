#include "fixedbuffer.hpp"


namespace archy {
	
buffer::buffer(int capacity) 
:	lock_head(),
	lock_tail(),
	size_head(),
	size_tail(capacity)
{
	_size = capacity;
	_used = 0;
}

int buffer::wait(void *buf)
{
	int	rc;

	size_head.wait();
	lock_head.do_lock();
	rc = onwait(buf);
	--_used;
	lock_head.do_unlock();
	size_tail.post();
	return rc;	
}

int buffer::post(void *buf)
{
	int	rc;

	size_tail.wait();
	lock_tail.do_lock();
	rc = onpost(buf);
	++_used;
	lock_tail.do_unlock();
	size_head.post();
	return rc;
}

int buffer::peek(void *buf)
{
	int	rc;
	
	scoped_lock lock(lock_head);
	rc = onpeek(buf);
	return rc;
}

fixedbuffer::fixedbuffer(int capacity, int osize) throw(thread_resource_error)
:buffer(capacity)
{
	objsize = osize;
	buf = new char[capacity * objsize];
	if( !buf )
		throw thread_resource_error();
	
	head = tail = buf;
}

fixedbuffer::~fixedbuffer()
{
	if(buf)
		delete[] buf;
}

bool fixedbuffer::isvalid(void)
{
	if(head && tail)
		return true;

	return false;
}

#define	MAXBUF	(buf + (getsize() * objsize))

int fixedbuffer::onpeek(void *data)
{
	memcpy(data, head, objsize);
	return objsize;
}

int fixedbuffer::onwait(void *data)
{
	memcpy(data, head, objsize);
	if((head += objsize) >= MAXBUF)
		head = buf;
	return objsize;
}

int fixedbuffer::onpost(void *data)
{
	memcpy(tail, data, objsize);
	if((tail += objsize) >= MAXBUF)
		tail = buf;
	return objsize;
}

} // namespace archy


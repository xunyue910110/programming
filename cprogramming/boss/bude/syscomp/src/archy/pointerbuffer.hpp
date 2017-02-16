#ifndef ARCHY_POINTERBUFFER_INCLUDE_HEADER801410471041414_841414
#define ARCHY_POINTERBUFFER_INCLUDE_HEADER801410471041414_841414


#include "mutex.hpp"
#include "semaphore.hpp"

namespace archy {

class pointerbuffer
{
	mutex lock_head, lock_tail;
	semaphore size_head, size_tail;
	int _size;
	int _used;

protected:
	virtual void* onpeek() = 0;
	virtual void* onwait() = 0;
	virtual void onpost(void *buf) = 0;

public:
	pointerbuffer(int capacity)
	:lock_head(),
	lock_tail(),
	size_head(),
	size_tail(capacity)
	{
		_size = capacity;
		_used = 0;
	}

	virtual ~pointerbuffer()
		{}

	inline int getsize(void)
		{return _size;}
	
	inline int getused(void)
		{return _used;}

	void* wait()
	{
		void* ret = 0;
		size_head.wait();
		lock_head.do_lock();
		ret = onwait();
		--_used;
		lock_head.do_unlock();
		size_tail.post();

		return ret;
	}

	void post(void *buf)
	{
		void* ret = 0;

		size_tail.wait();
		lock_tail.do_lock();
		onpost(buf);
		++_used;
		lock_tail.do_unlock();
		size_head.post();
	}

	void* peek()
	{
		void* ret = 0;
	
		scoped_lock lock(lock_head);
		ret = onpeek();
		return ret;

	}

	virtual bool isvalid(void)
		{return true;}
};


class fixedpointerbuffer : public pointerbuffer
{
private:
	void **buf;
	int head, tail;

protected:
	void* onpeek()
	{
		return buf[head];
	}


	void* onwait()
	{
		void* ret = buf[head];
		if(++head >= getsize())
			head = 0;
		return ret;
	}


	void onpost(void *bu)
	{
		buf[tail] = bu;
		if(++tail >= getsize())
			tail = 0;
	}

public:
	fixedpointerbuffer(int capacity)throw(thread_resource_error)
	:pointerbuffer(capacity)
	{
		typedef void* TYPEPP;
		buf = new TYPEPP[capacity + 1];
		if( !buf )
			throw thread_resource_error();
		
		head = tail = 0;
	}
		

	virtual ~fixedpointerbuffer()
	{
		if(buf)
			delete[] buf;
	}

	



};

}// namespace arch

#endif // ARCHY_POINTERBUFFER_INCLUDE_HEADER801410471041414_841414


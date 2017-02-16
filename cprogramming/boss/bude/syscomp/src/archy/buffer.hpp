#ifndef ARCHY_BUFFER_INCLUDE_HEADER801410471041414_841414
#define ARCHY_BUFFER_INCLUDE_HEADER801410471041414_841414


#include "mutex.hpp"
#include "semaphore.hpp"

namespace archy {

template <typename T>
class tbuffer
{
	mutex lock_head, lock_tail;
	semaphore size_head, size_tail;
	int _size;
	int _used;

protected:
	virtual T onpeek() = 0;
	virtual T onwait() = 0;
	virtual void onpost(T buf) = 0;

public:
	tbuffer(int capacity)
	:lock_head(),
	lock_tail(),
	size_head(),
	size_tail(capacity)
	{
		_size = capacity;
		_used = 0;
	}

	virtual ~tbuffer()
		{}

	int getsize()
		{return _size;}
	
	int getused()
		{return _used;}

	T wait()
	{
		T ret = 0;
		size_head.wait();
		lock_head.do_lock();
		ret = onwait();
		--_used;
		lock_head.do_unlock();
		size_tail.post();

		return ret;
	}

	void post(T buf)
	{
		T ret = 0;

		size_tail.wait();
		lock_tail.do_lock();
		onpost(buf);
		++_used;
		lock_tail.do_unlock();
		size_head.post();
	}

	T peek()
	{
		T ret = 0;
	
		scoped_lock lock(lock_head);
		ret = onpeek();
		return ret;

	}

	virtual bool isvalid(void)
		{return true;}
};

template <typename T>
class tfixedbuffer : public tbuffer<T>
{
private:
	T *buf;
	int head, tail;

protected:
	T onpeek()
	{
		return buf[head];
	}


	T onwait()
	{
		T ret = buf[head];
		if(++head >= this->getsize())
			head = 0;
		return ret;
	}


	void onpost(T bu)
	{
		buf[tail] = bu;
		if(++tail >= this->getsize())
			tail = 0;
	}

public:
	tfixedbuffer(int capacity)throw(thread_resource_error)
	:tbuffer<T>(capacity)
	{
		buf = new T[capacity + 1];
		if( !buf )
			throw thread_resource_error();
		
		head = tail = 0;
	}

	virtual ~tfixedbuffer()
	{
		if(buf)
			delete[] buf;
	}

};

}// namespace arch

#endif // ARCHY_BUFFER_INCLUDE_HEADER801410471041414_841414


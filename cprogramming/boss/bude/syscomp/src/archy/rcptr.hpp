#if !defined(RCOBJECT_RCIPTR)
#define RCOBJECT_RCIPTR

namespace archy {
	
class rcobject {                       // base class for reference-
public:                                // counted objects
	void addReference(){ ++refCount; }
	void removeReference(){ if (--refCount == 0) 	delete this; }
	
	void markUnshareable(){ shareable = false; }
	bool isShareable() const{ return shareable; }
	
	bool isShared() const{ return refCount > 1; }

protected:
	rcobject()
	:refCount(0), shareable(true) 
	{}
	rcobject(const rcobject& rhs)
	:refCount(0), shareable(true) 
	{}
	rcobject& operator=(const rcobject& rhs)
	{ return *this; }
	virtual ~rcobject(){}

private:
	int refCount;
	bool shareable;
};


template<class T>
class rcptr {
public:
	rcptr(T* realPtr = 0);
	rcptr(const rcptr& rhs);
	~rcptr();
	
	rcptr& operator=(const rcptr& rhs);
	
	const T* operator->() const;               // see below for an
	T* operator->();                           // explanation of why
	const T& operator*() const;                // these functions are
	T& operator*();                            // declared this way
	
	
	operator const T* () const { return counter->pointee; }
	operator T*() {
		return counter->pointee;
	}
    operator bool () const {
		return counter->pointee != 0;
    }
	
private:
	struct CountHolder: public rcobject {
		~CountHolder() { delete pointee; }
		T *pointee;
	};
	
	CountHolder *counter;
	
	void init();
	void makeCopy();                           // see below
};

template<class T>
void rcptr<T>::init()
{
	if (counter->isShareable() == false) {
		T *oldValue = counter->pointee;
		counter = new CountHolder;
		counter->pointee = new T(*oldValue);
	}
	
	counter->addReference();
}

template<class T>
rcptr<T>::rcptr(T* realPtr)
:counter(new CountHolder)
{
	counter->pointee = realPtr;
	init();
}

template<class T>
rcptr<T>::rcptr(const rcptr& rhs)
:counter(rhs.counter)
{ init(); }

template<class T>
rcptr<T>::~rcptr()
{ counter->removeReference(); }

template<class T>
rcptr<T>& rcptr<T>::operator=(const rcptr& rhs)
{
	if (counter != rhs.counter) {
		counter->removeReference();
		counter = rhs.counter;
		init();
	}
	return *this;
}

template<class T>                          // implement the copy
void rcptr<T>::makeCopy()                 // part of copy-on-
{                                          // write (COW)
	if (counter->isShared()) {
		T *oldValue = counter->pointee;
		counter->removeReference();
		counter = new CountHolder;
		counter->pointee = new T(*oldValue);
		counter->addReference();
	}
}

template<class T>                           // const access;
const T* rcptr<T>::operator->() const      // no COW needed
{ return counter->pointee; }

template<class T>                           // non-const
T* rcptr<T>::operator->()                  // access; COW
{ makeCopy(); return counter->pointee; }    // needed

template<class T>                           // const access;
const T& rcptr<T>::operator*() const       // no COW needed
{ return *(counter->pointee); }

template<class T>                           // non-const
T& rcptr<T>::operator*()                   // access; do the
{ makeCopy(); return *(counter->pointee); } // COW thing


} // namespace archy

#endif

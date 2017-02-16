//////////////////////////////////////////////////////////////////////
// 
// 类名: threadkey
// 描述: 线程专有数据类

// 作者: 黄进兵(huangjb@lianchuang.com)
// 时间: 2003-7-14 
// 版本: V1.0.0 
// 版权: 南京联创科技股份有限公司
//////////////////////////////////////////////////////////////////////


#include "threadkey.hpp"

#if defined(ARCHY_HAS_WINTHREADS)
 #define KEY_INVALID 0xffffffff
 #define pthread_key_delete(key) TlsFree(key)
 #define pthread_getspecific(key) TlsGetValue(key)
 #define pthread_setspecific(key, ptr) TlsSetValue(key, ptr)
#elif defined(ARCHY_HAS_PTHREADS)
 #ifndef	KEY_INVALID
 #define	KEY_INVALID	((pthread_key_t)(~0))
 #endif
#endif

namespace archy {
	
threadkey::threadkey() throw (thread_resource_error)
{
#if defined(ARCHY_HAS_WINTHREADS)
	key = TlsAlloc();
#elif defined(ARCHY_HAS_PTHREADS)
	if(pthread_key_create(&key, NULL)) {
		key = KEY_INVALID;
		throw thread_resource_error();
	}
#endif
}

threadkey::~threadkey()
{
	if(key != KEY_INVALID)
		pthread_key_delete(key);
}

void *threadkey::getkey(void)
{
	if(key != KEY_INVALID)
		return pthread_getspecific(key);
	else
		return NULL;
}

bool threadkey::setkey(void *ptr)
{
	if(key != KEY_INVALID) {
		pthread_setspecific(key, ptr);
		return true;
	}
	return false;
}

} // namespace archy


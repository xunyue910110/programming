//////////////////////////////////////////////////////////////////////
// 
// 类名: threadkey
// 描述: 线程专有数据类

// 作者: 黄进兵(huangjb@lianchuang.com)
// 时间: 2003-7-14 
// 版本: V1.0.0 
// 版权: 南京联创科技股份有限公司
//////////////////////////////////////////////////////////////////////

#ifndef ARCHY_THREAD_SPECIFIC_VAL_INCLUDE888888888034242
#define ARCHY_THREAD_SPECIFIC_VAL_INCLUDE888888888034242
#include "archy_config.hpp"
#if defined(ARCHY_HAS_PTHREADS)
#include <pthread.h>
#endif 
#include "thread_exceptions.hpp"
#include "noncopyable.hpp"

namespace archy {

class threadkey: private noncopyable
{
private:
#if defined(ARCHY_HAS_PTHREADS)
	pthread_key_t key;
#elif defined(ARCHY_HAS_WINTHREADS)
	DWORD	key;
#endif

public:
	threadkey()throw (thread_resource_error);
	virtual ~threadkey();
	void *getkey(void);
	bool setkey(void *);
};

} // namespace archy

#endif // ARCHY_THREAD_SPECIFIC_VAL_INCLUDE888888888034242


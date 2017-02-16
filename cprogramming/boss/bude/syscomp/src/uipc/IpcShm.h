#ifndef __IPC_SHM_H_
#define __IPC_SHM_H_

#include <sys/shm.h>

namespace budeipc {

/**
 * 共享内存
 */
 
using namespace std;

class IpcShm;
typedef auto_ptr<IpcShm> PIpcShm;
 
class IpcShm {
public:
	/// 管理进程引用
	static int create(int key, int size);
	static void remove(int key);
	
	/// 处理进程引用
	static int isExist(int key);
	static IpcShm* getShm(int key);
	
	/// 以下为共享内存段类接口
	
	virtual ~IpcShm() { };

	//! 访问、挂接、引用共享内存段
	virtual char* getbuf() = 0;
	virtual long getkey() = 0;
	virtual long getipcid() = 0;
	virtual int getsize() = 0;
	
	//! 访问、挂接、引用共享内存段
	virtual int write(const char* inbuf, int buflen) = 0;
	
	//! 访问、挂接、引用共享内存段
	virtual int read(char* outbuf, int buflen) = 0;
					
};
	
} // namespace budeipc

#endif


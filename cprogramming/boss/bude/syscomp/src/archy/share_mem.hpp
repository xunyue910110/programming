// 共享内存封装类
#ifndef ARCHY__SHARE_MEM_HEADER_INCLUDED__
#define ARCHY__SHARE_MEM_HEADER_INCLUDED__

namespace archy {

class share_mem//:private noncopyable
{
public:
	enum SHM_OPT{SHM_READONLY, SHM_READWRITE};
	
	// 如果该共享内存存在，则返回它的映射地址；如果不存在则先创建然后再返回
	void * get_shm( 
		const char * shm_name,  // 共享内存的名字，如果是unix环境，需要输入数字，在各进程间共享
		unsigned long shm_size, // 共享内存的大小 
		bool   create_when_null = true, // 不存在时是否创建
		SHM_OPT shm_opt = SHM_READWRITE // 共享内存的读写属性 
	);
	
    void   del_shm(const char * shm_name);
	// 返回共享内存的映射地址
	void * get_addr() const{ return _MemoryAddress; }
	
	// 返回共享内存的大小，该大小在getShm时候就已经设定，可以频繁调用
	int size() const{ return _Size;	}

public:
	share_mem();
	~share_mem();

protected:
	void *  _Handle;      	//共享内存的系统句柄，在unix下表示ID
	void *  _MemoryAddress; //共享内存在进程中的映射地址
	int     _Size;          // 储共享内存的大小
};  

} // namespace archy
#endif

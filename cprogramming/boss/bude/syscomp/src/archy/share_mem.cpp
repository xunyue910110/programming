#include "share_mem.hpp"
#include "debug.hpp"
#ifdef WIN32
#include <windows.h>

#else
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#endif

#include <stdlib.h>

namespace archy {

share_mem::share_mem()
{
    _MemoryAddress = 0;
    _Handle = 0;
}

share_mem::~share_mem()
{
#ifdef WIN32
    if( _MemoryAddress != 0 )
        ::UnmapViewOfFile(_MemoryAddress);
        
    if( _Handle != 0 )
        ::CloseHandle(_Handle);
#endif

}


void * share_mem::get_shm(const char * shm_name, unsigned long shm_size, 
						  bool   create_when_null,
						  SHM_OPT shm_opt)
{
// windows下的共享内存
#ifdef WIN32
	//先打开，如果不成功，再create
	int opt = 0;
	if( shm_opt == SHM_READONLY )
		opt = FILE_MAP_READ;
	else if( shm_opt == SHM_READWRITE )
		opt = FILE_MAP_WRITE;
	
	_Handle = ::OpenFileMapping(opt, false, shm_name);
	
	if( _Handle == 0 && create_when_null){
		_Handle = ::CreateFileMapping( (HANDLE)0xFFFFFFFF ,
			0,PAGE_READWRITE, 0, 0x1000, shm_name);
	}
	
	if( _Handle == 0 ) 
		return 0;
	
	_MemoryAddress = (char *) MapViewOfFile(_Handle, opt, 0, 0, 0);
	
	return _MemoryAddress;
	
#else  //unix下的共享内存
	struct	shmid_ds	ShmData;
	key_t SHM_KEY = atoi(shm_name);
	//读写属性
	int opt = 0;
	if( shm_opt == SHM_READONLY )
		opt = SHM_R;
	else if( shm_opt == SHM_READWRITE )
		opt = SHM_R|SHM_W;
	
	if (create_when_null)
	{
		opt |= IPC_CREAT;
	}

	(int &) _Handle = shmget( SHM_KEY, shm_size, opt | IPC_EXCL );
	
	if( (int &) _Handle <0 ){
		//提取共享内存标识
		(int &) _Handle=shmget( SHM_KEY, 0, SHM_R|SHM_W );
	}
	
	if( (int &) _Handle <0 ){
		return 0;
	}
	//查询共享内存块大小
	shmctl( (int &) _Handle, IPC_STAT, &ShmData );
	
	_Size = ShmData.shm_segsz;
	//映射进程内地址
	_MemoryAddress = shmat( (int &) _Handle, 0, 0 );
	
	if( _MemoryAddress == (void *)(-1) ){
		return 0;
	}
	return _MemoryAddress;

#endif
}

void   share_mem::del_shm(const char * shm_name)
{
#ifdef WIN32
    
#else  //unix下的共享内存
    int id;
	key_t SHM_KEY = atoi(shm_name);

    id = shmget( SHM_KEY, 0, SHM_R|SHM_W );
    if (id != -1)
    {
        shmctl(id, IPC_RMID, 0);
    }
#endif

}

} // namespace archy

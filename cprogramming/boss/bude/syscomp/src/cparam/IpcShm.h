#ifndef __IPC_SHM_H_
#define __IPC_SHM_H_


namespace budeipc {

	/**
	 * 共享内存
	 */

	/**
	 * interface IpcShm definition
	 */ 
	class IpcShm {
	public:
		
		/// sharememory create and remove
		static int create(int key, int size);
		static void remove(int key);
		
		/// 
		static int isExist(int key);
		static IpcShm& getShm(int key);
		
		/// following is IpcShm'e interface
		
		virtual ~IpcShm() { };
	
		//! 访问、挂接、引用共享内存段
		virtual char* atbuf() = 0;
		virtual void dtbuf(char* _buf) = 0;
		
		virtual long getkey() = 0;
		virtual long getipcid() = 0;
		virtual int getsize() = 0;
	};
		
} // namespace budeipc

#endif

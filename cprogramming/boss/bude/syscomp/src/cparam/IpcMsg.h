#ifndef __IPC_MSG_H_
#define __IPC_MSG_H_


namespace budeipc {

	
	/**
	 *  消息结构基类型，此为通用类型
	 */
	struct IpcMsgBufBase {
		long m_type;
	};
	
	/**
	 *  此处定义的消息结构中有虚方法，因此此类消息对象中会有8个字节（long型）
	 *  的vtbl指针；此种消息结构能够正确工作的前提是vtbl指针在对象的内存布局
	 *  时是放置在对象成员之后的！！！
	 *  另，扩展消息对象时不可以定义类类型的数据成员（如string型别）！！！
	 */
	struct IpcMsgBuf : public IpcMsgBufBase {
		
		virtual int pmsgsz() { return 0; }
		virtual int gmsgsz() { return 1; }
	};
	

	/**
	 * interface IpcMsg definition
	 */
	class IpcMsg {
	public:
	    static int create(int key);
	    static void remove(int key);
	
	    static int isExist(int key);
	    static IpcMsg& getMsg(int key);
	
		virtual ~IpcMsg() {}
	    virtual long getkey() = 0;
		virtual long getipcid() = 0;
		virtual long getmsgcnt() = 0;
		virtual long getcbytes() = 0;
		virtual long getqbytes() = 0;
	
	    virtual int put(IpcMsgBufBase& msg, int msglen, int flag=0)=0;
	    virtual int vput(IpcMsgBuf& msg, int flag=0) = 0;
	    
	    virtual int get(IpcMsgBufBase& msg, int maxmsglen, int flag=0)=0;
	    virtual int vget(IpcMsgBuf& msg, int flag=0)=0;
	    
	    virtual int xget(IpcMsgBufBase& msg, int maxmsglen, int msgtype, int flag=0)=0;
	
	}; 

}   // namespace


#endif

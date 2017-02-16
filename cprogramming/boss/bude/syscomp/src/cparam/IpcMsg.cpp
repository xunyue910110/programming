#include "IpcMsg.h"

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

//#include "budeipc.h"
#include "IpcLocal.h"


namespace budeipc {
	
	/**
	 * class IpcMsgImpl definition
	 */
	class IpcMsgImpl : public budeipc::IpcMsg {
	public:
		
		IpcMsgImpl(int key) : m_key(key)
		{
			if ( (m_msgid = msgget(m_key, 0)) == -1 ) {
				IPCTHROW_C(IpcException, errno, "访问消息队列错误");
			}
		}
		
		virtual ~IpcMsgImpl() { }
		
		virtual long getkey() { return m_key; }	
		
		virtual long getipcid() { return m_msgid; }
		
		virtual long getmsgcnt() 
		{
			struct msqid_ds _mds;
			
			if (msgctl(m_msgid, IPC_STAT, &_mds) == -1) {
				IPCTHROW_C(IpcException, errno, "");
			}
			
			return _mds.msg_qnum;
		}
		
		virtual long getcbytes() 
		{
			struct msqid_ds _mds;
			
			if (msgctl(m_msgid, IPC_STAT, &_mds) == -1) {
				IPCTHROW_C(IpcException, errno, "");
			}
			
			return _mds.msg_cbytes;
		}
		
		virtual long getqbytes()
		{
			struct msqid_ds _mds;
			
			if (msgctl(m_msgid, IPC_STAT, &_mds) == -1) {
				IPCTHROW_C(IpcException, errno, "");
			}
			
			return _mds.msg_qbytes;
		}
		
		virtual int put(budeipc::IpcMsgBufBase& msg, int msglen, int flag) {
			if (msgsnd(m_msgid, &msg, msglen, 0|flag) == -1) {
				if (errno == EAGAIN) {
					return -1;
				} else {
					IPCTHROW_C(IpcException, errno, "发送消息错误");
				}
			}
			
			return 0;		
		}
		
		virtual int vput(budeipc::IpcMsgBuf& msg, int flag) {
			int _len = msg.pmsgsz();
			
			if (msgsnd(m_msgid, &msg, _len, 0|flag) == -1) {
				if (errno == EAGAIN) {
					return -1;
				} else {
					IPCTHROW_C(IpcException, errno, "发送消息错误");
				}
			}
			
			return 0;		
		}
	
		virtual int get(budeipc::IpcMsgBufBase& msg, int msglen, int flag) {
			if (msgrcv(m_msgid, &msg, msglen, 0, 0|flag) == -1) {
				if (errno == ENOMSG) {
					return -1;
				} else {
					IPCTHROW_C(IpcException, errno, "接收消息错误");
				}
			}
			
			return 0;
		}
	
		virtual int vget(budeipc::IpcMsgBuf& msg, int flag) {
			int _maxlen = msg.gmsgsz();
				
			if (msgrcv(m_msgid, &msg, _maxlen, 0, 0|flag) == -1) {
				if (errno == ENOMSG) {
					return -1;
				} else {
					IPCTHROW_C(IpcException, errno, "接收消息错误");
				}
			}
			
			return 0;
		}
	
		virtual int xget(budeipc::IpcMsgBufBase& msg, int msglen, int msgtype, int flag) {
			if (msgrcv(m_msgid, &msg, msglen, msgtype, 0|flag) == -1) {
				if (errno == ENOMSG) {
					return -1;
				} else {
					IPCTHROW_C(IpcException, errno, "接收消息错误");
				}
			}
			
			return 0;
		}
		
	private:
		
		int m_key;
		
		int m_msgid;	
	
	};
	
	
	///
	int budeipc::IpcMsg::create(int key)
	{
		int msgid;
		if ((msgid = msgget(key, IPC_CREAT | IPC_EXCL | 0660)) == -1) {
			IPCTHROW_C(IpcException, errno, "创建消息队列错误");
		}	
		
		return msgid;
	}
	
	void budeipc::IpcMsg::remove(int key)
	{
		int msgid;
		if ((msgid = msgget(key, 0)) != -1) {
			msgctl(msgid, IPC_RMID, (struct msqid_ds*)0);
		}
	}
	
	budeipc::IpcMsg& budeipc::IpcMsg::getMsg(int key)
	{
		static IpcMsgImpl stmsg(key);
		
		return stmsg;
	}
	
	int budeipc::IpcMsg::isExist(int key)
	{
		int msgid;
		if ((msgid = msgget(key, IPC_CREAT | IPC_EXCL)) == -1) {
			return 1;
		}
		msgctl(msgid, IPC_RMID, (struct msqid_ds*)0);
		
		return 0;
	}

} // namespace budeipc

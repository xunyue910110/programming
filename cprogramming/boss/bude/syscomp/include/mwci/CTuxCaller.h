//! Tuxedo Caller
/*!
 IMWCaller 类的 Tuxedo 实现
*/

#ifndef CTUXCALLER_H_HEADER_INCLUDED_BE5E83BF
#define CTUXCALLER_H_HEADER_INCLUDED_BE5E83BF
#include "mwci/IMWCaller.h"
#include "mwci/MWException.h"
#include "mwci/CFmlBuf.h"
#include "mwci/CNameService.h"

class CBuffer;

// Realize of Tuxedo Caller
//##ModelId=419036470048
class CTuxCaller : public IMWCaller
{
  public:

	///构造
    //##ModelId=4190B5BB0342
    CTuxCaller();

	///解析
    //##ModelId=4190B5BC0036
    virtual ~CTuxCaller();

    ///进行同步服务调用
	/**
		进行同步服务调用，如果发生任何错误，则抛出异常。
		按这个格式调用是线程安全的，如果需要同事调用多台不同服务器上的服务，需要调用基类的SetMultiCtxt方法。
		此函数调用提供本地服务缓存功能。
		\param sSrvName 逻辑服务名
		\param DataIn   输入数据
		\param DataOut  服务调用返回数据
		\param Flag     详见Tuxedo文档，一般情况下使用默认值就可以了
		\param bIfUseNaming 是否使用名字服务
	*/
    //##ModelId=41A15D7E0132
    virtual int SynCall(const string &sSrvName, CBuffer& DataIn, CBuffer& DataOut, long Flag = 0L,bool bIfUseNaming=true) throw(CTuxException);


    ///进行异步服务调用
	/**
		进行同步服务调用，如果发生任何错误，则抛出异常。
		按这个格式调用是线程安全的，但是必须要自行记录下函数返回的异步调用ID。
		如果需要同时调用多台不同服务器上的服务，需要调用基类的SetMultiCtxt方法。
		此函数调用不提供本地服务缓存功能。
		\param sSrvName 逻辑服务名
		\param DataIn   输入数据
		\param Flag     祥见Tuxedo文档，一般情况下使用默认值就可以了
		\param bIfUseNaming 是否需要进行名字解析。
			- true 默认，需要将sSrvName,转换成物理服务名并进行调用，需要的时候会更据Naming的配置信息进行服务器连接操作
			- false 将sSrvName做为物理服务名进行调用，并且不进行服务器连接操作;
					此参数设置后不会将CallID记录在Caller实例内部
			        如果要在服务内部调用其他服务,必须将次参数设置成false
		\return 返回值为本次异步调用的ID
		\sa AsyGetReply()
	*/
    //##ModelId=41A15D7E02A5
    virtual int AsyCall(const string &sSrvName, CBuffer& DataIn, long Flag = 0L, bool bIfUseNaming = true) throw(CTuxException);

    ///获取异步服务调用的返回值
	/**
		进行同步服务调用，如果发生任何错误，则抛出异常
		按这个格式调用是线程安全的
		如果需要同时调用多台不同服务器上的服务，需要调用基类的SetMultiCtxt方法
		\param DataOut  输出数据
		\param callid 因该填写之前调用AsyCall的返回值，
					  如果使用默认值，则使用上次AsyCall时是记录在Caller内部的Callid
		\param Flag   祥见Tuxedo文档，一般情况下使用默认值就可以了
		\return 返回值在tuxedo Call中无实际意义
		\sa AsyCall()
	*/
    //##ModelId=41A15D7F0007
    virtual int AsyGetReply(CBuffer& DataOut, int callid=-1, long Flag = 0L) throw(CTuxException);

    ///获取异步服务调用的返回值
	/**
		进行同步服务调用，如果发生任何错误，则抛出异常
		此函数调用提供本地服务缓存功能。
		按这个格式调用是线程不安全的
		如果需要同时调用多台不同服务器上的服务，需要调用基类的SetMultiCtxt方法
		\return 返回值在tuxedo Call中无实际意义
		\sa AsyCall()
	*/
    //##ModelId=41A599A40111
     virtual int AsyGetReply() throw(MWException);

	///建立会话调用连接
	/**
		建立会话服务调用，如果发生任何错误，则抛出异常
		此函数调用不提供本地服务缓存功能。
		按这个格式调用是线程安全的，但是在后续的调用中必须记录下本次调用的Callid
		如果需要同时调用多台不同服务器上的服务，需要调用基类的SetMultiCtxt方法
		\param sSrvName 逻辑服务名
		\param DataIn 发送给服务器端的数据，如果建立连接时不需要，则使用默认值
		\param Flag 一般使用默认值即可，详细如下
			- TPNOTRAN  If the caller is in transaction mode and this flag is set, then when svc is invoked, it is not performed on behalf of the caller's transaction. Note that svc may still be invoked in transaction mode but it will not be the same transaction: a svc may have as a configuration attribute that it is automatically invoked in transaction mode. A caller in transaction mode that sets this flag is still subject to the transaction timeout (and no other). If a service fails that was invoked with this flag, the caller's transaction is not affected.
			- TPSENDONLY The caller wants the connection to be set up initially such that it can only send data and the called service can only receive data (that is, the caller initially has control of the connection). Either TPSENDONLY or TPRECVONLY must be specified.
			- TPRECVONLY The caller wants the connection to be set up initially such that it can only receive data and the called service can only send data (that is, the service being called initially has control of the connection). Either TPSENDONLY or TPRECVONLY must be specified.
			- TPNOBLOCK The connection is not established and the data is not sent if a blocking condition exists (for example, the data buffers through which the message is sent are full). Note that this flag applies only to the send portion of tpconnect(); the function may block waiting for an acknowledgement from the server. When TPNOBLOCK is not specified and a blocking condition exists, the caller blocks until the condition subsides or a blocking timeout or transaction timeout occurs.
			- TPNOTIME This flag signifies that the caller is willing to block indefinitely and wants to be immune to blocking timeouts. Transaction timeouts will still affect the program.
			- TPSIGRSTRT If a signal interrupts any underlying system calls, then the interrupted call is reissued.
		\return 返回值  Call id
		\sa ConvsSend(),ConvsRecv(),ConvsDisConn()
	*/
    //##ModelId=41A15D7F0170
    virtual int ConvsConn(const string &sSrvName, CBuffer* DataIn = NULL, long Flag = -1L) throw(CTuxException);

	///在服务器/客户机之间发送信息
	/**
		如果发生任何错误，则抛出异常
		此函数调用不提供本地服务缓存功能。
		按这个格式调用是线程安全的，但是在必须指定之前调用ConvsConn返回的Callid
		如果需要同时调用多台不同服务器上的服务，需要调用基类的SetMultiCtxt方法
		\param DataIn 发送给服务器端的数据
		\param connetid ConvsConn时返回的Call ID ,如果使用默认值则对最后一次ConvsConn的Call ID
		\param event 一般使用默认值就可以了，详细如下
			- TPEV_DISCONIMM Received by the subordinate of a conversation, this event indicates that the originator of the conversation has issued an immediate disconnect on the connection .
			- TPEV_SVCERR Received by the originator of a conversation, this event indicates that the subordinate of the conversation has issued tpreturn() without having control of the conversation.
			- TPEV_SVCFAIL Received by the originator of a conversation, this event indicates that the subordinate of the conversation has issued tpreturn() without having control of the conversation.
		\param Flag 一般使用默认值即可，详细如下
			- TPRECVONLY This flag signifies that, after the caller's data is sent, the caller gives up control of the connection (that is, the caller can not issue any more tpsend() calls). When the receiver on the other end of the connection receives the data sent by tpsend(), it will also receive an event (TPEV_SENDONLY) indicating that it has control of the connection (and can not issue more any tprecv() calls).
			- TPNOBLOCK The data and any events are not sent if a blocking condition exists (for example, the internal buffers into which the message is transferred are full). When TPNOBLOCK is not specified and a blocking condition exists, the caller blocks until the condition subsides or a timeout occurs (either transaction or blocking timeout).
			- TPNOTIME This flag signifies that the caller is willing to block indefinitely and wants to be immune to blocking timeouts. Transaction timeouts may still occur.
			- TPSIGRSTRT If a signal interrupts any underlying system calls, then the interrupted system call is reissued.
		\return 返回值无意义
		\sa ConvsConn(),ConvsRecv(),ConvsDisConn()
	*/
    //##ModelId=41A15D7F02E3
    virtual int ConvsSend(CBuffer& DataIn, int connetid=-1,long *event = NULL, long Flag = -1L) throw(CTuxException);

	///在服务器/客户机之间接受信息
	/**
		如果发生任何错误，则抛出异常
		此函数调用不提供本地服务缓存功能。
		按这个格式调用是线程安全的，但是在必须指定之前调用ConvsConn返回的Callid
		如果需要同时调用多台不同服务器上的服务，需要调用基类的SetMultiCtxt方法
		\param DataOut 接收到的数据
		\param connetid ConvsConn时返回的Call ID ,如果使用默认值则对最后一次ConvsConn的Call ID
		\param event 一般使用默认值就可以了，详细如下
			- TPEV_DISCONIMM Received by the subordinate of a conversation, this event indicates that the originator of the conversation has issued an immediate disconnect on the connection .
			- TPEV_SENDONLY The program on the other end of the connection has relinquished control of the connection. The recipient of this event is allowed to send data but cannot receive any data until it relinquishes control.
			- TPEV_SVCSUCC Received by the originator of a conversation, this event indicates that the subordinate service on the other end of the conversation has finished successfully as defined by the application (that is, it called tpreturn() with TPSUCCESS).
			- TPEV_SVCERR Received by the originator of a conversation, this event indicates that the subordinate of the conversation has issued tpreturn() without having control of the conversation.
			- TPEV_SVCFAIL Received by the originator of a conversation, this event indicates that the subordinate of the conversation has issued tpreturn() without having control of the conversation.
		\param Flag 一般使用默认值即可，详细如下
			- TPNOCHANGE When this flag is set, the type of the buffer pointed to by *data is not allowed to change.
			- TPNOBLOCK If data is already available to receive, then tprecv() gets the data and returns. When this flag is not specified and no data is available to receive, the caller blocks until data arrives.
			- TPNOTIME This flag signifies that the caller is willing to block indefinitely and wants to be immune to blocking timeouts. Transaction timeouts will still affect the program.
			- TPSIGRSTRT If a signal interrupts the underlying receive system call, then the call is reissued.
		\return 返回值无意义
		\sa ConvsConn(),ConvsSend(),ConvsDisConn()
	*/
    //##ModelId=41A15D8000E5
    virtual int ConvsRecv(CBuffer& DataOut, int connetid=-1,long *event = NULL, long Flag = -1L) throw(CTuxException);

    /// Defining the Event-based Message Handler
    //##ModelId=41A15D8002EE
    //virtual int SetMsgHandler(MyFunc  func) throw(CTuxException);
    virtual int SetMsgHandler(void(_LKENTER *disp)(char *, long, long) ) throw(CTuxException);

    /// Checking for Event-based Messages
    //##ModelId=41A15D8003B6
    virtual int ChkMsg(void) throw(CTuxException);

    /// Subscribing to Events in Event-based Messages mode.
    //##ModelId=41A15D810079
    virtual long Subscribe(char *eventexpr, char *filter, void *ctl, long Flag = 0L) throw(CTuxException);

    /// Unsubscribing to Events in Event-based Messages mode.
    //##ModelId=41A15D81029F
    virtual int Unsubscribe(long handle, long Flag = 0L) throw(CTuxException);

    /// Posting Events in Event-based mode.
    //##ModelId=41A15D82000C
    virtual int Post(char *eventname, CBuffer& Data, long flags = 0L ) throw(CTuxException);

    /// Broadcasting Messages by Name
    //##ModelId=41A15D82020B
    virtual int Broadcast(char *lmid, char *usrname, char *cltname, CBuffer& Data, long flags = 0L ) throw(CTuxException);

    /// Broadcasting Messages by Identifier
    //##ModelId=41A15D830144
    virtual int Notify(void *clientid, CBuffer& Data, long flags = 0L ) throw(CTuxException);

    /// Starting the Transaction
    //##ModelId=41A15D83032E
    virtual int TransBegin(unsigned long timeout, long flags = 0L ) throw(CTuxException);

    /// Suspending a Transaction
    //##ModelId=41A15D8400FF
    virtual int TransSuspend() throw(CTuxException);

    /// Resuming a Transaction
    //##ModelId=41A15D840312
    virtual int TransResume() throw(CTuxException);

    /// Committing the Current Transaction
    //##ModelId=41A15D8500EC
    virtual int TransCommit(long flags=0L) throw(CTuxException);

    /// Aborting the Current Transaction
    //##ModelId=41A15D850205
    virtual int TransAbort(long flags=0L) throw(CTuxException);

    /// Enqueuing Messages in Queue Message process mode.
    //##ModelId=41A15D850331
    virtual int Enqueue(char *qspace, char *qname, void *ctl, CBuffer& Data, long flags=0L) throw(CTuxException);

    /// Dequeuing Messages in Queue Message process mode.
    //##ModelId=41A15D860347
    virtual int Dequeue(char *qspace, char *qname, void *ctl, CBuffer& Data, long flags=0L) throw(CTuxException);

    /// 服务调用返回
	/**
	  In Request/Response Communication Mode (That is synchronously call using
      SynCall or asynchronously call using AsyCall ) it's sending Replies.
      When you use Return in Conversation Mode, it's alway mean that the
      conversation is end.
	  \param rval
		- TPSUCCESS 服务成功
		- TPFAIL 服务失败
	  \param rcode 自定义的返回代码
	  \param Data 返回的数据
	  \param flags 一般使用默认值
	*/
    //##ModelId=41A15D8801A5
    virtual int Return(int rval, long rcode, CBuffer& Data, long flags = 0L);

    /// 服务转移
	/**
		Forwarding a service request to another service
		\param svc 服务名成
		\param Data 数据
		\param flags 一般使用默认值
	*/
    //##ModelId=41A15D8903C3
    virtual int Forward(char *svc, CBuffer& Data, long flags = 0L) throw(CTuxException);

    ///没有使用
    //##ModelId=41A15D8B011D
    virtual int Set(int iValId, void *data) throw(CTuxException);

	///连接到服务器
	/**
		在一般情况下,不需要显示的调用这个函数,Caller 会自己更据Naming信息,寻找服务存在的IP地址
		和端口号,并且在需要的时候进行连接操作.
		\param sIP 服务器IP地址 ,sIP也可以使包含冒号和端口号的字符串
				   如果sIP为空,则不会进行任何连接
		\param sPort 服务器端口号,如果端口号为空,则只更据sIP进行连接
	*/
    //##ModelId=41A15D8C0327
    virtual int Connect(const string &sIP="", const string &sPort="") throw(CTuxException);

    //------------------------
    // Common Function
    //------------------------
	///判断是否已经连接服务器
    //##ModelId=41A15D8D02F7
	virtual bool IsConnect();

    ///断开与服务器的连接
    //##ModelId=41A15D8D03C9
    virtual bool DisConnect(void) throw(CTuxException);

    ///断开与会话服务程序的连接,但是并不会断开与服务器的连接
	/**
		\param cd 要断开的会话连接ID,如果不指定，则端开最后一次的连接
	*/
    //##ModelId=41A15D8E01CC
    virtual bool ConvsDisConn(int cd=-1) throw(CTuxException);

	///设置连接数据
	/**
		在多线程环境下不要使用这个函数
		\param sSrvName 逻辑服务名
		\param Data 输入数据
	*/
    //##ModelId=41A15D8F0000
    virtual int SetData(const string &sSrvName, CBuffer& Data) throw(CTuxException);

	///获取数据
	/**
		获取先前调用返回的数据
		\param Data 返回的数据
	*/
    //##ModelId=41A15D8F0368
    virtual int GetData(CBuffer& Data) throw(MWException);

    ///Testing Whether a Transaction Has Started
    //##ModelId=41A15D9001BB
    virtual bool IsInTrans();

    /// 放弃一个异步调用
	/**
		\param asyid 异步调用ID号
	*/
    //##ModelId=41A15D9002A1
    virtual int CancelAsyCall(int asyid=-1) throw(CTuxException);

	///没有用
    //##ModelId=41A15D9100EA
    virtual int GetErrorInfo(int iErrorCode);

	///进行同步调用
    /**
		事先使用 SetData() 设置相关数据
		之后使用 GetData() 获取服务调用返回值
		线程不安全
		\sa SetData(),GetData()
	*/
    //##ModelId=41A15D92024A
    virtual int SynCall();

    ///进行异步调用
    //##ModelId=41A15D920344
    virtual int AsyCall();

	///设置环境Tuxedo环境变量
    //##ModelId=41ABEDB6029A
    virtual  void PutEnv(char *env);

	///获取环境Tuxedo环境变量
    //##ModelId=41ABEDD40257
    virtual  char *GetEnv(char *envName);

    ///进行同步服务调用
	/**
		进行同步服务调用，如果发生任何错误，则抛出异常。
		按这个格式调用是线程安全的，如果需要同事调用多台不同服务器上的服务，需要调用基类的SetMultiCtxt方法。
		此函数调用提供本地服务缓存功能。
		\param sSrvName 逻辑服务名
		\param DataIn   输入数据
		\param DataOut  服务调用返回数据
	*/
    //##ModelId=41A15D7E0132
    virtual int InterSynCall(const string &sSrvName, CBuffer& DataIn, CBuffer& DataOut) throw(CTuxException);

	/**
		进行同步服务调用，如果发生任何错误，则抛出异常。
		按这个格式调用是线程安全的，如果需要同事调用多台不同服务器上的服务，需要调用基类的SetMultiCtxt方法。
		此函数调用提供本地服务缓存功能。
		\param sSrvName 逻辑服务名
		\param DataIn   输入数据
		\param DataOut  服务调用返回数据
	*/
    //##ModelId=41A15D7E0132
    virtual int ExternalSynCall(const string &sSrvName, CBuffer& DataIn, CBuffer& DataOut) throw(CTuxException);

  protected:

	///内部处理Naming服务
	/**
		@param	LogicalName		输入，逻辑服务名
		@param	SvcName			输出，物理服务名
		@param	SvcInfo			输出，物理服务所在地址和端口号
		@param	IfNeedLocal		输出，物理服务是否需要本地缓存
		@param	IfFromRemote	输出，物理服务名是否从远程获取
		@param	IfNeedLNS		输入，是否需要进行本地名字服务查找
	*/
    //##ModelId=41C96FD403C5
	bool ProcessNamingInfo(const string & LogicalName,string& SvcName,string& SvcInfo,bool& IfNeedLocal,bool * IfFromRemote,bool IfNeedLNS);

	///进行本地服务缓存处理
	/**
		@param type - LLSVC_LOOKUP 本地缓存查找
					- LLSVC_SAVE   本地缓存保存
		@param Input	输入数据
		@param Output	输出数据
		@param LogicName	逻辑名
	*/
    //##ModelId=41C96FD502D6
	bool ProcessLocalService(int type,CBuffer& Input,CBuffer& Output,const STD string& LogicName);

	///进行连接服务器
	/**
		在需要的时候会进行连接操作。
		@param connectInfo 服务器地址("IP:PORT")
		@param bIfNaming 是否是名字服务在使用，表示
	*/
    //##ModelId=41C96FD6015B
	bool ProcessConnect(string& connectInfo,bool bIfNaming=false);



  private:

     //##ModelId=4190B5BB036A
    CTuxCaller(const CTuxCaller& right);

    //##ModelId=41A15D8C00CE
    virtual CTuxCaller& operator=(const CTuxCaller& right);

	///在Mutli Context环境下保存本次连接的环境信息
    //##ModelId=4190B5280029
    long m_lTuxedoCtxt;

	///保存最后一次会话ID
    //##ModelId=41A599A3005B
    int m_ConnectID;

	///保存最后一次异步调用的ID
    //##ModelId=41A599A30142
    int m_AsyCallID;

	///保存最后一次调用的返回缓冲区大小
    //##ModelId=41A599A301EC
    long m_lReturnBufSize;

	///是否处于调试模式
    //##ModelId=41AB1374039F
	static bool m_bDebugTag;

	static bool m_bDebugNoBuf;

	///保存最后一次事务ID
    //##ModelId=41A599A30296
    TPTRANID m_tranID;

	///存放SetData()设置的输入数据
    //##ModelId=41A599A30340
    CFmlBuf m_InBuf;

	///存放最后一次服务调用返回的数据
    //##ModelId=41A599A30373
    CFmlBuf m_OutBuf;

	///最后一次远端服务器地址信息
    //##ModelId=41C96FD30265
    string m_LastRemoteInfo;

	///最后一次调用的逻辑服务名
    //##ModelId=41D1637002F3
	string m_LastLogicalName;

	///最后一次调用是否需要本地服务
    //##ModelId=41C96FD3034B
    bool m_bLastNeedLocal;

	///最后一次本地服务缓冲操作是否成功
    //##ModelId=41C96FD4000D
    bool m_bLastLocalOperOK;

};



#endif /* CTUXCALLER_H_HEADER_INCLUDED_BE5E83BF */

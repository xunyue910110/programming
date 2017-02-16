#ifndef IMWCALLER_H_HEADER_INCLUDED_BE67D5FD
#define IMWCALLER_H_HEADER_INCLUDED_BE67D5FD
#include "mwci/MWException.h"
#include "mwci/CBuffer.h"
#include "mwci/MWGlobal.h"

//class CNameService;
//class CLocalService;
// Interface of Mid-Ware Caller System
//
// TxRPC is not supported!
// security base on Public Key is not supported!
//##ModelId=419036A4031D
class IMWCaller
{
  public:


    /// Sending Synchronous Messages
    //##ModelId=4190411E0283
    virtual int SynCall(const string &sSrvName, CBuffer& DataIn, CBuffer& DataOut, long Flag = 0L,bool bIfUseNaming=true) throw(MWException) = 0;

    virtual  int InterSynCall(const string &sSrvName, CBuffer& DataIn, CBuffer& DataOut) throw(MWException) = 0;
    virtual  int ExternalSynCall(const string &sSrvName, CBuffer& DataIn, CBuffer& DataOut) throw(MWException) = 0;
    /// Sending Asynchronous Messages
    //##ModelId=419044A30347
    virtual int AsyCall(const string &sSrvName, CBuffer& DataIn, long Flag=0L, bool bIfUseNaming = true) throw(MWException) = 0;

    /// Getting an Asynchronous Reply
    //##ModelId=41905E450274
    virtual int AsyGetReply( CBuffer& DataOut,int callid=-1, long Flag = 0L) throw(MWException) = 0;

    /// Getting an Asynchronous Reply
    //##ModelId=41A599A603B3
    virtual int AsyGetReply() throw(MWException) = 0;

    /// sets up a conversation
    //##ModelId=419061CA0388
    virtual int ConvsConn(const string &sSrvName, CBuffer* DataIn = NULL, long Flag = -1L) throw(MWException) = 0;

    /// Sending Messages between conversation server and client
    //##ModelId=419063170282
    virtual int ConvsSend(CBuffer& DataIn, int connectid=-1,long *event = NULL, long Flag = -1L) throw(MWException) = 0;

    /// Sending Messages between conversation server and client
    //##ModelId=419063570248
    virtual int ConvsRecv(CBuffer& DataOut,int connectid=-1, long *event = NULL, long Flag = -1L) throw(MWException) = 0;

    /// Defining the Event-based Message Handler
    //##ModelId=419068B701C5
    virtual int SetMsgHandler(void (_LKENTER *)(char *, long, long)) throw(MWException) = 0;

    /// Checking for Event-based Messages
    //##ModelId=4190697B0054
    virtual int ChkMsg(void) throw(MWException) = 0;

    /// Subscribing to Events in Event-based Messages mode.
    //##ModelId=41906B3D02BE
    virtual long Subscribe(char *eventexpr, char *filter, void *ctl, long Flag = 0L) throw(MWException) = 0;

    /// Unsubscribing to Events in Event-based Messages mode.
    //##ModelId=41906B810334
    virtual int Unsubscribe(long handle, long Flag = 0L) throw(MWException) = 0;

    /// Posting Events in Event-based mode.
    //##ModelId=41906C34011F
    virtual int Post(char *eventname, CBuffer& Data, long Flag = 0L) throw(MWException) = 0;

    /// Broadcasting Messages by Name
    //##ModelId=41906C7F0343
    virtual int Broadcast(char *lmid, char *usrname, char *cltname, CBuffer& Data,  long Flag = 0L) throw(MWException) = 0;

    /// Broadcasting Messages by Identifier
    //##ModelId=41906C950287
    virtual int Notify(void *clientid, CBuffer& Data, long Flag = 0L) throw(MWException) = 0;

    /// Starting the Transaction
    //##ModelId=41906D0E03DF
    virtual int TransBegin(unsigned long timeout, long  Flag = 0L) throw(MWException) = 0;

    /// Suspending a Transaction
    //##ModelId=41906D1B0261
    virtual int TransSuspend() throw(MWException) = 0;

    /// Resuming a Transaction
    //##ModelId=41906D43033B
    virtual int TransResume() throw(MWException) = 0;

    /// Committing the Current Transaction
    //##ModelId=41906D750134
    virtual int TransCommit(long flags=0L) throw(MWException) = 0;

    /// Aborting the Current Transaction
    //##ModelId=41906DB601C4
    virtual int TransAbort(long flags=0L) throw(MWException) = 0;

    /// Enqueuing Messages
    /** in Queue Message process mode.
	*/
    //##ModelId=41906EB00155
    virtual int Enqueue(char *qspace, char *qname, void *ctl, CBuffer& Data, long  Flag = 0L) throw(MWException) = 0;

    // Dequeuing Messages
    /** in Queue Message process mode.
	*/
    //##ModelId=41906F0C018A
    virtual int Dequeue(char *qspace, char *qname, void *ctl, CBuffer& Data, long  Flag = 0L) throw(MWException) = 0;

	/// Service return to Client
    /** In Request/Response Communication Mode (That is synchronously call using
       SynCall or asynchronously call using AsyCall ) it's sending Replies.
       When you use Return in Conversation Mode, it's alway mean that the
       conversation is end.
	*/
    //##ModelId=419072EB02E3
    virtual int Return(int rval, long rcode, CBuffer& Data, long  Flag = 0L ) = 0;

    /// Forwarding a service request to another service
    //##ModelId=4190758500F6
    virtual int Forward(char *svc, CBuffer& Data, long  Flag = 0L ) throw(MWException) = 0;

    /// Set variables.
    //##ModelId=419076C900E8
    virtual int Set(int iValId, void *data) throw(MWException) = 0;

	///Destory
    //##ModelId=4190B04101BF
    virtual ~IMWCaller();

	///=
    //##ModelId=4190B0410237
    virtual IMWCaller& operator=(const IMWCaller& right) ;

	///Connect to Server
    //##ModelId=4190D35003A5
    virtual int Connect(const string &sIP="", const string &sPort="") throw(MWException) = 0;

    //------------------------
    // Common Function
    //------------------------
    /// Is Connected to server?
	//##ModelId=419314470316
    virtual bool IsConnect();

    /// Leaving Server
    //##ModelId=419366E30248
    virtual bool DisConnect(void) throw(MWException) = 0;

    /// Disconnect conversation
    //##ModelId=419315730213
    virtual bool ConvsDisConn(int cd) throw(MWException) = 0;

	/// Set data for future use
    //##ModelId=419316B601B3
    virtual int SetData(const string &sSrvName, CBuffer& Data) throw(MWException) = 0;

	/// Get data from the last CALL
    //##ModelId=419316C70013
    virtual int GetData(CBuffer& Data) throw(MWException) = 0;

    /// Testing Whether a Transaction Has Started
    //##ModelId=419317E503DF
    virtual bool IsInTrans() = 0;

    /// Cancel a Asy. Call
    //##ModelId=41935EE003BC
    virtual int CancelAsyCall(int asyid=-1) throw(MWException) = 0;

    ///Get Error Info
	//##ModelId=41935EE100BA
    virtual int GetErrorInfo(int iErrorCode) = 0;

    /// make a syn. Call
    //##ModelId=4193659000E2
    virtual int SynCall() = 0;

    /// make a Asyn. call
    //##ModelId=41936590016F
    virtual int AsyCall() = 0;

    /// 获取事前设置的客户端的名称
	//##ModelId=419D92760392
    string GetCltname() const;

	/// 设置客户端的名称
	/**
		应该在首次调用之前进行设置
		 \param left 客户端的名称
	*/
    //##ModelId=419D9277009A
    void SetCltname(string &left);

	/// 获取客户端的名称
	/**

		 \return  客户端的名称
	*/
    //##ModelId=419D927701E5
    string GetGrpname() const;

	/// 设置客户组的名称
	/**
		应该在首次调用之前进行设置
		 \param left 客户组的名称
	*/
    //##ModelId=419D9277028F
    void SetGrpname(string &left);

 	/// 获取客户端的密码
	/**

		 \return  客户端的密码
	*/
    //##ModelId=419D927703BB
    string GetPasswd() const;

 	/// 设置客户密码
	/**
		应该在首次调用之前进行设置
		 \param left 客户客户密码
	*/
    //##ModelId=419D9278007E
    void SetPasswd(string &left);

	///获取服务器IP地址信息
    //##ModelId=419D927801B4
    string GetSrvIP() const;

	///设置服务器IP地址信息
    //##ModelId=419D9278025E
    void SetSrvIP(string &left);

	///获取服务器端口信息
    //##ModelId=419D92780395
    string GetSrvPort() const;

    ///设置是否使用多上下文(Multi Contex)模式
	/**
		如果使用多上下文(Multi Contex)模式,则该客户端所有Caller
		都必须设置该属性
		在第一次调用服务前设置
	*/
	//##ModelId=41D1637101FA
	void SetMultiCtxt(bool var=true)
	{
		m_bMTFlag=var;
	}

	///获取是否使用多上下文(Multi Contex)模式
    //##ModelId=41D16371038B
	bool GetMulitCtxt(void)
	{
		return m_bMTFlag;
	};

	///设置服务器端口信息
    //##ModelId=419D92790057
    void SetSrvPort(string &left);

	///获取用户名
    //##ModelId=419D92790197
    string GetUsrname() const;

	///设置用户名
    //##ModelId=419D92790242
    void SetUsrname(string &left);

    ///获取中间件系统的环境变量
	//##ModelId=41ABED0000AD
    virtual char *GetEnv(char *envName) = 0;

	///设置中间件系统的环境变量
    //##ModelId=41ABED1103BF
    virtual void PutEnv(char *env) = 0;



  protected:

	///服务器
    //##ModelId=4190B0DB0148
    string m_sSrvIP;

    //##ModelId=4190B0EF006B
    string m_sSrvPort;

    /// 多上下文标志
    /**
		- true 为支持多上下文
		- fals e为不支持多上下文
	*/
    //##ModelId=4190B2FA0211
    bool m_bMTFlag;

    /// 连接标志
    /**
		- true为已经连接
		- false为没有连接
	*/
    //##ModelId=4190B4070041
    bool m_bIsConn;

	///存放客户端的用户名
    //##ModelId=4193190500D6
    string m_sUsrname;

	///存放客户端名
    //##ModelId=419C5E1F0344
    string m_sCltname;

	///存放客户端密码
    //##ModelId=419C5E2501B2
    string m_sPasswd;

	///存放组名
    //##ModelId=419C5E29028A
    string m_sGrpname;

	///存放服务名
    //##ModelId=41A599A50361
    string m_sSvcName;

	///日志文件句柄
    //##ModelId=41D16E4C00E8
	static FILE* m_LogFilefp;
};



#endif /* IMWCALLER_H_HEADER_INCLUDED_BE67D5FD */

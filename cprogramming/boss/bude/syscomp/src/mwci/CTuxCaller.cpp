#include "mwci/CTuxCaller.h"
#include "mwci/CBuffer.h"
#include <stdarg.h>
//#define _TUXCALLER_STATIC_LOG 定义是否每次都关闭文件
#ifdef BORLANDC_CONFIG
#include "mwci/localservice.h"
#include <vcl>
#else
//BORLAND C 不支持 time.h 郁闷!
#include <time.h>
#endif

#include "log4cpp/Logger.h"
using namespace log4cpp;

static Logger& logger = Logger::getLogger("gboss.scom.mwci.CTuxCaller");

///构造函数
//##ModelId=4190B5BB0342
CTuxCaller::CTuxCaller()
{

 	m_lTuxedoCtxt = -1;
	m_bIsConn=false;
	m_bMTFlag=false;
	m_ConnectID=-1;
	m_AsyCallID=-1;
	m_lReturnBufSize=-1;
    m_bLastNeedLocal=false;
    m_bLastLocalOperOK=false;
}

///不支持复制构造
//##ModelId=4190B5BB036A
CTuxCaller::CTuxCaller(const CTuxCaller& right)
{
	//this->operator=( right );
}

//解析函数
//##ModelId=4190B5BC0036
CTuxCaller::~CTuxCaller()
{
	FILE * fp=m_LogFilefp;
	if (m_LogFilefp)
	{
		fp=m_LogFilefp;
		m_LogFilefp=NULL;
		fclose(fp);
	}
}

//##ModelId=41A15D7E0100
int CTuxCaller::Connect(const string &sIP, const string &sPort) throw(CTuxException)
{
	char chInfo[128];
	//设置TUXEDO环境变量
	if(!sIP.empty() )
	{
		string strInfo = "WSNADDR=//";
		if (sPort.empty())
			strInfo += sIP;
		else
			strInfo += sIP + ":" + sPort;

		if( strInfo.size() >= 128 )
			THROW_C_P1(CTuxException,TUX_WSNADDR_TOO_LONG,"invaild length[%s]",strInfo.c_str());

		strcpy( chInfo, strInfo.c_str() );
		if( tuxputenv( chInfo ) == -1 )
			THROW_C_P2(CTuxException,tperrno,"[detail:[%d][%s]]",tperrno,tpstrerror(tperrno));
	}

	TPINIT *tpinfo = (TPINIT*)tpalloc( (char*)"TPINIT", NULL, sizeof(TPINIT) );
	if( tpinfo == NULL )
		THROW_C_P2(CTuxException,tperrno,"[detail:[%d][%s]]",tperrno,tpstrerror(tperrno));


	if( this->m_bMTFlag )
	{
		LOG_DEBUG(logger," |- [MULTICONTEXTS] Connection!");
		tpinfo->flags = TPMULTICONTEXTS;
	}

	strcpy(tpinfo->passwd, this->m_sPasswd.c_str() );
	strcpy(tpinfo->usrname, this->m_sUsrname.c_str() );
	strcpy(tpinfo->cltname, this->m_sCltname.c_str() );
	strcpy(tpinfo->grpname, this->m_sGrpname.c_str() );

	//连接到TUXEDO服务器
	if( tpinit( tpinfo ) == -1 )
	{
		if(tpinfo)
			tpfree( (char*)tpinfo );
		THROW_C_P2(CTuxException,tperrno,"[detail:[%d][%s]]",tperrno,tpstrerror(tperrno));
	}
	LOG_DEBUG_P1(logger," |- [tpinit] %s",chInfo);
	if( this->m_bMTFlag )
	{
		if( tpgetctxt( (TPCONTEXT_T*)&this->m_lTuxedoCtxt, 0 ) < 0 ){
			if(tpinfo)
				tpfree( (char*)tpinfo );
			THROW_C_P2(CTuxException,tperrno,"[detail:[%d][%s]]",tperrno,tpstrerror(tperrno));
		}
		else
			LOG_DEBUG_P1(logger," |- [MULTICONTEXTS] tpgetctxt %d",m_lTuxedoCtxt);
	}

	if( tpinfo != NULL )
	{
		tpfree( (char*)tpinfo );
	}

	m_LastRemoteInfo=chInfo+10;
	m_bIsConn=true;

	return 0;
}

//##ModelId=41A15D7E0132
int CTuxCaller::SynCall(const string &sSrvName, CBuffer& DataIn, CBuffer& DataOut, long Flag,bool bIfUseNaming) throw(CTuxException)
{
	int iRet;
	bool IfNeedLocal=false;
	string SvcName;
	string SvcInfo;
	bool IfNamingFromRemote=false;

	LOG_DEBUG_P4(logger,"[SynCall] LogicalName:%s InBufSize:%d Out_Buf_Size:%d Flag:%d",sSrvName.c_str(),DataIn.GetBufSize(),DataOut.GetBufSize(),Flag);
	LOG_TRACE_P1(logger,"CFmlBuf\n:%s",((CFmlBuf*)&DataIn)->ToString().c_str());

	if(bIfUseNaming)
	{
		if(!ProcessNamingInfo(sSrvName, SvcName,SvcInfo, IfNeedLocal,&IfNamingFromRemote,true))
			THROW_C(CTuxException,LNS_OPERFAIL,"Local Name Service Operate Failed");
		else
			LOG_DEBUG_P3(logger," |- Service:%s SRV_ADDR:%s NeedLocalService:%d",SvcName.c_str(),SvcInfo.c_str(),IfNeedLocal);
	}
	else
	{
		SvcName=sSrvName;
		LOG_DEBUG(logger," |- No Naming request!");
	}

	if (IfNeedLocal)
	{
		if(ProcessLocalService(LLSVC_LOOKUP, DataIn, DataOut,sSrvName))
			return 0;
		else
			LOG_DEBUG(logger," |- Local Service Lookup Fail!(Connecting Remote...)");
	}
	if(bIfUseNaming)
		if(!this->ProcessConnect(SvcInfo))
		{
			LOG_DEBUG_P1(logger," |- ProcessConnect Fail!(%s)",SvcInfo.c_str());
			THROW_C(CTuxException,LNS_CONNECT_FAIL,"Local Name Service Connect Failed");
		}

	if( this->m_bMTFlag )
	{
		LOG_DEBUG_P1(logger," |- [MULTICONTEXTS] tpsetctxt:%d",m_lTuxedoCtxt);
		if(tpsetctxt(this->m_lTuxedoCtxt, 0L)==-1)
			THROW_C_P2(CTuxException,tperrno,"[detail:[%d][%s]]",tperrno,tpstrerror(tperrno));
	}


	iRet=tpcall((char *)SvcName.c_str(),(char *)DataIn.GetData(),0L,(char **)DataOut.GetBufRef(),&m_lReturnBufSize,Flag);

	if (iRet==-1 )
	{
		///如果是从远程服务器上取得的名字或者如果不是TPENOENT的错误直接返回
		if(IfNamingFromRemote || tperrno!=TPENOENT || !bIfUseNaming)
			THROW_C_P2(CTuxException,tperrno,"[detail:[%d][%s]]",tperrno,tpstrerror(tperrno));
		///////////////////////////////////////////////////////////////////
		///开始再次从远程获取名字
		///////////////////////////////////////////////////////////////////
		LOG_DEBUG(logger," |-=- tpcall TPENOENT Try remote naming...\n\t\t~~~~~~~~~~~~~~~~~BEGIN~~~~~~~~~~~~~~~~~~~~~");
		if(!ProcessNamingInfo(sSrvName, SvcName,SvcInfo, IfNeedLocal,&IfNamingFromRemote,false))
		{
			LOG_DEBUG_P1(logger," |-=- ProcessNamingInfo Fail!(%s)",SvcInfo.c_str());
			THROW_C(CTuxException,LNS_OPERFAIL,"Local Name Service Operate Failed");
		}

		if(!this->ProcessConnect(SvcInfo))
		{
			LOG_DEBUG_P1(logger," |-=- ProcessConnect Fail!(%s)",SvcInfo.c_str());
			THROW_C(CTuxException,LNS_CONNECT_FAIL,"Local Name Service Connect Failed");
		}
		LOG_DEBUG(logger," |-=- \n\t\t~~~~~~~~~~~~~~~~~~ END ~~~~~~~~~~~~~~~~~~~~");

		if( this->m_bMTFlag )
		{
			LOG_DEBUG_P1(logger," |- [MULTICONTEXTS] tpsetctxt:%d",m_lTuxedoCtxt);
			if(tpsetctxt(this->m_lTuxedoCtxt, 0L)==-1)
				THROW_C_P2(CTuxException,tperrno,"[detail:[%d][%s]]",tperrno,tpstrerror(tperrno));
		}

		if(SvcName.length() == 0)
			THROW_C_P1(CTuxException,LNS_NO_DATA_FOUND,"[naming service no data found:[%s]]",(char *)sSrvName.c_str());

		iRet=tpcall((char *)SvcName.c_str(),(char *)DataIn.GetData(),0L,(char **)DataOut.GetBufRef(),&m_lReturnBufSize,Flag);
		LOG_DEBUG_P2(logger," |-=- tpcall Retry ADDR:%s SVC:%s ",SvcInfo.c_str(),SvcName.c_str());
		if (iRet==-1)
		{
			THROW_C_P2(CTuxException,tperrno,"[detail:[%d][%s]]",tperrno,tpstrerror(tperrno));
		}

	}

	if (IfNeedLocal)
		if(!ProcessLocalService(LLSVC_SAVE, DataIn, DataOut,sSrvName))
			LOG_DEBUG(logger," |- SAVE Local Service Data Fail!");

	{
		LOG_TRACE_P1(logger,"CFmlBuf!\n:%s",((CFmlBuf*)&DataOut)->ToString().c_str());
	}
	return iRet;
}

//按此格式调用服务，不提供本地缓存功能
//##ModelId=41A15D7E02A5
int CTuxCaller::AsyCall(const string &sSrvName, CBuffer& DataIn, long Flag, bool bIfUseNaming) throw(CTuxException)
{
	int iRet;
	bool IfNeedLocal=false;
	string SvcName;
	string SvcInfo;
	bool IfNamingFromRemote=false;

	LOG_DEBUG_P3(logger,"[AsyCall] LogicalName:%s InBufSize:%d Flag:%d",sSrvName.c_str(),DataIn.GetBufSize(),Flag);
	LOG_TRACE_P1(logger,"CFmlBuf!\n:%s",((CFmlBuf*)&DataIn)->ToString().c_str());

	if( bIfUseNaming )
	{
		if(!ProcessNamingInfo(sSrvName, SvcName, SvcInfo,IfNeedLocal,&IfNamingFromRemote,true))
			THROW_C(CTuxException,LNS_CONNECT_FAIL,"Local Name Service Connect Failed");

		LOG_DEBUG_P3(logger," |- Service:%s SRV_ADDR:%s NeedLocalService:%d",SvcName.c_str(),SvcInfo.c_str(),IfNeedLocal);

		if(!this->ProcessConnect(SvcInfo))
		{
			LOG_DEBUG_P1(logger," |- ProcessConnect Fail!(%s)",SvcInfo.c_str());
			THROW_C(CTuxException,LNS_CONNECT_FAIL,"Local Name Service Connect Failed");
		}
	}
	else
	{
		SvcName=sSrvName;
		LOG_DEBUG(logger," |- No Naming request!");
	}

	m_bLastLocalOperOK=false;
	m_bLastNeedLocal=false;

	if( this->m_bMTFlag )
	{
		LOG_DEBUG_P1(logger," |- [MULTICONTEXTS] tpsetctxt:%d",m_lTuxedoCtxt);
		if(tpsetctxt(this->m_lTuxedoCtxt, 0L)==-1)
			THROW_C_P2(CTuxException,tperrno,"[detail:[%d][%s]]",tperrno,tpstrerror(tperrno));
	}

	iRet=tpacall((char *)SvcName.c_str(),(char*)DataIn.GetData(),0L,Flag);
		LOG_DEBUG_P1(logger," |- tpacall AysCallid:%d",iRet);

	if (iRet==-1)
	{
		///如果是从远程服务器上取得的名字或者如果不是TPENOENT的错误,或者不使用Naming直接返回
		if(IfNamingFromRemote || tperrno!=TPENOENT || !bIfUseNaming)
			THROW_C_P2(CTuxException,tperrno,"[detail:[%d][%s]]",tperrno,tpstrerror(tperrno));

		///////////////////////////////////////////////////////////////////
		///开始再次从远程获取名字
		///////////////////////////////////////////////////////////////////
		LOG_DEBUG(logger," |-=- tpacall TPENOENT Try remote naming...\n\t\t~~~~~~~~~~~~~~~~~~BEGIN~~~~~~~~~~~~~~~~~~~~");
		if(!ProcessNamingInfo(sSrvName, SvcName, SvcInfo,IfNeedLocal,&IfNamingFromRemote,false))
		{
			LOG_DEBUG_P1(logger," |-=- ProcessNamingInfo Fail!(%s)",SvcInfo.c_str());
			THROW_C(CTuxException,LNS_CONNECT_FAIL,"Local Name Service Connect Failed");
		}

		if(!this->ProcessConnect(SvcInfo))
		{
			LOG_DEBUG_P1(logger," |-=- ProcessConnect Fail!(%s)",SvcInfo.c_str());
			THROW_C(CTuxException,LNS_CONNECT_FAIL,"Local Name Service Connect Failed");
		}
		LOG_DEBUG(logger,"\n\t\t~~~~~~~~~~~~~~~~~~~ END ~~~~~~~~~~~~~~~~~~~");

		if( this->m_bMTFlag )
		{
			LOG_DEBUG_P1(logger," |- [MULTICONTEXTS] tpsetctxt:%d",m_lTuxedoCtxt);
			if(tpsetctxt(this->m_lTuxedoCtxt, 0L)==-1)
				THROW_C_P2(CTuxException,tperrno,"[detail:[%d][%s]]",tperrno,tpstrerror(tperrno));
		}

		iRet=tpacall((char *)SvcName.c_str(),(char*)DataIn.GetData(),0L,Flag);
		LOG_DEBUG_P3(logger," |-=- tpacall (Retry) ADDR:%s SVC:%s Callid:%d",SvcInfo.c_str(),SvcName.c_str(),iRet);
		if (iRet==-1)
		{
			THROW_C_P2(CTuxException,tperrno,"[detail:[%d][%s]]",tperrno,tpstrerror(tperrno));
		}

	}/// End of if (Error Process)

	if(bIfUseNaming)
		m_AsyCallID=iRet;
	return iRet;
}

//##ModelId=41A15D7F0007
int CTuxCaller::AsyGetReply(CBuffer& DataOut, int asyid,long Flag) throw(CTuxException)
{
	int iRet;
	int theCallID;

	if(asyid!=-1)
		theCallID=asyid;
 	else if (m_AsyCallID!=-1)
		theCallID=m_AsyCallID;
	else
		THROW_C(CTuxException,TUX_INVALID_OPER,"Tuxedo Invalid Operate");

	LOG_DEBUG(logger,"[AsyGetReply]");

	if( this->m_bMTFlag )
	{
		LOG_DEBUG_P1(logger," |- [MULTICONTEXTS] tpsetctxt:%d",m_lTuxedoCtxt);
		if(tpsetctxt(this->m_lTuxedoCtxt, 0L)==-1)
			THROW_C_P2(CTuxException,tperrno,"[detail:[%d][%s]]",tperrno,tpstrerror(tperrno));
	}

	iRet=tpgetrply(&theCallID,(char**)DataOut.GetBufRef(),&m_lReturnBufSize,Flag);

	LOG_DEBUG_P3(logger," |- tpgetrply AsyCallID:%d lReturnBufSize:%d Flag:%d",theCallID,m_lReturnBufSize,Flag);

	if (iRet==-1)
	{
		THROW_C_P2(CTuxException,tperrno,"[detail:[%d][%s]]",tperrno,tpstrerror(tperrno));
	}

	LOG_TRACE_P1(logger,"CFmlBuf!\n:%s",((CFmlBuf*)&DataOut)->ToString().c_str());
	return iRet;
}

//##ModelId=41A599A40111
int CTuxCaller::AsyGetReply() throw(MWException)
{
	int iRet;
	if (m_AsyCallID==-1)
		THROW_C_P2(CTuxException,tperrno,"[detail:[%d][%s]]",tperrno,tpstrerror(tperrno));

	//如果上次调用本地服务操作成功，则直接返回，数据已经在缓冲区中
	if(m_bLastNeedLocal && m_bLastLocalOperOK)
	{
		m_bLastNeedLocal=false;
		m_bLastLocalOperOK=false;
		return 0;
	}

	LOG_DEBUG(logger,"[AsyGetReply]");
	if( this->m_bMTFlag )
	{

		LOG_DEBUG_P1(logger," |- [MULTICONTEXTS] tpsetctxt:%d",m_lTuxedoCtxt);
		if(tpsetctxt(this->m_lTuxedoCtxt, 0L)==-1)
			THROW_C_P2(CTuxException,tperrno,"[detail:[%d][%s]]",tperrno,tpstrerror(tperrno));
	}

	iRet=tpgetrply(&m_AsyCallID,(char**)m_OutBuf.GetBufRef(),&m_lReturnBufSize,0L);

	LOG_DEBUG_P3(logger," |- AsyCallID:%d lReturnBufSize:%d Flag:%d",m_AsyCallID,m_lReturnBufSize,0);

	if (iRet==-1)
	{
		THROW_C_P2(CTuxException,tperrno,"[detail:[%d][%s]]",tperrno,tpstrerror(tperrno));
	}

	if (m_bLastNeedLocal)
		if(!ProcessLocalService(LLSVC_SAVE, m_InBuf, m_OutBuf,m_LastLogicalName))
			LOG_DEBUG(logger," |- SAVE Local Service Data fail!");

	LOG_DEBUG_P1(logger,"CFmlBuf!\n:%s",m_OutBuf.ToString().c_str());

	return iRet;
}

//##ModelId=41A15D7F0170
int CTuxCaller::ConvsConn(const string &sSrvName, CBuffer* DataIn, long Flag) throw(CTuxException)
{
	int iRet;
	long lflag;

	bool IfNeedLocalSvc=false;
	string SvcName;
	string SvcInfo;
	bool IfNamingFromRemote=false;

	LOG_DEBUG_P3(logger,"[ConvsConn] Logical:%s Flag:%d",sSrvName.c_str(),m_AsyCallID,Flag);

	if(!ProcessNamingInfo(sSrvName, SvcName,SvcInfo, IfNeedLocalSvc,&IfNamingFromRemote,true))
	{
		LOG_DEBUG_P1(logger," |- Process Naming Info Fail! (%s)",sSrvName.c_str());
		THROW_C(CTuxException,LNS_CONNECT_FAIL,"Local Name Service Connect Failed");
	}
	LOG_DEBUG_P2(logger," |- Service:%s SRV_ADDR:%s",SvcName.c_str(),SvcInfo.c_str());

	if(!this->ProcessConnect(SvcInfo))
	{
		LOG_DEBUG_P1(logger," |- ProcessConnect Fail!(%s)",SvcInfo.c_str());
		THROW_C(CTuxException,LNS_CONNECT_FAIL,"Local Name Service Connect Failed");
	}


	if (Flag==-1)
		lflag=TPSENDONLY;
	else
		lflag=Flag|TPRECVONLY ;

	if( this->m_bMTFlag )
	{
		LOG_DEBUG_P1(logger," |- [MULTICONTEXTS] tpsetctxt:%d",m_lTuxedoCtxt);
		if(tpsetctxt(this->m_lTuxedoCtxt, 0L)==-1)
			THROW_C_P2(CTuxException,tperrno,"[detail:[%d][%s]]",tperrno,tpstrerror(tperrno));
	}

	if (DataIn == NULL)
		iRet=tpconnect((char *)SvcName.c_str(),(char *)NULL,0L,lflag);
	else
		iRet=tpconnect((char *)SvcName.c_str(),(char *)DataIn->GetData(),0L,lflag);

	LOG_DEBUG_P1(logger," |-- ConnectID:%d",iRet);

	if (iRet==-1)
	{
		///如果是从远程服务器上取得的名字或者如果不是TPENOENT的错误直接返回
		if(IfNamingFromRemote || tperrno!=TPENOENT)
			THROW_C_P2(CTuxException,tperrno,"[detail:[%d][%s]]",tperrno,tpstrerror(tperrno));


		///////////////////////////////////////////////////////////////////
		///开始再次从远程获取名字
		///////////////////////////////////////////////////////////////////
		LOG_DEBUG(logger," |-=- tpconnect TPENOENT Try remote naming...\n\t\t~~~~~~~~~~~~~~~~~~~~BEGIN~~~~~~~~~~~~~~~~~~~");
		if(!ProcessNamingInfo(sSrvName, SvcName,SvcInfo, IfNeedLocalSvc,&IfNamingFromRemote,false))
		{
			LOG_DEBUG_P1(logger," |-=- ProcessNamingInfo Fail!(%s)",SvcInfo.c_str());
			THROW_C(CTuxException,LNS_CONNECT_FAIL,"Local Name Service Connect Failed");
		}

		if(!this->ProcessConnect(SvcInfo))
		{
			LOG_DEBUG_P1(logger," |-=- ProcessConnect Fail!(%s)",SvcInfo.c_str());
			THROW_C(CTuxException,LNS_CONNECT_FAIL,"Local Name Service Connect Failed");
		}
		LOG_DEBUG(logger,"\n\t\t~~~~~~~~~~~~~~~~~~ END ~~~~~~~~~~~~~~~~~~~~");

		if( this->m_bMTFlag )
		{
			LOG_DEBUG_P1(logger," |- [MULTICONTEXTS] tpsetctxt:%d",m_lTuxedoCtxt);
			if(tpsetctxt(this->m_lTuxedoCtxt, 0L)==-1)
				THROW_C_P2(CTuxException,tperrno,"[detail:[%d][%s]]",tperrno,tpstrerror(tperrno));
		}

		if (DataIn == NULL)
			iRet=tpconnect((char *)SvcName.c_str(),(char *)NULL,0L,lflag);
		else
			iRet=tpconnect((char *)SvcName.c_str(),(char *)DataIn->GetData(),0L,lflag);

		LOG_DEBUG_P3(logger," |-=- tpconnect Retry ADDR:%s SVC:%s ID:%d",SvcInfo.c_str(),SvcName.c_str(),iRet);
		if (iRet==-1)
		{
			THROW_C_P2(CTuxException,tperrno,"[detail:[%d][%s]]",tperrno,tpstrerror(tperrno));
		}

	}

	m_ConnectID=iRet;

	return iRet;
}

//##ModelId=41A15D7F02E3
//int tpsend(int cd, char *data, long len, long flags, long *revent)
int CTuxCaller::ConvsSend(CBuffer& DataIn, int connetid,long *event , long Flag ) throw(CTuxException)
{
	int iRet;
	long lFlag,lEven=0L;
	int theConnectID;

	if (m_ConnectID==-1 &&  connetid==-1 )
		THROW_C(CTuxException,TUX_INVALID_OPER,"Tuxedo Invalid Operate");
	else
	{
		LOG_DEBUG(logger,"[ConvsSend]");
		LOG_TRACE_P1(logger,"CFmlBuf!\n%s",((CFmlBuf*)&DataIn)->ToString().c_str());
	}

	if (Flag==-1L)
		lFlag = TPRECVONLY;

	if(connetid!=-1)
		theConnectID=connetid;
	else
		theConnectID=m_ConnectID;

	if( this->m_bMTFlag )
	{
		LOG_DEBUG_P1(logger," |- [MULTICONTEXTS] tpsetctxt:%d",m_lTuxedoCtxt);
		if(tpsetctxt(this->m_lTuxedoCtxt, 0L)==-1)
			THROW_C_P2(CTuxException,tperrno,"[detail:[%d][%s]]",tperrno,tpstrerror(tperrno));
	}


	iRet=tpsend(theConnectID,(char *)DataIn.GetData(),0L,lFlag,&lEven);

	if (event!=NULL)
		*event=	lEven;

	LOG_DEBUG_P3(logger," |- tpsend ConnectID:%d event:%d Flag:%d",theConnectID,lEven,Flag);

	if (iRet==-1)
		THROW_C_P2(CTuxException,tperrno,"[detail:[%d][%s]]",tperrno,tpstrerror(tperrno));


	return iRet;
}

//##ModelId=41A15D8000E5
int CTuxCaller::ConvsRecv(CBuffer& DataOut, int connetid,long *event, long Flag) throw(CTuxException)
{
	int iRet;
	long lFlag,lEven=0L,lbufsize;
	int theConnectID;

	if (Flag==-1L)
		lFlag = TPNOCHANGE;

	LOG_DEBUG(logger,"[ConvsRecv]");

	if (m_ConnectID==-1 &&  connetid==-1 )
		THROW_C(CTuxException,TUX_INVALID_OPER,"Tuxedo Invalid Operate");

	if(connetid!=-1)
		theConnectID=connetid;
	else
		theConnectID=m_ConnectID;

	if( this->m_bMTFlag )
	{
		LOG_DEBUG_P1(logger," |- [MULTICONTEXTS] tpsetctxt:%d",m_lTuxedoCtxt);
		if(tpsetctxt(this->m_lTuxedoCtxt, 0L)==-1)
			THROW_C_P2(CTuxException,tperrno,"[detail:[%d][%s]]",tperrno,tpstrerror(tperrno));
	}


	iRet=tprecv(theConnectID,(char **)DataOut.GetBufRef(),&lbufsize,lFlag,&lEven);

	LOG_DEBUG_P3(logger," |- ConnectID:%d event:%d Flag:%d",theConnectID,lEven,Flag);

	if (iRet != -1 )
		THROW_C_P2(CTuxException,tperrno,"[detail:[%d][%s]]",tperrno,tpstrerror(tperrno));

	if (event==NULL)
		{
			switch (tperrno)
			{
				case	TPEEVENT:
					switch ((int)(lEven))
					{
						case	TPEV_SENDONLY:	/* Normal event get the contral of connection.*/
							{
								LOG_TRACE_P1(logger,"CFmlBuf!\n%s",((CFmlBuf*)&DataOut)->ToString().c_str());
							}
							return 0;
						case	TPEV_DISCONIMM: /* disorderly disconnect */
							{
								LOG_TRACE_P1(logger,"CFmlBuf!\n%s",((CFmlBuf*)&DataOut)->ToString().c_str());
							}
							return 1;
						case	TPEV_SVCSUCC:
							{
								LOG_TRACE_P1(logger,"CFmlBuf!\n%s",((CFmlBuf*)&DataOut)->ToString().c_str());
							}
							return 2;
							//成功结束
					}
				default :	LOG_DEBUG(logger," |- UnWanted Event!");
							THROW_C_P2(CTuxException,tperrno,"[detail:[%d][%s]]",tperrno,tpstrerror(tperrno));
			}
		}
	else
		*event = lEven;

	return 0;
}

//##ModelId=41A15D8002EE
int CTuxCaller::SetMsgHandler(void(_LKENTER *disp)(char *, long, long)) throw(CTuxException)
{
	if ( tpsetunsol ( disp) == TPUNSOLERR )
		THROW_C_P2(CTuxException,tperrno,"[detail:[%d][%s]]",tperrno,tpstrerror(tperrno));
	return 0;
}

//##ModelId=41A15D8003B6
int CTuxCaller::ChkMsg(void) throw(CTuxException)
{
	if ( tpchkunsol() == -1 )
		THROW_C_P2(CTuxException,tperrno,"[detail:[%d][%s]]",tperrno,tpstrerror(tperrno));
	return 0;
}

//##ModelId=41A15D810079
//Return value is handld of subscribtion.
long CTuxCaller::Subscribe(char *eventexpr, char *filter, void *ctl, long Flag) throw(CTuxException)
{
	//	long tpsubscribe(char *eventexpr, char *filter, TPEVCTL *ctl, long flags)
	long lHandle;
	if (m_bMTFlag==true && m_lTuxedoCtxt==TPINVALIDCONTEXT )
		THROW_C(CTuxException,TUX_INVALID_OPER,"Tuxedo Invalid Operate");

	if ( (lHandle=tpsubscribe(eventexpr,filter,(tpevctl_t *)ctl,Flag)) == -1 )
		THROW_C_P2(CTuxException,tperrno,"[detail:[%d][%s]]",tperrno,tpstrerror(tperrno));
	return lHandle;
}

//##ModelId=41A15D81029F
int CTuxCaller::Unsubscribe(long handle, long Flag) throw(CTuxException)
{
	if (m_bMTFlag==true && m_lTuxedoCtxt==TPINVALIDCONTEXT )
		THROW_C(CTuxException,TUX_INVALID_OPER,"Tuxedo Invalid Operate");

//	int tpunsubscribe(long subscription, long flags)
	if ( tpunsubscribe(handle,Flag) == -1 )
		THROW_C_P2(CTuxException,tperrno,"[detail:[%d][%s]]",tperrno,tpstrerror(tperrno));
	return 0;
}

//##ModelId=41A15D82000C
//int tppost(char *eventname, char *data, long len, long flags)
int CTuxCaller::Post(char *eventname, CBuffer& Data, long flags) throw(CTuxException)
{
	if (m_bMTFlag==true && m_lTuxedoCtxt==TPINVALIDCONTEXT )
		THROW_C(CTuxException,TUX_INVALID_OPER,"Tuxedo Invalid Operate");

	if ( tppost(eventname,(char *)Data.GetData(),0L,flags) == -1 )
		THROW_C_P2(CTuxException,tperrno,"[detail:[%d][%s]]",tperrno,tpstrerror(tperrno));
	return 0;
}

//##ModelId=41A15D82020B
int CTuxCaller::Broadcast(char *lmid, char *usrname, char *cltname, CBuffer& Data, long flags) throw(CTuxException)
{
	if (m_bMTFlag==true && m_lTuxedoCtxt==TPINVALIDCONTEXT )
		THROW_C(CTuxException,TUX_INVALID_OPER,"Tuxedo Invalid Operate");

	if (tpbroadcast(lmid,usrname,cltname,(char *)Data.GetData(),0L,flags)== -1 )
		THROW_C_P2(CTuxException,tperrno,"[detail:[%d][%s]]",tperrno,tpstrerror(tperrno));
	return 0;

}

//##ModelId=41A15D830144
//Routine for sending notification by client identifier.
int CTuxCaller::Notify(void *clientid, CBuffer& Data, long flags) throw(CTuxException)
{
	//int tpnotify(CLIENTID *clientid, char *data, long len, long flags)
	if (m_bMTFlag==true && m_lTuxedoCtxt==TPINVALIDCONTEXT )
		THROW_C(CTuxException,TUX_INVALID_OPER,"Tuxedo Invalid Operate");

	if ( tpnotify((clientid_t *)clientid,(char *)Data.GetData(),0L,flags)==-1)
		THROW_C_P2(CTuxException,tperrno,"[detail:[%d][%s]]",tperrno,tpstrerror(tperrno));
	return 0;
}

//##ModelId=41A15D83032E
//int tpbegin(unsigned long timeout, long flags)
//Routine for beginning a transaction.
int CTuxCaller::TransBegin(unsigned long timeout, long flags) throw(CTuxException)
{
	if (m_bMTFlag==true && m_lTuxedoCtxt==TPINVALIDCONTEXT )
		THROW_C(CTuxException,TUX_INVALID_OPER,"Tuxedo Invalid Operate");

	//Already in transtration , return directly.
	if ( IsInTrans() == true)
		return 0;

	if( this->m_bMTFlag )
	{
		LOG_DEBUG_P1(logger,"[MULTICONTEXTS] tpsetctxt:%d",m_lTuxedoCtxt);
		if(tpsetctxt(this->m_lTuxedoCtxt, 0L)==-1)
			THROW_C_P2(CTuxException,tperrno,"[detail:[%d][%s]]",tperrno,tpstrerror(tperrno));
	}


	//Star a transtation.
	if( tpbegin(timeout, 0L) ==-1 )
		THROW_C_P2(CTuxException,tperrno,"[detail:[%d][%s]]",tperrno,tpstrerror(tperrno));

	LOG_DEBUG_P1(logger,"[TransBegin] TimeOut:%d",timeout);

	return 0;
}

//##ModelId=41A15D8400FF
//int tpsuspend(TPTRANID *tranid, long flags)
//Suspends a global transaction.
int CTuxCaller::TransSuspend() throw(CTuxException)
{
	if (m_bMTFlag==true && m_lTuxedoCtxt==TPINVALIDCONTEXT )
		THROW_C(CTuxException,TUX_INVALID_OPER,"Tuxedo Invalid Operate");

	LOG_DEBUG(logger,"[TransSuspend]");

	if( this->m_bMTFlag )
	{
		LOG_DEBUG_P1(logger,"[MULTICONTEXTS] tpsetctxt:%d",m_lTuxedoCtxt);
		if(tpsetctxt(this->m_lTuxedoCtxt, 0L)==-1)
			THROW_C_P2(CTuxException,tperrno,"[detail:[%d][%s]]",tperrno,tpstrerror(tperrno));
	}


	if( tpsuspend(&m_tranID, 0L) == -1)
		THROW_C_P2(CTuxException,tperrno,"[detail:[%d][%s]]",tperrno,tpstrerror(tperrno));

	return 0;
}

//##ModelId=41A15D840312
//int tpresume(TPTRANID *tranid, long flags)
//Resumes a global transaction.
int CTuxCaller::TransResume() throw(CTuxException)
{
	if (m_bMTFlag==true && m_lTuxedoCtxt==TPINVALIDCONTEXT )
		THROW_C(CTuxException,TUX_INVALID_OPER,"Tuxedo Invalid Operate");

	LOG_DEBUG(logger,"[TransResume]");

	if( this->m_bMTFlag )
	{
		LOG_DEBUG_P1(logger,"[MULTICONTEXTS] tpsetctxt:%d",m_lTuxedoCtxt);
		if(tpsetctxt(this->m_lTuxedoCtxt, 0L)==-1)
			THROW_C_P2(CTuxException,tperrno,"[detail:[%d][%s]]",tperrno,tpstrerror(tperrno));
	}


	if ( tpresume(&m_tranID, 0L) == -1)
		THROW_C_P2(CTuxException,tperrno,"[detail:[%d][%s]]",tperrno,tpstrerror(tperrno));

	return 0;

}

//##ModelId=41A15D8500EC
//tpcommit()—Routine for committing current transaction.
int CTuxCaller::TransCommit(long flags) throw(CTuxException)
{
	if (m_bMTFlag==true && m_lTuxedoCtxt==TPINVALIDCONTEXT )
		THROW_C(CTuxException,TUX_INVALID_OPER,"Tuxedo Invalid Operate");

	LOG_DEBUG(logger,"[TransCommit]");

	if( this->m_bMTFlag )
	{
		LOG_DEBUG_P1(logger,"[MULTICONTEXTS] tpsetctxt:%d",m_lTuxedoCtxt);
		if(tpsetctxt(this->m_lTuxedoCtxt, 0L)==-1)
			THROW_C_P2(CTuxException,tperrno,"[detail:[%d][%s]]",tperrno,tpstrerror(tperrno));
	}


	if (tpcommit(0L)==-1)
		THROW_C_P2(CTuxException,tperrno,"[detail:[%d][%s]]",tperrno,tpstrerror(tperrno));

	return 0;
}

//[flags] is reserved for future use in Tuxedo System.
//Return value is not used in Tuxedo System.
//##ModelId=41A15D850205
int CTuxCaller::TransAbort(long flags) throw(CTuxException)
{
	if (m_bMTFlag==true && m_lTuxedoCtxt==TPINVALIDCONTEXT )
		THROW_C(CTuxException,TUX_INVALID_OPER,"Tuxedo Invalid Operate");

	LOG_DEBUG(logger,"[TransAbort]");

	if( this->m_bMTFlag )
	{
		LOG_DEBUG_P1(logger,"[MULTICONTEXTS] tpsetctxt:%d",m_lTuxedoCtxt);
		if(tpsetctxt(this->m_lTuxedoCtxt, 0L)==-1)
			THROW_C_P2(CTuxException,tperrno,"[detail:[%d][%s]]",tperrno,tpstrerror(tperrno));
	}


	if (tpabort(0L) == -1)
		THROW_C_P2(CTuxException,tperrno,"[detail:[%d][%s]]",tperrno,tpstrerror(tperrno));

	return 0;
}

//##ModelId=41A15D850331
int CTuxCaller::Enqueue(char *qspace, char *qname, void *ctl, CBuffer& Data, long flags) throw(CTuxException)
{
	int iRet;

	if( this->m_bMTFlag )
	{
		LOG_DEBUG_P1(logger,"[MULTICONTEXTS] tpsetctxt:%d",m_lTuxedoCtxt);
		if(tpsetctxt(this->m_lTuxedoCtxt, 0L)==-1)
			THROW_C_P2(CTuxException,tperrno,"[detail:[%d][%s]]",tperrno,tpstrerror(tperrno));
	}

	iRet=tpenqueue(qspace, qname,(tpqctl_t *) ctl, (char *)Data.GetData(), 0L, flags);
	if (iRet == -1 )
		THROW_C_P2(CTuxException,tperrno,"[detail:[%d][%s]]",tperrno,tpstrerror(tperrno));
	return 0;
}

//##ModelId=41A15D860347
int CTuxCaller::Dequeue(char *qspace, char *qname, void *ctl, CBuffer& Data, long flags) throw(CTuxException)
{
	if( this->m_bMTFlag )
	{
		LOG_DEBUG_P1(logger,"[MULTICONTEXTS] tpsetctxt:%d",m_lTuxedoCtxt);
		if(tpsetctxt(this->m_lTuxedoCtxt, 0L)==-1)
			THROW_C_P2(CTuxException,tperrno,"[detail:[%d][%s]]",tperrno,tpstrerror(tperrno));
	}

	int iRet=tpdequeue(qspace, qname, (tpqctl_t *)ctl, (char **)Data.GetBufRef(), 0L, flags);
	if (iRet == -1 )
		THROW_C_P2(CTuxException,tperrno,"[detail:[%d][%s]]",tperrno,tpstrerror(tperrno));

	return 0;
}

//##ModelId=41A15D8801A5
int CTuxCaller::Return(int rval, long rcode, CBuffer& Data, long flags)
{
	LOG_DEBUG_P3(logger,"[Return] rval:%d,rcode:%d,flags:%d",rval,rcode,flags);
	FBFR *pFmlBuf=(FBFR *)Data.GetData();
	Data.SetData(NULL);
	delete &Data;
	tpreturn(rval, rcode, ( char *)pFmlBuf, 0L, flags);
	return 0;
}

//##ModelId=41A15D8903C3
int CTuxCaller::Forward(char *svc, CBuffer& Data, long flags) throw(CTuxException)
{
	LOG_DEBUG_P2(logger,"[Forward] service:%s,flags:%d",svc,flags);
	FBFR *pFmlBuf=(FBFR *)Data.GetData();
	Data.SetData(NULL);
	delete &Data;
	tpforward( svc, ( char *)pFmlBuf, 0L, flags);
	return 0;
}

//##ModelId=41A15D8B011D
//reverse for futuer use
int CTuxCaller::Set(int iValId, void *data) throw(CTuxException)
{
	return 0;
}

//##ModelId=41A15D8C00CE
CTuxCaller& CTuxCaller::operator=(const CTuxCaller& right)
{
//	if( this != &right )
//	{
//		m_lTuxedoCtxt = right.m_lTuxedoCtxt;
//		m_bIsConn=false;
//		m_bMTFlag=right.m_bMTFlag;
//		m_LocalService=right.m_LocalService;
//		m_nameService=right.m_nameService;
//		m_sCltname=right.m_sCltname;
//		m_sGrpname=right.m_sCltname;
//		m_sPasswd=right.m_sCltname;
//		m_sSrvIP=right.m_sCltname;
//		m_sSrvPort=right.m_sCltname;
//		m_sUsrname=right.m_sCltname;
//		m_ConnectID=-1;
//		m_AsyCallID=-1;
//		m_bDebugTag=right.m_bDebugTag;
//		m_lReturnBufSize=right.m_lReturnBufSize;
//		if (m_bDebugTag)
//			LOG_DEBUG(logger,"[operator=] ");
//	}
	return *this;
}

/*
//##ModelId=41A15D8C0327

int CTuxCaller::Connect(const string &sIP, const string &sPort) throw(CTuxException)
{

	this->m_sSrvIP	= sIP;
	this->m_sSrvPort	= sPort;
	this->Connect();

	return 0;
}
*/

//##ModelId=41A15D8D02F7
bool CTuxCaller::IsConnect()
{
	return m_bIsConn;
}

//##ModelId=41A15D8D03C9
bool CTuxCaller::DisConnect(void) throw(CTuxException)
{
	if ( m_bIsConn == false )
		return 0;

	LOG_DEBUG(logger," |- DisConnect");

	if( this->m_bMTFlag )
	{
		LOG_DEBUG_P1(logger," |- [MULTICONTEXTS] tpsetctxt:%d",m_lTuxedoCtxt);
		if(tpsetctxt(this->m_lTuxedoCtxt, 0L)==-1)
			THROW_C_P2(CTuxException,tperrno,"[detail:[%d][%s]]",tperrno,tpstrerror(tperrno));
	}

	if (tpterm() == -1)
		THROW_C_P2(CTuxException,tperrno,"[detail:[%d][%s]]",tperrno,tpstrerror(tperrno));
	m_bIsConn=false;

	return 0;
}

//##ModelId=41A15D8E01CC
bool CTuxCaller::ConvsDisConn(int cd) throw(CTuxException)
{
	if (m_ConnectID==-1 && cd== -1)
		THROW_C(CTuxException,TUX_INVALID_OPER,"Tuxedo Invalid Operate");

	LOG_DEBUG_P1(logger,"[ConvsDisConn] ConnectID:%d",m_ConnectID);

	if( this->m_bMTFlag )
	{
		LOG_DEBUG_P1(logger," |- [MULTICONTEXTS] tpsetctxt:%d",m_lTuxedoCtxt);
		if(tpsetctxt(this->m_lTuxedoCtxt, 0L)==-1)
			THROW_C_P2(CTuxException,tperrno,"[detail:[%d][%s]]",tperrno,tpstrerror(tperrno));
	}

	if(tpdiscon(cd==-1?m_ConnectID:cd)== -1 )
		THROW_C_P2(CTuxException,tperrno,"[detail:[%d][%s]]",tperrno,tpstrerror(tperrno));

	return 0;
}

//##ModelId=41A15D8F0000
int CTuxCaller::SetData(const string &sSrvName, CBuffer& Data) throw(CTuxException)
{
	CFmlBuf *pBuf;
	CBuffer *pBuffer;

	LOG_DEBUG_P1(logger,"[SetData] service:%s",sSrvName.c_str());

	if (Data.GetBufType() != TUX_BUF_TYPE_FML)
		THROW_C(CTuxException,TUX_UNSUPPORT,"Tuxedo Unsupport");

	pBuffer=&Data;

	pBuf=(CFmlBuf *)pBuffer;

	m_sSvcName=sSrvName;
	m_InBuf.ClearFmlValue();
	m_InBuf=*pBuf;

	return 0;
}

//##ModelId=41A15D8F0368
int CTuxCaller::GetData(CBuffer& Data) throw(MWException)
{
	CFmlBuf *pBuf;

	if (Data.GetBufType() != TUX_BUF_TYPE_FML)
		THROW_C(CTuxException,TUX_UNSUPPORT,"Tuxedo Unsupport");

	LOG_DEBUG(logger,"[GetData]");

	pBuf=(CFmlBuf *)&Data;

	try
	{
		pBuf->ClearFmlValue();
		(*pBuf)=m_OutBuf;
		return 0;
	}catch(CFmlException e)
	{
		throw;
	}

	return 0;
}

//##ModelId=41A15D9001BB
//Routine for checking if a transaction is in progress.
bool CTuxCaller::IsInTrans()
{
	if (m_bMTFlag==true && m_lTuxedoCtxt==TPINVALIDCONTEXT )
		THROW_C(CTuxException,TUX_INVALID_OPER,"Tuxedo Invalid Operate");

	if( this->m_bMTFlag )
	{
		LOG_DEBUG_P1(logger,"[MULTICONTEXTS] tpsetctxt:%d",m_lTuxedoCtxt);
		if(tpsetctxt(this->m_lTuxedoCtxt, 0L)==-1)
			THROW_C_P2(CTuxException,tperrno,"[detail:[%d][%s]]",tperrno,tpstrerror(tperrno));
	}

	switch ( tpgetlev())
	{
		case 0 : return false;
		case 1 : return true;
	}

	THROW_C_P2(CTuxException,tperrno,"[detail:[%d][%s]]",tperrno,tpstrerror(tperrno));
}

//##ModelId=41A15D9002A1
//Routine for canceling a call descriptor for outstanding reply.
int CTuxCaller::CancelAsyCall(int asyid) throw(CTuxException)
{
	if ((asyid==-1 && m_AsyCallID==-1) || (m_bMTFlag==true && m_lTuxedoCtxt==TPINVALIDCONTEXT ))
		THROW_C(CTuxException,TUX_INVALID_OPER,"Tuxedo Invalid Operate");

	LOG_DEBUG_P1(logger,"[CancelAsyCall] AsyCallID:%d",m_AsyCallID);

	if( this->m_bMTFlag )
	{
		LOG_DEBUG_P1(logger," |- [MULTICONTEXTS] tpsetctxt:%d",m_lTuxedoCtxt);
		if(tpsetctxt(this->m_lTuxedoCtxt, 0L)==-1)
			THROW_C_P2(CTuxException,tperrno,"[detail:[%d][%s]]",tperrno,tpstrerror(tperrno));
	}

	if (asyid!=-1)
		if (tpcancel(asyid) == -1 )
			THROW_C(CTuxException,TUX_INVALID_OPER,"Tuxedo Invalid Operate");
	else
		if (tpcancel(m_AsyCallID) == -1 )
			THROW_C(CTuxException,TUX_INVALID_OPER,"Tuxedo Invalid Operate");

	return 0;
}


//##ModelId=41A15D92024A
int CTuxCaller::SynCall()
{
	int iRet=0;
	string SvcName,SvcInfo;
	bool IfNeedLocal;
	bool IfNamingFromRemote=false;
	{
		LOG_DEBUG_P3(logger,"[SynCall(void)] LogicalName:%s InBufSize:%d Out_Buf_Size:%d",m_sSvcName.c_str(),m_InBuf.GetBufSize(),m_OutBuf.GetBufSize());
		LOG_DEBUG_P1(logger,"CFmlBuf!\n%s",m_InBuf.ToString().c_str());
	}

	if(!ProcessNamingInfo(m_sSvcName, SvcName,SvcInfo, IfNeedLocal,&IfNamingFromRemote,true))
		THROW_C(CTuxException,LNS_CONNECT_FAIL,"Local Name Service Connect Failed");

	LOG_DEBUG_P3(logger," |- Service:%s SRV_ADDR:%s NeedLocalService:%d",SvcName.c_str(),SvcInfo.c_str(),IfNeedLocal);

	if (IfNeedLocal)
		if(ProcessLocalService(LLSVC_LOOKUP, m_InBuf, m_OutBuf,m_sSvcName))
			return 0;

	LOG_DEBUG(logger," |- Local Service Lookup Fail!(Connecting Remote...)");

	if(!this->ProcessConnect(SvcInfo))
	{
		LOG_DEBUG_P1(logger," |- ProcessConnect Fail!(%s)",SvcInfo.c_str());

		THROW_C(CTuxException,LNS_CONNECT_FAIL,"Local Name Service Connect Failed");
	}

	if( this->m_bMTFlag )
	{
		LOG_DEBUG_P1(logger," |- [MULTICONTEXTS] tpsetctxt:%d",m_lTuxedoCtxt);
		if(tpsetctxt(this->m_lTuxedoCtxt, 0L)==-1)
			THROW_C_P2(CTuxException,tperrno,"[detail:[%d][%s]]",tperrno,tpstrerror(tperrno));
	}

	iRet=tpcall((char *)SvcName.c_str(),(char *)m_InBuf.GetData(),0L,(char **)m_OutBuf.GetBufRef(),&m_lReturnBufSize,0L);


	LOG_DEBUG_P2(logger," |--SynCall(void) service:%s lReturnBufSize:%d",m_sSvcName.c_str(),m_lReturnBufSize);

	if (iRet==-1)
	{
		///如果是从远程服务器上取得的名字或者如果不是TPENOENT的错误直接返回
		if(IfNamingFromRemote || tperrno!=TPENOENT)
			THROW_C_P2(CTuxException,tperrno,"[detail:[%d][%s]]",tperrno,tpstrerror(tperrno));

		///////////////////////////////////////////////////////////////////
		///开始再次从远程获取名字
		///////////////////////////////////////////////////////////////////
		LOG_DEBUG(logger," |-=- tpcall TPENOENT Try remote namintrerror(tperrno));

		///////////////////////////////////////////////////////////////////
		///寮
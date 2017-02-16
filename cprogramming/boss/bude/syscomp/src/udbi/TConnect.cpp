/** Copyright (c) 2004, 联创科技
 *  All rights reserved.
 * 
 *  文件名称：TConnect.h
 *  文件标识：
 *  摘    要：
 * 
 *  当前版本：1.0
 *  作    者：WangFJ
 *  完成日期：2004年12月06日
 *
 *  取代版本： 
 *  原作者  ：
 *  完成日期：
 */        

#include "TTable.h"
#include "TConnect.h"
                  
//! 2005.11.21 WangFJ.                  
#include "TCursor.h"
                  
/** \class UDBI::TConnect  TConnect.h
 *  \brief 数据库连接接口实现类
 *
 *  TConnect 封装了数据库连接、事务控制功能，可以支持XA方式和非XA方式的数据库连接。
 *  所有的SQL语句执行都是在一个数据库连接下进行的，数据库访问程序的开发步骤为：
 *  \arg 初始化OCI编程环境（jdb 静态类）
 *  \arg 建立数据库连接（TConnect 连接类)
 *  \arg SQL语句执行（JTable SQL语句执行接口类）
 * 
 *  在非XA方式下建立数据库连接需要提供用户名、口令、数据库连接别名；
 *  在XA方式下建立连接是通过调用xa函数实现的，在一个进程空间里，如果之前已经调用xa
 *  函数创建了数据库连接，则本调用将直接获取并返回那个连接；
 */

/**
 *  构造函数，使用参数\a vpEnv 初始化OCIEnv，并如果\a iOption 参数不等于\c CONNECTED_XA 值，则只做连接初始化，
 *  如果\a iOption 参数等于\c CONNECTED_XA ,则在初始化后，调用xa函数建立XA数据库连接。
 *  BOSS1.5研发项目中，由于需要支持分布式事务，所以将使用这个构造函数建立数据库连接
 *  缺省构造函数和XA连接构造函数共用此构造
 *  @param OCIEnv *vpEnv  OCI编程环境句柄，通常在静态类jdb中初始化
 *  @param int iOption  可选值 CONNECTED_NOT, CONNECTED_XA
 */
UDBI::TConnect::TConnect(OCIEnv* vpEnv, const int iOption):
  m_envhp(vpEnv),m_srvhp(0),m_svchp(0),m_errhp(0),m_usrhp(0),m_transhp(0),m_connected(CONNECTED_NOT),m_trans(false), jtable_(0)
{
	UDBI_TRACE("TConnect::TConnect(OCIEnv* vpEnv, const int iOption)");
	
	(void)OCIHandleAlloc((dvoid *)m_envhp, (dvoid **)&m_errhp, (ub4)OCI_HTYPE_ERROR, (size_t)0, (dvoid **)0);
    
    if (iOption==CONNECTED_XA) {
    	ConnectXA();
    }
}

/**
 *  构造函数，使用参数 \a xpEnv 初始化OCIEnv，并使用\a cTns, \a cUser, \a cPassword 建立本地数据库连接
 */
UDBI::TConnect::TConnect(OCIEnv* vpEnv, const string& cTns, const string& cUser, const string& cPassword):
	m_envhp(vpEnv),m_srvhp(0),m_svchp(0),m_errhp(0),m_usrhp(0),m_transhp(0),m_connected(CONNECTED_NOT),m_trans(false), jtable_(0)
{
	UDBI_TRACE_P2("TConnect::TConnect(cTns='%s', cUser='%s', cPassword='#')", cTns.c_str(), cUser.c_str());
	
	(void)OCIHandleAlloc((dvoid *)m_envhp, (dvoid **)&m_errhp, (ub4)OCI_HTYPE_ERROR, (size_t)0, (dvoid **)0);

    Connect(cTns,cUser,cPassword);
}

/**
 *  构造函数(for OCCI)，使用参数 \a xpEnv 初始化OCIEnv
 */
UDBI::TConnect::TConnect(OCIEnv* vpEnv, OCISvcCtx* vpSvc):
	m_envhp(vpEnv), m_srvhp(0), m_svchp(vpSvc), m_errhp(0), m_usrhp(0), m_transhp(0), m_connected(CONNECTED_OCCI),m_trans(false), jtable_(0)
{
	UDBI_TRACE("TConnect(OCIEnv *vpEnv, OCISvcCtx *vpSvc)");	
	
	(void)OCIHandleAlloc((dvoid *)m_envhp, (dvoid **)&m_errhp, (ub4)OCI_HTYPE_ERROR, (size_t)0, (dvoid **)0);
	(void)OCIHandleAlloc((dvoid *)m_envhp, (dvoid **)&m_transhp, (ub4)OCI_HTYPE_TRANS, (size_t)0, (dvoid **)0 );
	OCIAttrSet((dvoid *)m_svchp, (ub4)OCI_HTYPE_SVCCTX, (dvoid *)m_transhp, 
               (ub4)0, (ub4)OCI_ATTR_TRANS, (OCIError *)m_errhp );	
               
    m_connected = CONNECTED_OCCI;
    
    //! 创建一个单件JTable*
    if ( !jtable_ ) {
		jtable_ = new TTable(m_envhp, m_svchp);    
	}    
}

/**
 *  \fn UDBI::TConnect::~TConnect() 
 *  析构函数，断开数据库连接，释放资源
 */
UDBI::TConnect::~TConnect()
{
	UDBI_TRACE("TConnect::~TConnect()");
	
    DisConnect();
}

/**
 *  \fn void UDBI::TConnect::Connect(const string& cTns, const string& cUser, const string& cPassword)
 *  数据库连接函数，在初始化的OCIEnv *下，使用\a cTns, \a cUser, \a cPassword 参数建立本地数据库连接
 */
void UDBI::TConnect::Connect(const string& cTns, const string& cUser, const string& cPassword)
{
	UDBI_TRACE_P2("TConnect::Connect(cTns='%s', cUser='%s', cPassword='#')", cTns.c_str(), cUser.c_str());
	
	//! 支持重连接
	if (m_connected != CONNECTED_NOT) {
		DisConnect();
	}
	
	(void)OCIHandleAlloc((dvoid *)m_envhp, (dvoid **)&m_svchp, (ub4)OCI_HTYPE_SVCCTX, (size_t)0, (dvoid **)0 );
	(void)OCIHandleAlloc((dvoid *)m_envhp, (dvoid **)&m_srvhp, (ub4)OCI_HTYPE_SERVER, (size_t)0, (dvoid **)0 );

	//创建数据库连接：输入tnsname
	CHECKERR(m_errhp, OCIServerAttach((OCIServer *)m_srvhp, m_errhp, 
                      (text *)cTns.c_str(), (sb4)strlen(cTns.c_str()), (ub4)OCI_DEFAULT) );
	OCIAttrSet((dvoid *)m_svchp, (ub4)OCI_HTYPE_SVCCTX, (dvoid *)m_srvhp,
                      (ub4)0, (ub4)OCI_ATTR_SERVER, (OCIError *)m_errhp );

	//创建数据库会话：输入username/password
	(void)OCIHandleAlloc((dvoid *)m_envhp, (dvoid **)&m_usrhp, (ub4)OCI_HTYPE_SESSION, (size_t)0, (dvoid **)0 );
	OCIAttrSet((dvoid *)m_usrhp, (ub4)OCI_HTYPE_SESSION, (dvoid *)cUser.c_str(), 
               (sb4)strlen(cUser.c_str()), (ub4)OCI_ATTR_USERNAME, (OCIError *)m_errhp );
	OCIAttrSet((dvoid *)m_usrhp, (ub4)OCI_HTYPE_SESSION, (dvoid *)cPassword.c_str(), 
               (sb4)strlen(cPassword.c_str()), (ub4)OCI_ATTR_PASSWORD, (OCIError *)m_errhp );

	CHECKERR(m_errhp, OCISessionBegin(m_svchp, m_errhp, m_usrhp, (ub4)OCI_CRED_RDBMS, (ub4)OCI_DEFAULT));
	OCIAttrSet((dvoid *)m_svchp, (ub4)OCI_HTYPE_SVCCTX, (dvoid *)m_usrhp, 
               (ub4)0, (ub4)OCI_ATTR_SESSION, (OCIError *)m_errhp );

	//创建事务句柄并绑定到服务上下文：
	(void)OCIHandleAlloc((dvoid *)m_envhp, (dvoid **)&m_transhp, (ub4)OCI_HTYPE_TRANS, (size_t)0, (dvoid **)0 );
	OCIAttrSet((dvoid *)m_svchp, (ub4)OCI_HTYPE_SVCCTX, (dvoid *)m_transhp, 
               (ub4)0, (ub4)OCI_ATTR_TRANS, (OCIError *)m_errhp );

    m_connected = CONNECTED_LOCAL;
    
    //! 创建一个单件JTable*
    if ( !jtable_ ) {
		jtable_ = new TTable(m_envhp, m_svchp);    
	}
    
}

/**
 *  \fn void UDBI::TConnect::ConnectXA()
 *  数据库连接函数，在初始化的OCIEnv *下，调用xa函数建立XA数据库连接
 */
void UDBI::TConnect::ConnectXA()
{
	UDBI_TRACE("TConnect::ConnectXA()");
	
	//! 支持重连接
	if (m_connected != CONNECTED_NOT) {
		DisConnect();
	}
	
    m_svchp = xaoSvcCtx(NULL);
    
    // xa 连接下支持UDBI事务控制
    (void)OCIHandleAlloc((dvoid *)m_envhp, (dvoid **)&m_transhp, (ub4)OCI_HTYPE_TRANS, (size_t)0, (dvoid **)0 );
	OCIAttrSet((dvoid *)m_svchp, (ub4)OCI_HTYPE_SVCCTX, (dvoid *)m_transhp, 
               (ub4)0, (ub4)OCI_ATTR_TRANS, (OCIError *)m_errhp );
               
    m_connected = CONNECTED_XA;
    
    //! 创建一个单件JTable*
    if ( !jtable_ ) {
		jtable_ = new TTable(m_envhp, m_svchp);    
	}
}

/**
 *  \fn void UDBI::TConnect::DisConnect()
 *  断开本地数据库连接，释放资源（包括会话、事务、服务连接等）
 */
void UDBI::TConnect::DisConnect()
{
	UDBI_TRACE("TConnect::DisConnect()");
	
    //! 销毁单件JTable*
    delete jtable_;   jtable_ = 0;
    
    //! 释放资源	
	if ((m_connected == CONNECTED_XA)&&(m_errhp)) {
    	 OCIHandleFree((dvoid *)m_transhp, OCI_HTYPE_TRANS);
         OCIHandleFree((dvoid *)m_errhp, OCI_HTYPE_ERROR);
    } else if (m_connected == CONNECTED_LOCAL) {
		if (m_srvhp && m_svchp && m_usrhp && m_transhp && m_errhp) {
            OCISessionEnd(m_svchp, m_errhp, m_usrhp, OCI_DEFAULT);
            OCIHandleFree((dvoid *)m_transhp, OCI_HTYPE_TRANS);
            OCIHandleFree((dvoid *)m_usrhp, OCI_HTYPE_SESSION);
            OCIServerDetach(m_srvhp, m_errhp, OCI_DEFAULT);
            OCIHandleFree(m_srvhp, OCI_HTYPE_SERVER);
            OCIHandleFree(m_svchp, OCI_HTYPE_SVCCTX);
            OCIHandleFree(m_errhp, OCI_HTYPE_ERROR);
        }
    } else if ((m_connected == CONNECTED_OCCI)&&(m_errhp)) {
    	 OCIHandleFree((dvoid *)m_transhp, OCI_HTYPE_TRANS);
         OCIHandleFree((dvoid *)m_errhp, OCI_HTYPE_ERROR);
    }

    m_connected = CONNECTED_NOT;
}

/**
 *  \fn void UDBI::TConnect::TransBegin()
 *  开启事务，非XA 方式下的事务控制
 */
void UDBI::TConnect::TransBegin()
{
	UDBI_TRACE("TConnect::TransBegin()");
	
	OCIAttrSet((dvoid *)m_svchp, (ub4)OCI_HTYPE_SVCCTX, (dvoid *)m_transhp, 
               (ub4)0, (ub4)OCI_ATTR_TRANS, (OCIError *)m_errhp );
    CHECKERR(m_errhp, OCITransStart(m_svchp, m_errhp, (uword)0, OCI_DEFAULT) );
    
    this->m_trans = true;
}

/**
 *  \fn void UDBI::TConnect::TransCommit() 
 *  提交事务，非XA 方式下的事务控制
 */
void UDBI::TConnect::TransCommit()
{
	UDBI_TRACE("TConnect::TransCommit()");
	
    CHECKERR(m_errhp, OCITransCommit(m_svchp, m_errhp, (ub4)OCI_DEFAULT) );
    
    this->m_trans = false;
}

/**
 *  \fn void UDBI::TConnect::TransRollback()
 *  回滚事务，非XA 方式下的事务控制
 */
void UDBI::TConnect::TransRollback()
{
	UDBI_TRACE("TConnect::TransRollback()");
	
    CHECKERR(m_errhp, OCITransRollback(m_svchp, m_errhp, (ub4)OCI_DEFAULT) );
    
    this->m_trans = false;
}

/**
 *  \fn bool UDBI::TConnect::isTrans() 
 *  UDBI事务判断，事务中返回TRUE，否则返回FALSE
 */
bool UDBI::TConnect::isTrans()
{
	return this->m_trans;	
}

/**
 *  \fn UDBI::JTable* UDBI::TConnect::getJTable()
 *  返回一个单件JTable* 
 */

/**
 *  \fn UDBI::JTable *UDBI::TConnect::CreateJTable()
 *  创建JTable接口
 */
UDBI::JTable* UDBI::TConnect::CreateJTable(const int prefetchRecs)
{
	return new TTable(this->m_envhp, this->m_svchp, prefetchRecs);
}

/**
 *  \fn UDBI::JTable *UDBI::TConnect::CreateJTable(const string& sTabName)
 *  创建JTable接口，并用sTabName参数初始化
 */
UDBI::JTable* UDBI::TConnect::CreateJTable(const string& sTabName, const int prefetchRecs)
{
	return new TTable(this->m_envhp, this->m_svchp, sTabName, prefetchRecs);
}

//! 2005.12.20 WangFJ.
/**
 *  \fn UDBI::JTable *UDBI::TConnect::CreateCursor(const int prefetchRecs)
 *  创建JCursor接口
 */
UDBI::JCursor* UDBI::TConnect::CreateCursor(const int prefetchRecs)
{
	return new TCursor(this->m_envhp, prefetchRecs);
}

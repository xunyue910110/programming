/** 
 * @file TConnect.h
 * 数据库连接接口实现类
 *
 * @author WangFJ
 * @author &copy; All Rights Reserved By NanJing Linkage Tech. Co., Ltd.
 * @date    2004-2005
 *
 * @sa contact:
 *   http://www.lianchuang.com 
 */        

#ifndef __TCONNECT_H_
#define __TCONNECT_H_

#include "udbi-inter.h"
#include "JEConnect.h"

namespace UDBI {
using std::string;

const int CONNECTED_NOT = 0;
const int CONNECTED_LOCAL = 1;
const int CONNECTED_XA = 2;
const int CONNECTED_OCCI = 3;


//! 数据库连接类，封装数据库连接、连接上的事务控制功能
class TConnect : public JEConnect {

public:

	//! 构造函数，不创建数据库连接（iOption为CONNECTED_NOT）或for xa（iOption为非CONNECTED_NOT）
    TConnect(OCIEnv* vpEnv, const int iOption=CONNECTED_NOT);
    
    //! 构造函数，非xa方式连接
    TConnect(OCIEnv* vpEnv, const string& cTns, const string& cUser, const string& cPassword);
    
    //! 构造函数，for occi混合编程支持，借用已有连接
    TConnect(OCIEnv* vpEnv, OCISvcCtx* vpSvc);
    
    //! 析构函数
    virtual ~TConnect();
    
    //! 数据库连接函数，非xa方式创建数据库连接
    virtual void Connect(const string& cTns, const string& cUser, const string& cPassword);
    
    //! 数据库连接函数，xa方式创建数据库连接或获取已有xa连接
    virtual void ConnectXA();
    
    //! 断开数据库连接、释放资源
    virtual void DisConnect();

	//! 事务控制（非XA方式）
    virtual void TransBegin();		// 开启事务，非XA方式
    virtual void TransCommit();		// 提交事务，非XA方式
    virtual void TransRollback();	// 回滚事务，非XA方式
    virtual bool isTrans();			// 判断是否已启事务

	//! 创建JTable* 
	virtual JTable* getJTable() { return jtable_; }
	virtual JTable* CreateJTable(const int prefetchRecs=UDBI_PREFETCH_RECS);
	virtual JTable* CreateJTable(const string& sTabName, const int prefetchRecs=UDBI_PREFETCH_RECS);
	
	//! 2005.12.20  WangFJ.
	virtual JCursor* CreateCursor(const int prefetchRecs=UDBI_PREFETCH_RECS);

private:
    OCIEnv* m_envhp;
    OCIError* m_errhp;
    OCISvcCtx* m_svchp;

    OCIServer* m_srvhp;
    OCISession* m_usrhp;
    OCITrans* m_transhp;
    int m_connected;
    bool m_trans;
    
    JTable* jtable_;
    
};


}// namespace UDBI


#endif /* __TConnect_H_ */

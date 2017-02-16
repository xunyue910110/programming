/** Copyright (c) 2004, 联创科技
 *  All rights reserved.
 * 
 *  文件名称：jdb.h
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

#ifndef __JCONNECT_H_
#define __JCONNECT_H_

#include "udbi.h"
#include "JTable.h"

namespace UDBI {
using std::string;

// 数据库连接接口
class JConnect {
public:
	virtual ~JConnect() {};
    virtual void Connect(const string& cTns, const string& cUser, const string& cPassword)=0;
    virtual void ConnectXA()=0;
    virtual void DisConnect()=0;

    virtual void TransBegin()=0;  // 开启事务，非XA方式
    virtual void TransCommit()=0; // 提交事务，非XA方式
    virtual void TransRollback()=0; // 回滚事务，非XA方式
    virtual bool isTrans()=0;		// 事务中返回TRUE，否则返回FALSE

	//! 获取一个单例（singleton）的JTable对象的指针，连接对象内部维护，
	//! 不需要也不允许使用者手工做delete 操作
	virtual JTable* getJTable()=0;
	
	//! 创建JTable 对象，返回指针，需要使用者自己进行delete
	virtual JTable* CreateJTable(const int prefetchRecs=UDBI_PREFETCH_RECS)=0;
	//! 创建JTable 对象，返回指针，需要使用者自己进行delete
	virtual JTable* CreateJTable(const string& sTabName, const int prefetchRecs=UDBI_PREFETCH_RECS)=0;
	
};


}// namespace UDBI


#endif /* __JCONNECT_H_ */

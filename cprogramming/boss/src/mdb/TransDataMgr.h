// ############################################
// Source file : TransDataMgr.h
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : chen min
// E_mail      : chenm@lianchuang.com
// Create      : 2008-5-4
// Update      : 2008-5-4
// Copyright(C): chen min, Linkage.
// ############################################

#ifndef TRANSDATAMGR_H_INCLUDED
#define TRANSDATAMGR_H_INCLUDED

#include <map>

#include "PublicInfo.h"

typedef multimap<void *,TransData *> TRANSDATA_POOL;
typedef vector<TransData *> TRRANSDATA_VECTOR;
typedef TRANSDATA_POOL::iterator TRANSDATA_POOL_ITR; 

// 事务私有数据管理器,存放 记录的实际地址(key) 和 transData 结构
// 1、delete、update操作,先执行selectWithTrans,根据返回的地址,在TRANSDATA_POOL里根据address匹配：
//    如果匹配到,则使用TRANSDATA_POOL中的数据,否则,将选出的数据插入TRANSDATA_POOL
// 2、insert操作,直接插入TRANSDATA_POOL,地址为0
//	  如果之前,有对相同索引记录的delete操作,table类的commit方法,先执行delete操作,再执行insert,没问题
//    如果之后,有对记录的delete/update操作,那么地址值都是0,也应该能匹配到

class TransDataMgr
{
	public:
		TransDataMgr();
		~TransDataMgr();
		
		// 根据输入TransData中的记录地址,查找是否缓存中有匹配
		TransData *matchTransData(TransData *pTransData);
		bool clearTransData();
		bool insertTransData(TransData *pTransData);
		TRRANSDATA_VECTOR & getTransDataPool();		
		
	private:
		TRANSDATA_POOL m_transDataPool;
		TRRANSDATA_VECTOR m_transDataVector;
		
};

#endif //TRANSDATAMGR_H_INCLUDED

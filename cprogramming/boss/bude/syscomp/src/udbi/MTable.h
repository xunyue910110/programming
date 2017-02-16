/** 
 * @file MTable.h
 * DAO父类，通过JTable* 完成数据库访问
 *
 * @author WangFJ
 * @author &copy; All Rights Reserved By NanJing Linkage Tech. Co., Ltd.
 * @date    2004-2005
 *
 * @sa contact:
 *   http://www.lianchuang.com 
 */        

#ifndef __MTABLE_H_
#define __MTABLE_H_

#include "JTable.h"

namespace UDBI {
using std::string;

/**
 *  \class MTable   MTable.h
 *  \brief MTable 封装直接引用单件连接实例功能，主要用于DAO封装
 * 
 *  所有DAO封装从此类继承
 */
// class JTable;

class MTable {
public:
	//! 设置从数据库端获取数据时每次获取记录数
	void SetPrefetchRecs(const int prefetchRecs) 
	{
		m_ptab->SetPrefetchRecs(prefetchRecs);
	}		
	
	//! 设置查询结果中字符型字段为空时返回的值（对所有字符型字段）
    void SetNullText(const string& nullText) 
    {
    	m_ptab->SetNullText(nullText);
    }
    
    //! 添加SQL语句
    void AddSql(const string& sql) 
    {
    	m_ptab->AddSql(sql);
    }
	    	
	//! 参数设置函数，支持int& , char [], const string& , int [], char **类型参数
    void SetParam(const string& par, int val, short* isNull=0)
    {
    	m_ptab->SetParam(par, val, isNull);
    }
    void SetParam(const string& par, double val, short* isNull=0)
    {
    	m_ptab->SetParam(par, val, isNull);
    }
    void SetParam(const string& par, const string& val, short* isNull=0)
    {
    	m_ptab->SetParam(par, val, isNull);
    }
    void SetParam(const string& par, vector<int>& vval, short* isNull=0)
    {
    	m_ptab->SetParam(par, vval, isNull);
    }
    void SetParam(const string& par, vector<string>& vval, int len, short* isNull=0)
    {
    	m_ptab->SetParam(par, vval, len, isNull);
    }
    
    void BindParam(const string& par, int& val, short* isNull=0)
    {
    	m_ptab->BindParam(par, val, isNull);
    }
    void BindParam(const string& par, double& val, short* isNull=0)
    {
    	m_ptab->BindParam(par, val, isNull);
    }
    void BindParam(const string& par, char* buf, int buflen, short* isNull=0)
    {
    	m_ptab->BindParam(par, buf, buflen, isNull);
    }
    
    /// 增补函数
    void SetParam(const string& par, int* pival, int arraylen, short* isNull=0)
    {
    	m_ptab->SetParam(par, pival, arraylen, isNull);
    }
    void SetParam(const string& par, char* pcval, int clen, int arraylen, short* isNull=0)
    {
    	m_ptab->SetParam(par, pcval, clen, arraylen, isNull);
    }
    
    //! SQL 语句执行函数，执行增、删、改、查
    //! 如果需要引用缓存的SQL语句则需要传入xxBy 参数，缺省为空，则执行AddSql设置的SQL语句
    //! 如同时传入xxBy参数和通过AddSql设置了SQL语句，则执行xxBy引用到的SQL语句
    int ExecSelect(const string& selBy="")
    {
    	return m_ptab->ExecSelect(selBy);
    }    
    int ExecUpdate(const string& updBy="")
    {
    	return m_ptab->ExecUpdate(updBy);
    }    
    int ExecDelete(const string& delBy="") 
    {
    	return m_ptab->ExecDelete(delBy);
    }    
    int ExecInsert(const string& insBy="") 
    {
    	return m_ptab->ExecInsert(insBy);
    }
    
    int ExecExt(int exTimes)
    {
    	return m_ptab->ExecExt(exTimes);
    }
    int ExecExt(const string& extBy="")
    {
    	return m_ptab->ExecExt(extBy);
    }
    int ExecExt(const string& extBy, int exTimes)
    {
    	return m_ptab->ExecExt(extBy, exTimes);
    }

	//! 从数据库端获取数据，每次fetch一条记录，没有数据时返回False
    bool Fetch() { return m_ptab->Fetch(); }
    
    //! 元数据信息获取，已实现字段的数据类型、字段大小
    int GetFieldType(const string& fldName)
    {
    	return m_ptab->GetFieldType(fldName);
    }
    int GetFieldSize(const string& fldName)
    {
    	return m_ptab->GetFieldSize(fldName);
    }

	//! 判断查询结果中当前记录的指定字段是否为空
    bool isNull(const string& fldName)
    {
    	return m_ptab->isNull(fldName);
    }
    bool isNull(const int fldNum)
    {
    	return m_ptab->isNull(fldNum);
    }

	//! GET查询结果中字段值，目前实现返回int, int64_t, string型值
    int GetInt(const string& fldName, bool* ifNull=0, const int nullValue=-1)
    {
    	return m_ptab->GetInt(fldName, ifNull, nullValue);
    }
    int64_t GetInt64(const string& fldName, bool* ifNull=0, const int nullValue=-1)
    {
    	return m_ptab->GetInt64(fldName, ifNull, nullValue);
    }
    string GetString(const string& fldName, bool* ifNull=0)
    {
    	return m_ptab->GetString(fldName, ifNull);
    }
    
    int GetInt(const int fldNum, bool* ifNull=0, const int nullValue=-1) 
    {
    	return m_ptab->GetInt(fldNum, ifNull, nullValue);
    }
    int64_t GetInt64(const int fldNum, bool* ifNull=0, const int nullValue=-1)
    {
    	return m_ptab->GetInt64(fldNum, ifNull, nullValue);
    }
    string GetString(const int fldNum, bool* ifNull=0)
    {
    	return m_ptab->GetString(fldNum, ifNull);
    }

	//! 清空函数
	void Clear() { m_ptab->Clear(); }
	
	//! 参数调试信息输出
	string parToString()
	{
		return m_ptab->parToString();
	}

protected:
	//! 带表名参数的构造函数，该表名用于匹配缓存的SQL语句（同sqlRef一起）
    MTable(const string& tabName);
    
    //! 析构函数
	virtual ~MTable() { delete m_ptab; }

private:
	JTable* m_ptab;	
	
};

}

#endif

/** 
 * @file TTable.h
 * SQL语句操作实现类
 *
 * @author WangFJ
 * @author &copy; All Rights Reserved By NanJing Linkage Tech. Co., Ltd.
 * @date    2004-2005
 *
 * @sa contact:
 *   http://www.lianchuang.com 
 */        

#ifndef __TTABLE_H_
#define __TTABLE_H_

#include "udbi-inter.h"
#include "JETable.h"
#include "TParam.h"
#include "JRecord.h"

namespace UDBI {
using std::string;

/**
  * SQL语句操作接口类，封装参数设置、语句执行、查询结果访问功能
  */
class TTable : public JETable {
	
public:
	
	//! 通用构造函数
    TTable(OCIEnv* vpEnv, OCISvcCtx* vpSvc, const int prefetchRecs=UDBI_PREFETCH_RECS);
    
    //! 带表名参数的构造函数，该表名用于匹配缓存的SQL语句（同sqlRef一起）
    TTable(OCIEnv* vpEnv, OCISvcCtx* vpSvc, const string& tabName, const int prefetchRecs=UDBI_PREFETCH_RECS);
    
    //! 析构函数
    virtual ~TTable();
    
    //! 设置从数据库端获取数据时每次获取记录数
	virtual void SetPrefetchRecs(const int prefetchRecs);
	
	//! 设置查询结果中字符型字段为空时返回的值（对所有字符型字段）
	virtual void SetNullText(const string& nullText);
	
	//! 添加SQL语句
	virtual void AddSql(const string& sql);
	virtual string GetSql(void)
	{
		return m_sql;
	}
    
    //! 参数设置函数，支持int& , char [], const string& , int [], char* *类型参数
    virtual void SetParam(const string& par, int val, short* isNull=0);
    virtual void SetParam(const string& par, double val, short* isNull=0);
    virtual void SetParam(const string& par, const string& val, short* isNull=0);
    virtual void SetParam(const string& par, vector<int>& vval, short* isNull=0);
    virtual void SetParam(const string& par, vector<string>& vval, int len, short* isNull=0);

    virtual void BindParam(const string& par, int& val, short* isNull=0);
    virtual void BindParam(const string& par, double& val, short* isNull=0);
    virtual void BindParam(const string& par, char* buf, int buflen, short* isNull=0);
    
    //! 2005.12.20  WangFJ.
    virtual void BindParam(const string& par, JCursor* pcur);

    /// 增补函数、用于数组参数绑定
    virtual void SetParam(const string& par, int* pival, int arraylen, short* isNull=0);
    virtual void SetParam(const string& par, char* pcval, int clen, int arraylen, short* isNull=0);

    //! SQL 语句执行函数，执行增、删、改、查
    //! 如果需要引用缓存的SQL语句则需要传入xxBy 参数，缺省为空，则执行AddSql设置的SQL语句
    //! 如同时传入xxBy参数和通过AddSql设置了SQL语句，则执行xxBy引用到的SQL语句
    virtual int ExecSelect(const string& selBy="");
    virtual int ExecUpdate(const string& updBy="");
    virtual int ExecDelete(const string& delBy="");
    virtual int ExecInsert(const string& insBy="");
	
	virtual int ExecExt(int exTimes);
	virtual int ExecExt(const string& extBy="");
	virtual int ExecExt(const string& extBy, int exTimes);

    //! 从数据库端获取数据，每次fetch一条记录，没有数据时返回False
    virtual bool Fetch();
    
    //! 元数据信息获取，已实现字段的数据类型、字段大小
    virtual int GetFieldType(const string& fldName);
    virtual int GetFieldSize(const string& fldName);

	//! 判断查询结果中当前记录的指定字段是否为空
    virtual bool isNull(const string& fldName);
    virtual bool isNull(const int fldNum);

    //! GET查询结果中字段值，目前实现返回int, int64_t, string型值
    virtual int GetInt(const string& fldName, bool* ifNull=0, const int nullValue=-1);
    virtual int64_t GetInt64(const string& fldName, bool* ifNull=0, const int nullValue=-1);
    virtual string GetString(const string& fldName, bool* ifNull=0);
    
    virtual int GetInt(const int fldNum, bool* ifNull=0, const int nullValue=-1);
    virtual int64_t GetInt64(const int fldNum, bool* ifNull=0, const int nullValue=-1);
    virtual string GetString(const int fldNum, bool* ifNull=0);

    //! 清空函数，清空
    virtual void Clear();
    
    //! 参数调试信息输出
    virtual string parToString();

	///
	virtual int GetFieldCount();
	virtual string GetFieldName(int fldNum);
    
protected:
    OCIEnv* m_envhp;
    OCIError* m_errhp;
    OCISvcCtx* m_svchp;
    OCIStmt* m_stmthp;
    string m_TabName;
    int m_PreFetchRecs;
    
    int Prepare();
    int Execute(int exTimes=1);
    void BindParams();

private:
	TTable();
    int m_CanFetch;
    
    int m_CurRec;
    int m_AllFetchedRecs;
    int m_LastFetchedRecs;

    string m_sql;
    ub2 m_StmtType;

    JParam  m_params;
    JRecord  m_records;
};


} // namespace UDBI


#endif /* __TTABLE_H_ */

#ifndef  __TCURSOR_H_
#define  __TCURSOR_H_

#include "udbi-inter.h"
#include "JRecord.h"
#include "JCursor.h"

namespace UDBI {

using std::string;
	
class TCursor : public JCursor {
public:

	//!构造函数
	TCursor(OCIEnv* vpEnv, const int prefetchRecs=UDBI_PREFETCH_RECS);
	
	//! 析构函数
	virtual ~TCursor();
		
	//! 光标关闭、打开函数
	virtual void Close();
	virtual void Open();

	//! 设置从数据库端获取数据时每次获取记录数
	virtual void SetPrefetchRecs(const int prefetchRecs);

	//! 设置查询结果中字符型字段为空时返回的值（对所有字符型字段）
    virtual void SetNullText(const string& nullText);

	//! 判断查询结果中当前记录的指定字段是否为空
    virtual bool isNull(const string& fldName);
    virtual bool isNull(const int fldNum);

	//! GET查询结果中字段值，目前实现返回int, int64_t, string型值
    virtual int GetInt(const string& fldName, bool *ifNull, const int nullValue=-1);
    virtual int64_t GetInt64(const string& fldName, bool *ifNull, const int nullValue=-1);
    virtual string GetString(const string& fldName, bool *ifNull);
    
    virtual int GetInt(const int fldNum, bool *ifNull, const int nullValue=-1);
    virtual int64_t GetInt64(const int fldNum, bool *ifNull, const int nullValue=-1);
    virtual string GetString(const int fldNum, bool *ifNull);
    
    virtual OCIStmt** stmtHandle();
    
    virtual bool Fetch();
    
protected:
    OCIEnv* m_envhp;
    OCIError* m_errhp;
    OCISvcCtx* m_svchp;
    OCIStmt* m_stmthp;
    int m_PreFetchRecs;	    
	
private:
    int m_CanFetch;
    
    int m_CurRec;
    int m_AllFetchedRecs;
    int m_LastFetchedRecs;

    JRecord  m_records;
};

	
} // namespace UDBI

#endif /*  __TCURSOR_H_  */

#ifndef  __JCURSOR_H_
#define  __JCURSOR_H_

class OCIStmt;

namespace UDBI {
	
class JCursor {
public:

	//! 析构函数
	virtual ~JCursor() {};
	
	//! 设置从数据库端获取数据时每次获取记录数
	virtual void SetPrefetchRecs(const int prefetchRecs)=0;
	
	virtual void Close()=0;
	virtual void Open()=0;

	//! 设置查询结果中字符型字段为空时返回的值（对所有字符型字段）
    virtual void SetNullText(const string& nullText)=0;

	//! 判断查询结果中当前记录的指定字段是否为空
    virtual bool isNull(const string& fldName)=0;
    virtual bool isNull(const int fldNum)=0;

	//! GET查询结果中字段值，目前实现返回int, int64_t, string型值
    virtual int GetInt(const string& fldName, bool *ifNull=0, const int nullValue=-1)=0;
    virtual int64_t GetInt64(const string& fldName, bool *ifNull=0, const int nullValue=-1)=0;
    virtual string GetString(const string& fldName, bool *ifNull=0)=0;
    
    virtual int GetInt(const int fldNum, bool *ifNull=0, const int nullValue=-1)=0;
    virtual int64_t GetInt64(const int fldNum, bool *ifNull=0, const int nullValue=-1)=0;
    virtual string GetString(const int fldNum, bool *ifNull=0)=0;
		
	virtual OCIStmt** stmtHandle()=0;
	
	//! 从数据库端获取数据，每次fetch一条记录，没有数据时返回False
    virtual bool Fetch()=0;
};
	
	
} // namespace UDBI

#endif  /* __JCURSOR_H_  */

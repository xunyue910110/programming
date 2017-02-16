/** Copyright (c) 2004, 联创科技
 *  All rights reserved.
 * 
 *  文件名称：
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
#include "udbi-inter.h"
#include "JRecord.h"

#include "TCursor.h"

/**
 *  void UDBI::TCursor::TCursor(OCIEnv* vpEnv, const int prefetchRecs)
 *  构造函数
 */
UDBI::TCursor::TCursor(OCIEnv* vpEnv, const int prefetchRecs):
	m_envhp(vpEnv), m_errhp(0), m_stmthp(0), m_PreFetchRecs(prefetchRecs)
{
    m_CurRec = 0;
    m_CanFetch = 0;
    m_LastFetchedRecs = 0;
    m_AllFetchedRecs = 0;

    (void)OCIHandleAlloc((dvoid *)m_envhp, (dvoid **)&m_errhp, (ub4)OCI_HTYPE_ERROR, (size_t)0, (dvoid **)0);
    (void)OCIHandleAlloc((dvoid *)m_envhp, (dvoid **)&m_stmthp, OCI_HTYPE_STMT, (size_t)0, (dvoid **)0);	
}

/**
 *  void UDBI::TCursor::~TCursor()
 *  析构函数
 */
UDBI::TCursor::~TCursor()
{
	UDBI_TRACE("TCursor::~TCursor()");
	
    if (m_stmthp && m_errhp) {
        (void)OCIHandleFree(m_stmthp, OCI_HTYPE_STMT);
        (void)OCIHandleFree(m_errhp, OCI_HTYPE_ERROR);
    }
}

OCIStmt** UDBI::TCursor::stmtHandle()
{
	return &m_stmthp;
}

/**
 *  void UDBI::TCursor::Close()
 *  关闭光标
 */
void UDBI::TCursor::Close()
{
    m_CurRec = 0;
    m_CanFetch = 0;
    m_LastFetchedRecs = 0;
    m_AllFetchedRecs = 0;
 	
	m_records.Clear();
}

/**
 *  void UDBI::TCursor::Open()
 *  打开光标
 */
void UDBI::TCursor::Open()
{
    m_records.init(m_stmthp, m_errhp, m_PreFetchRecs);
    m_records.Define(m_stmthp, m_errhp);	
}
 

/**
 *  \fn void UDBI::TCursor::SetNullText(const string& nullText)
 *  设置字符串字段在查询结果为空时返回的值
 */
void UDBI::TCursor::SetNullText(const string& nullText)
{
	m_records.SetNullText(nullText);
}

/**
 *  \fn void UDBI::TCursor::SetPrefetchRecs(const int prefetchRecs)
 *  设置每次从数据库获取记录数
 */
void UDBI::TCursor::SetPrefetchRecs(const int prefetchRecs)
{
	m_PreFetchRecs = prefetchRecs;
}

/**
 *  \fn bool UDBI::TCursor::Fetch()
 *  查询结果Fetch函数，如果当前缓存仍有获取数据则移动记录指针，否则执行一次从数据库获取数据操作
 *  每次从数据库获取记录数由m_PreFetchRecs参数控制，该参数可以通过SetPrefetchRecs()方法设置。如
 *  果有结果数据则返回True，否则返回False
 *  @return bool 如果有结果，返回Ture，否则返回False
 */
bool UDBI::TCursor::Fetch()
{
    UDBI_TRACE_P2("TCursor::Fetch()[ARECS=%d, CURR=%d]", m_AllFetchedRecs, m_CurRec);
    
    unsigned tmpFetched=0;
    
    m_CurRec++;
    if (m_CurRec==m_LastFetchedRecs && m_LastFetchedRecs<m_PreFetchRecs ) {
        m_CanFetch = 0;
    } else if (m_CurRec==m_LastFetchedRecs ||!m_LastFetchedRecs) {
        OCIStmtFetch(m_stmthp, m_errhp, (ub4)m_PreFetchRecs, OCI_FETCH_NEXT, OCI_DEFAULT);
        tmpFetched = m_AllFetchedRecs;
        CHECKERR(m_errhp, OCIAttrGet(m_stmthp, OCI_HTYPE_STMT, &m_AllFetchedRecs, 0, OCI_ATTR_ROW_COUNT, m_errhp));
        m_LastFetchedRecs = m_AllFetchedRecs - tmpFetched;

		UDBI_DEBUG_P2("TCursor::Fetch()[m_afr=%d, m_lfr=%d]", m_AllFetchedRecs, m_LastFetchedRecs);
		
        if (!m_LastFetchedRecs) {
             m_CanFetch = 0;
        } else {
            m_CanFetch = 1;
            m_CurRec = 0;
        }
    }

    return (m_CanFetch==1)?true:false;
}


/**
 *  \fn bool UDBI::TCursor::isNull(const string& fldName)
 *  查询结果记录中指定字段值是否为空
 *  @return bool  如果为空，TRUE；不为空，FALSE
 */
/**
 *  \overload bool UDBI::TCursor::isNull(const int fldNum)
 */
bool UDBI::TCursor::isNull(const string& fldName)
{
	return m_records.isNull(fldName, m_CurRec);
}

bool UDBI::TCursor::isNull(const int fldNum)
{
	return m_records.isNull(fldNum, m_CurRec);
}

/**
 *  \fn int UDBI::TCursor::GetInt(const string& fldName, bool *ifNull, const int nullValue)
 *  查询结果记录中指定字段值读取函数，返回字段的\c int型值
 *  @param fldName  字段名, 常量字符串
 *  @param ifNull 传出参数, bool指针, 返回当前记录中本字段结果值是否为空, 如不关心是否为空可以不指定, 缺省为nil
 *	@param nullValue 空值, 整型值, 如指定, 则当前记录中如本字段为空, 则返回nullValue, 缺省为-1
 */
/**
 *  \fn int64_t UDBI::TCursor::GetInt64(const string& fldName, bool *ifNull, const int nullValue)
 *  查询结果记录中指定字段值读取函数，返回字段的\c int64_t 型值
 *  @param fldName  字段名, 常量字符串
 *  @param ifNull 传出参数, bool指针, 返回当前记录中本字段结果值是否为空, 如不关心是否为空可以不指定, 缺省为nil
 *	@param nullValue 空值, 整型值, 如指定, 则当前记录中如本字段为空, 则返回nullValue, 缺省为-1
 */
/**
 *  \fn std::string UDBI::TCursor::GetString(const string& fldName, bool *ifNull)
 *  查询结果记录中指定字段值读取函数，返回字段的\c std::string 型值, 空返回SetNullText()方法设置的字符串, 缺省为空串
 *  @param fldName  字段名, 常量字符串
 *  @param ifNull 传出参数, bool指针, 返回当前记录中本字段结果值是否为空, 如不关心是否为空可以不指定, 缺省为nil
 */
/**
 *  \overload int UDBI::TCursor::GetInt(const int fldNum, bool *ifNull, const int nullValue)
 */
/**
 *  \overload int64_t UDBI::TCursor::GetInt64(const int fldNum, bool *ifNull, const int nullValue)
 */
/**
 *  \overload std::string UDBI::TCursor::GetString(const int fldNum, bool *ifNull)
 */ 
int UDBI::TCursor::GetInt(const string& fldName, bool* ifNull, const int nullValue)
{
	if (!m_CanFetch) THROW_C_P1(EUdbiException, 90000, "%s:查询结果集为空！", fldName.c_str());
	
    return m_records.GetInt(fldName, m_CurRec, ifNull, nullValue);
}

int64_t UDBI::TCursor::GetInt64(const string& fldName, bool* ifNull, const int nullValue)
{
	if (!m_CanFetch) THROW_C_P1(EUdbiException, 90000, "%s:查询结果集为空！", fldName.c_str());
	
	return m_records.GetInt64(fldName, m_CurRec, ifNull, nullValue);
}

std::string UDBI::TCursor::GetString(const string& fldName, bool* ifNull)
{
	if (!m_CanFetch) THROW_C_P1(EUdbiException, 90000, "%s:查询结果集为空！", fldName.c_str());
	
    return  m_records.GetString(fldName, m_CurRec, ifNull);
}

int UDBI::TCursor::GetInt(const int fldNum, bool* ifNull, const int nullValue)
{
	if (!m_CanFetch) THROW_C_P1(EUdbiException, 90000, "%d:查询结果集为空！", fldNum);
	
    return m_records.GetInt(fldNum, m_CurRec, ifNull, nullValue);
}

int64_t UDBI::TCursor::GetInt64(const int fldNum, bool* ifNull, const int nullValue)
{
	if (!m_CanFetch) THROW_C_P1(EUdbiException, 90000, "%d:查询结果集为空！", fldNum);
	
    return m_records.GetInt64(fldNum, m_CurRec, ifNull, nullValue);
}

std::string UDBI::TCursor::GetString(const int fldNum, bool* ifNull)
{
	if (!m_CanFetch) THROW_C_P1(EUdbiException, 90000, "%d:查询结果集为空！", fldNum);
	
    return m_records.GetString(fldNum, m_CurRec, ifNull);
}

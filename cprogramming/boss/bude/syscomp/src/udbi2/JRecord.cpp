/** Copyright (c) 2004, 联创科技
 *  All rights reserved.
 * 
 *  文件名称：JRecord.cpp
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

#include "JRecord.h"

#include "JFieldString.h"
#include "JFieldNumber.h"
#include "JFieldDate.h"

#include <iostream>

/** \class UDBI::JRecord   JRecord.h
 *  \brief 记录类
 *
 *  JRecord类组合各JField类构造SQL语句的返回结果集，并对外提供统一的结果集访问方法。
 *  在SQL语句执行之后，通过JRecord对该语句的定义信息进行描述并准备合适的结果缓存以
 *  在fetch时缓存数据。
 *  JRecord也封装了结果集中各字段元数据信息的访问方法。
 *
 */
             
/**
 *  缺省构造函数
 */
UDBI::JRecord::JRecord():m_nFields(0), m_ppFields(0), m_stmthp(0), m_errhp(0)
{
	UDBI_TRACE("JRecord::JRecord()");
}
                    
/**
 *  构造函数，分析语句句柄属性，生成对应的JFields字段，准备接收查询结果记录
 */
UDBI::JRecord::JRecord(OCIStmt* vpStmt, OCIError* vpErr, int preFetchRecs):
	m_nFields(0), m_ppFields(0), m_stmthp(vpStmt), m_errhp(vpErr)
{
	UDBI_TRACE("JRecord::JRecord(OCIStmt* vpStmt, OCIError* vpErr, int preFetchRecs)");
	 
    init(vpStmt,vpErr,preFetchRecs);
}

/**
 *  析构函数
 */
UDBI::JRecord::~JRecord()
{
	UDBI_TRACE_P1("JRecord::~JRecord()[FieldsCount=%d]", m_nFields);
	
	Clear();    
}

/**
 *  初始化函数，分析语句句柄属性，实例化对应的JFields，其中：
 *  JFieldString实现以下数据库类型：char，varchar2，long
 *  JFieldNumber实现以下数据库类型：number
 *  JFieldDate实现以下数据库类型：date
 *  其他数据库类型，如raw，rowid，ref，lob等数据库类型还未实现
 */
void UDBI::JRecord::init(OCIStmt* vpStmt, OCIError* vpErr, int preFetchRecs)
{
	//UDBI_TRACE("JRecord::init(OCIStmt* vpStmt, OCIError* vpErr, int preFetchRecs)");
	
	m_stmthp = vpStmt;
	m_errhp = vpErr;
	m_nFields = 0;
	m_nullText.clear();
	
	ub2 colType;
	OCIParam* paramd=0;
	
	CHECKERR(m_errhp, OCIAttrGet((dvoid *)m_stmthp, OCI_HTYPE_STMT, 
					(dvoid *)&m_nFields, (ub4 *)0, (ub4)OCI_ATTR_PARAM_COUNT, m_errhp) );
									
    m_fields_map.clear();
    m_ppFields = new JField*[m_nFields];
    for (int i=0; i<m_nFields; i++) {
        m_ppFields[i] = 0;
    }
    
    UDBI_TRACE_P1("JRecord::init()[nFields=%d]", m_nFields);
    
    for (int i=0; i<m_nFields; i++) {
    	CHECKERR(m_errhp, OCIParamGet((dvoid *)m_stmthp, OCI_HTYPE_STMT, m_errhp, (dvoid **)&paramd, i+1));
    	CHECKERR(m_errhp, OCIAttrGet((dvoid *)paramd, OCI_DTYPE_PARAM, (dvoid *)&colType, 
    								(ub4 *)0, (ub4)OCI_ATTR_DATA_TYPE, m_errhp) );
    	switch (colType) {
    		case SQLT_CHR:
    		case SQLT_LNG:
    		case SQLT_AFC:
    			m_ppFields[i] = new JFieldString(paramd, m_errhp, colType, preFetchRecs);
    			break;
    		case SQLT_NUM:
    			m_ppFields[i] = new JFieldNumber(paramd, m_errhp, colType, preFetchRecs);
    			break;
    		case SQLT_DAT:
    			m_ppFields[i] = new JFieldDate(paramd, m_errhp, colType, preFetchRecs);
    			break;
    		case SQLT_BIN:
    		case SQLT_LBI:
    		case SQLT_LAB:
    		case SQLT_RID:
    		case SQLT_RDD:
    		case SQLT_CLOB:
    		case SQLT_BLOB:
    		case SQLT_FILE:
    		case SQLT_RSET:
    		case SQLT_NTY:
    		case SQLT_REF:
    			THROW_C_P1(EUdbiException, 9001, "[%d]:该类型字段目前不支持！", colType);
    			break;
    		default:
    			//m_ppFields[i] = new JFieldDummy(paramd, m_errhp, colType, preFetchRecs);
    			m_ppFields[i] = new JFieldString(paramd, m_errhp, colType, preFetchRecs);
    			break;
    	}    		
    	
    	string fldName;
    	m_ppFields[i]->GetName(fldName);
    	m_fields_map[fldName] = i;
    }
}

/**
 *  定义结果数据BUF与SQL语句的关联
 */
void UDBI::JRecord::Define(OCIStmt* vpStmt, OCIError* vpErr)
{
	UDBI_DEBUG_P1("JRecord::Define()[Fields=%d]", m_nFields);	

	for (int i=0; i<m_nFields; i++) {
		m_ppFields[i]->Define(vpStmt, i);
	}
}

/**
 *  \fn int UDBI::JRecord::GetFieldCount() const
 *  获取记录包含字段个数
 */
/*
int UDBI::JRecord::GetFieldCount() const
{
    return m_nFields;
}
*/

/**
 *  获取指定位置的字段名
 */
void UDBI::JRecord::GetFieldName(string& vFieldName, int fldNum) const
{
    if (fldNum<0 || fldNum>m_nFields) 
        THROW_C_P1(EUdbiException, 9000, "%d:该字段在结果集中不存在！", fldNum);

    m_ppFields[fldNum]->GetName(vFieldName);
}

/**
 *  获取指定位置的字段大小
 */
int UDBI::JRecord::GetFieldSize(int fldNum) const
{
    if (fldNum<0 || fldNum>m_nFields) 
        THROW_C_P1(EUdbiException, 9000, "%d:该字段在结果集中不存在！", fldNum);

    return (int)m_ppFields[fldNum]->GetSize();
}

/**
 *  获取指定名字的字段大小
 */
int UDBI::JRecord::GetFieldSize(const string& fldName) const
{
    map<string, int>::const_iterator iter = m_fields_map.find(fldName);
    if (iter == m_fields_map.end()) 
    	THROW_C_P1(EUdbiException, 9000, "%s:该字段在结果集中不存在！", fldName.c_str());
    	
    return (int)m_ppFields[iter->second]->GetSize();
}

/** 
 *  获取指定位置的字段数据类型编码
 */
int UDBI::JRecord::GetFieldType(int fldNum) const
{
    if (fldNum<0 || fldNum>m_nFields)
        THROW_C_P1(EUdbiException, 9000, "%d:该字段在结果集中不存在！", fldNum);

    return (int)m_ppFields[fldNum]->GetType();
}

/**
 *  获取指定名字的字段数据类型编码
 */
int UDBI::JRecord::GetFieldType(const string& fldName) const
{
    map<string, int>::const_iterator iter = m_fields_map.find(fldName);
    if (iter == m_fields_map.end()) 
    	THROW_C_P1(EUdbiException, 9000, "%s:该字段在结果集中不存在！", fldName.c_str());
    	
    return (int)m_ppFields[iter->second]->GetType();
}

/**
 *  获取指定位置的字段数据类型名称
 */
void UDBI::JRecord::GetFieldTypeName(string& vFieldTypeName, int fldNum) const
{
    if (fldNum<0 || fldNum>m_nFields)
        THROW_C_P1(EUdbiException, 9000, "%d:该字段在结果集中不存在！", fldNum);

    m_ppFields[fldNum]->GetTypeName(vFieldTypeName);
}

/**
 *  获取指定名字的字段数据类型名称
 */
void UDBI::JRecord::GetFieldTypeName(string& vFieldTypeName, const string& fldName) const
{
    map<string, int>::const_iterator iter = m_fields_map.find(fldName);
    if (iter == m_fields_map.end()) 
    	THROW_C_P1(EUdbiException, 9000, "%s:该字段在结果集中不存在！", fldName.c_str());
    	
    m_ppFields[iter->second]->GetTypeName(vFieldTypeName);
}

/**
 *  设置记录中字符串字段为空时返回的值
 */
void UDBI::JRecord::SetNullText(const string& nullText)
{
	m_nullText.assign(nullText);
}

/**
 *  判断指定位置字段的返回结果是否为NULL
 */
bool UDBI::JRecord::isNull(int fldNum, int recNum) const
{
    if (fldNum<0 || fldNum>m_nFields) 
        THROW_C_P1(EUdbiException, 9000, "%d:该字段在结果集中不存在！", fldNum);

    return m_ppFields[fldNum]->isNull(recNum);
}

/**
 *  判断指定名字的字段返回结果是否为NULL
 */
bool UDBI::JRecord::isNull(const string& fldName, int recNum) const
{
    map<string, int>::const_iterator iter = m_fields_map.find(fldName);
    if (iter == m_fields_map.end()) 
    	THROW_C_P1(EUdbiException, 9000, "%s:该字段在结果集中不存在！", fldName.c_str());
    	
    return m_ppFields[iter->second]->isNull(recNum);
}

/**
 *  返回指定记录的指定名字的字段结果整型值
 */
int UDBI::JRecord::GetInt(const string& fldName, int recNum, bool* ifNull, const int nullValue)
{
	//UDBI_DEBUG_P2("JRecord::GetInt(const string& fldName='%s', int recNul=%d)", fldName.c_str(), recNum);
	
	int retInt;
	
    map<string, int>::const_iterator iter = m_fields_map.find(fldName);
    if (iter == m_fields_map.end()) 
    	THROW_C_P1(EUdbiException, 9000, "%s:该字段在结果集中不存在！", fldName.c_str());
    
	if ( (m_ppFields[iter->second])->GetType() != SQLT_NUM )
		THROW_C_P1(EUdbiException, 9000, "%s:试图从一个非NUMBER型字段获取整型值！", fldName.c_str());
	
	if ( (m_ppFields[iter->second])->isNull(recNum) ) 
	{
		if (ifNull) *ifNull = true;
		return nullValue;
	} 
	else
	{    		
    	if (ifNull) *ifNull = false;
    	(dynamic_cast<JFieldNumber *>(m_ppFields[iter->second]))->GetInt(retInt, recNum);
    	return retInt;
    }
}

/**
 *  返回指定记录的指定名字的字段结果长整型值
 */
int64_t UDBI::JRecord::GetInt64(const string& fldName, int recNum, bool* ifNull, const int nullValue)
{
	//UDBI_DEBUG_P2("JRecord::GetInt64(const string& fldName='%s', int recNul=%d)", fldName.c_str(), recNum);
	
	int64_t retInt64;
	
    map<string, int>::const_iterator iter = m_fields_map.find(fldName);
    if (iter == m_fields_map.end()) 
    	THROW_C_P1(EUdbiException, 9000, "%s:该字段在结果集中不存在！", fldName.c_str());
    	
	if ( (m_ppFields[iter->second])->GetType() != SQLT_NUM )
		THROW_C_P1(EUdbiException, 9000, "%s:试图从一个非NUMBER型字段获取整型值！", fldName.c_str());
	
	if ( (m_ppFields[iter->second])->isNull(recNum) ) 
	{
		if (ifNull) *ifNull = true;
		return nullValue;
	} 
	else
	{    		
    	if (ifNull) *ifNull = false;
    	(dynamic_cast<JFieldNumber *>(m_ppFields[iter->second]))->GetInt64(retInt64, recNum);
    	return retInt64;
    }
}

/**
 *  返回指定记录的指定名字的字段结果字符串型值
 */
string UDBI::JRecord::GetString(const string& fldName, int recNum, bool* ifNull)
{
	//UDBI_DEBUG_P2("JRecord::GetString(const string& fldName='%s', int recNul=%d)", fldName.c_str(), recNum);
	
    string retStr;
	
	if (!m_ppFields) THROW_C_P1(EUdbiException, 9000, "%s:记录字段未创建！", fldName.c_str());
	
    map<string, int>::const_iterator iter = m_fields_map.find(fldName);
    if (iter == m_fields_map.end()) 
    	THROW_C_P1(EUdbiException, 9000, "%s:该字段在结果集中不存在！", fldName.c_str());
    	
	if ( (m_ppFields[iter->second])->isNull(recNum) ) 
	{
		if (ifNull) *ifNull = true;
		return m_nullText;
	} 
	else
	{    		
    	if (ifNull) *ifNull = false;
    	(m_ppFields[iter->second])->GetString(retStr, recNum);
    	return retStr;
    }
}

/**
 * \overload int UDBI::JRecord::GetInt(int fldNum, int recNum, bool* ifNull, const int nullValue)
 */
int UDBI::JRecord::GetInt(int fldNum, int recNum, bool* ifNull, const int nullValue)
{
	//UDBI_DEBUG_P2("JRecord::GetInt(int fldNum=%d, int recNum=%d)", fldNum, recNum);
	
    int retInt;

    if (fldNum<0 || fldNum>m_nFields)
        THROW_C_P1(EUdbiException, 9000, "%d:该字段在结果集中不存在！", fldNum);
    
    if ( (m_ppFields[fldNum])->GetType() != SQLT_NUM ) 
    	THROW_C_P1(EUdbiException, 9000, "%d:试图从一个非NUMBER型字段获取整型值！", fldNum);
    
    if ( (m_ppFields[fldNum])->isNull(recNum) )
    {
    	if (ifNull) *ifNull = true;
    	return nullValue;
    }
    else
    {
    	if (ifNull) *ifNull = false;    	
        (dynamic_cast<JFieldNumber *>(m_ppFields[fldNum]))->GetInt(retInt, recNum);
        return retInt;
    }
}

/**
 * \overload int64_t UDBI::JRecord::GetInt64(int fldNum, int recNum, bool* ifNull, const int nullValue)
 */
int64_t UDBI::JRecord::GetInt64(int fldNum, int recNum, bool* ifNull, const int nullValue)
{
	//UDBI_DEBUG_P2("JRecord::GetInt64(int fldNum=%d,int recNum=%d)", fldNum, recNum);
	
    int64_t retInt;

    if (fldNum<0 || fldNum>m_nFields)
        THROW_C_P1(EUdbiException, 9000, "%d:该字段在结果集中不存在！", fldNum);
    
    if ( (m_ppFields[fldNum])->GetType() != SQLT_NUM ) 
    	THROW_C_P1(EUdbiException, 9000, "%d:试图从一个非NUMBER型字段获取整型值！", fldNum);
    
    if ( (m_ppFields[fldNum])->isNull(recNum) )
    {
    	if (ifNull) *ifNull = true;
    	return nullValue;
    }
    else
    {
    	if (ifNull) *ifNull = false;    	
        (dynamic_cast<JFieldNumber *>(m_ppFields[fldNum]))->GetInt64(retInt, recNum);
        return retInt;
    }
}

/**
 * \overload string UDBI::JRecord::GetString(int fldNum, int recNum, bool *ifNull)
 */
string UDBI::JRecord::GetString(int fldNum, int recNum, bool* ifNull)
{
	//UDBI_DEBUG_P2("JRecord::GetString(int fldNum=%d,int recNum=%d)", fldNum, recNum);
	
    string retStr;
    
    if (!m_ppFields) THROW_C_P1(EUdbiException, 9000, "%d:字段对象未生成！", fldNum);

    if (fldNum<0 || fldNum>m_nFields) {
        THROW_C_P1(EUdbiException, 9000, "%d:该字段在结果集中不存在！", fldNum);
    }

	if ( (m_ppFields[fldNum])->isNull(recNum) )
	{
		if (ifNull) *ifNull = true;
		return m_nullText;
	}
	else
	{
		if (ifNull) *ifNull = false;
		m_ppFields[fldNum]->GetString(retStr, recNum);
		return retStr;
	}
}

/**
 *  数据成员清空，初始化
 */
void UDBI::JRecord::Clear()
{
    for (int i=0; i<m_nFields; i++) {
        delete m_ppFields[i];
    }
    delete [] m_ppFields;
    m_ppFields = 0;
    m_nFields = 0;
    
    m_fields_map.clear();
}


/** Copyright (c) 2004, 联创科技
 *  All rights reserved.
 * 
 *  文件名称：JField.cpp
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

#include "JField.h"

/** \class UDBI::JField    JField.h
 *  \brief 字段类
 *  
 *  JField是一个字段封装的基类，定义获取字段元数据的接口方法、字段绑定方法、字段结果
 *  访问方法；组件中继承JField之后实现有JFieldNumber、JFieldString、JFieldDate类。
 *  
 */

/**
 *  构造函数，根据输入字段类型获取、字段描述句柄解析字段元数据，分配对应的数据BUF
 */
UDBI::JField::JField(OCIParam* vpParam, OCIError* vpErr, ub2 vFieldType, int vRecs):
    m_errhp(vpErr), m_definehp(0), m_NullIndicator(0), m_nRecs(vRecs)
{
    char* tmpColName=0;
    text* colName=0;
    ub4 colNameLen=0;

    // col name 
    CHECKERR(m_errhp, OCIAttrGet((dvoid *)vpParam, OCI_DTYPE_PARAM, 
              (dvoid **)&colName, (ub4 *)&colNameLen, (ub4)OCI_ATTR_NAME, m_errhp) );
    tmpColName = (char *)calloc(colNameLen+1, sizeof(char));
    
    if (tmpColName == 0) {
    	THROW_C(EUdbiException, 90021, "JField::JField() 分配内存失败！");
    }
    
    for (unsigned i=0; i<colNameLen; i++) {
        tmpColName[i] = colName[i];
    }
    m_ColName.assign(tmpColName);
    free(tmpColName);

    // col type
    m_ColType = vFieldType;
	SetTypeName();
	
    // null indicator
    m_NullIndicator = (sb2 *)calloc(m_nRecs+1, sizeof(sb2));
    
    UDBI_TRACE_P2("JField::JField()[ColName='%s',ColType='%s']", m_ColName.c_str(), m_ColTypeName.c_str());
}

/**
 *  析构函数
 */
UDBI::JField::~JField()
{
 	UDBI_TRACE_P1("JField::~JField()[ColName='%s']", m_ColName.c_str());
 	
    free(m_NullIndicator);
}

/**
 *  GET字段名
 */
void UDBI::JField::GetName(string& vFieldName) const
{
    vFieldName.assign(m_ColName);
}

/**
 *  GET字段数据类型（编码）
 */
int UDBI::JField::GetType() const
{
    return (int)m_ColType;
}

/**
 *  GET字段数据类型（名字）
 */
void UDBI::JField::GetTypeName(string& vFieldTypeName) const
{
    vFieldTypeName.assign(m_ColTypeName);
}

/**
 *  GET字段大小
 */
unsigned UDBI::JField::GetSize() const
{
    return m_ColSize;
}

/**
 *  GET请求变量的数据类型（编码）
 */
int UDBI::JField::GetReqType() const
{
    return (int)m_ReqType;
}

/**
 *  判断指定记录的字段结果值是否为NULL
 */
bool UDBI::JField::isNull(int rec) const
{
	if (m_NullIndicator)
       return (m_NullIndicator[rec]==-1)?1:0;
    else
       return false;
}

/**
 *  内部方法，根据数据类型编码设置数据类型名称
 */
void UDBI::JField::SetTypeName()
{
	switch (m_ColType) {
		case SQLT_CHR:
			m_ColTypeName.assign("VARCHAR2");
			break;
		case SQLT_NUM:
			m_ColTypeName.assign("NUMBER");
			break;
		case SQLT_LNG:
			m_ColTypeName.assign("LONG");
			break;
		case SQLT_RDD:
		case SQLT_RID:
			m_ColTypeName.assign("ROWID");
			break;
		case SQLT_DAT:
			m_ColTypeName.assign("DATE");
			break;
		case SQLT_BIN:
			m_ColTypeName.assign("RAW");
			break;
		case SQLT_LBI:
			m_ColTypeName.assign("LONG RAW");
			break;
		case SQLT_AFC:
			m_ColTypeName.assign("CHAR");
			break;
		case SQLT_LAB:
			m_ColTypeName.assign("MLSLABEL");
			break;
		case SQLT_REF:
			m_ColTypeName.assign("REF");
			break;
		case SQLT_CLOB:
			m_ColTypeName.assign("CLOB");
			break;
		case SQLT_BLOB:
			m_ColTypeName.assign("BLOB");
			break;
		case SQLT_FILE:
			m_ColTypeName.assign("BFILE");
			break;
		case SQLT_RSET:
			m_ColTypeName.assign("Nested Table");
			break;
		default:
			m_ColTypeName.assign("UnKnown");
			break;
	}		
}

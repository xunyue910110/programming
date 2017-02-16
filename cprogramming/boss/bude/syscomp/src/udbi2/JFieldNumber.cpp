/** Copyright (c) 2004, 联创科技
 *  All rights reserved.
 * 
 *  文件名称：JFieldNumber.cpp
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

#include <sstream>

#include "JFieldNumber.h"

/** \class UDBI::JFieldNumber  JFieldNumber.h
 *  \brief number型的字段类
 *
 */

UDBI::JFieldNumber::JFieldNumber(OCIParam* vpParam, OCIError* vpErr, ub2 vFieldType, int vRecs):
	JField(vpParam, vpErr, vFieldType, vRecs), data(0)
{
	ub2 prec;
	m_ReqType = SQLT_STR;
	
	CHECKERR(m_errhp, OCIAttrGet((dvoid *)vpParam, OCI_DTYPE_PARAM, (dvoid *)&prec, (ub4 *)0, (ub4)OCI_ATTR_PRECISION, m_errhp) );
	if (!prec) {
		m_ColSize=255;
	} else {
		m_ColSize=(unsigned)prec;
	}
	
	data=(char *)calloc((m_ColSize+1)*m_nRecs, sizeof(char));
}

UDBI::JFieldNumber::~JFieldNumber()
{
	free(data);
}


void UDBI::JFieldNumber::GetString(string& val, int curRec)
{
	if (data) {
		val.assign(data+(m_ColSize+1)*curRec);
	} else {
		THROW_C_P1(EUdbiException, 90008, "%s:结果集数据为空！", m_ColName.c_str());
	}
}
	
void UDBI::JFieldNumber::GetInt(int& val, int curRec)
{
	if (data) {
		val=atoi(data+(m_ColSize+1)*curRec);
	} else {
		THROW_C_P1(EUdbiException, 90008, "%s:结果集数据为空！", m_ColName.c_str());
	}
}

void UDBI::JFieldNumber::GetInt64(int64_t& val, int curRec)
{
	if (data) {
#ifdef HPUX_OS_CONFIG		
		stringstream s;
		if (!(s << (data+(m_ColSize+1)*curRec) && s >> val))
		   THROW_C_P1(EUdbiException, 90008, "%s:结果转int64_t操作失败！", m_ColName.c_str());
#else		
		val = atoll(data+(m_ColSize+1)*curRec);
#endif		
	} else {
		THROW_C_P1(EUdbiException, 90008, "%s:结果集数据为空！", m_ColName.c_str());
	}
}

void UDBI::JFieldNumber::Define(OCIStmt* vpStmt, int fldPos)
{
	CHECKERR(m_errhp, OCIDefineByPos(vpStmt, &m_definehp, m_errhp, (ub4)(fldPos+1), 
               (dvoid *)data, (sb4)(m_ColSize+1), (sb2)m_ReqType, (dvoid *)m_NullIndicator, 0, (ub2 *)0, OCI_DEFAULT) );
}

	

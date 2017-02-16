/** Copyright (c) 2004, 联创科技
 *  All rights reserved.
 * 
 *  文件名称：JFieldString.cpp
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

#include "JFieldString.h"

/** \class UDBI::JFieldString   JFieldString.h
 *  \brief sting型的字段类
 *
 */

UDBI::JFieldString::JFieldString(OCIParam* vpParam, OCIError* vpErr, ub2 vFieldType, int vRecs):
	JField(vpParam, vpErr, vFieldType, vRecs), data(0)
{
	ub2 tmpColSize=0;
	m_ReqType = SQLT_STR;
	
	CHECKERR(m_errhp, OCIAttrGet((dvoid *)vpParam, OCI_DTYPE_PARAM, 
              (dvoid *)&tmpColSize, (ub4 *)0, (ub4)OCI_ATTR_DATA_SIZE, m_errhp) );
	
	m_ColSize = (unsigned)(tmpColSize?tmpColSize:4000);
	data = (char *)calloc((m_ColSize+1)*m_nRecs, sizeof(char));
}

UDBI::JFieldString::~JFieldString()
{
	free(data);
}

void UDBI::JFieldString::GetString(string& val, int curRec)
{
	if (data) {
		val.assign(data+(m_ColSize+1)*curRec);
	} else {
		THROW_C_P1(EUdbiException, 90008, "%s:结果集数据为空！", m_ColName.c_str());
	}
}

void UDBI::JFieldString::Define(OCIStmt* vpStmt, int fldPos)
{
	CHECKERR(m_errhp, OCIDefineByPos(vpStmt, &m_definehp, m_errhp, fldPos+1, 
              data, m_ColSize+1, m_ReqType, (void *)m_NullIndicator, 0, (ub2 *)0, OCI_DEFAULT));
}
	
	

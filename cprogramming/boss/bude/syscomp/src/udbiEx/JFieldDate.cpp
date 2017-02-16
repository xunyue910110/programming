/** Copyright (c) 2004, 联创科技
 *  All rights reserved.
 * 
 *  文件名称：JFieldDate.cpp
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

#include "JFieldDate.h"

/** \class UDBI::JFieldDate   JFieldDate.h
 *  \brief date型的字段类
 * 
 *  实现日期型字段类的缓存分配、定义、字符串格式化输出功能。
 */

UDBI::JFieldDate::JFieldDate(OCIParam* vpParam, OCIError* vpErr, ub2 vFieldType, int vRecs):
	JField(vpParam, vpErr, vFieldType, vRecs), date(0)
{
	m_ReqType = SQLT_DAT;
	m_ColSize = 7; // YYYY-MM-DD HH:MI:SS
	date = (char *)calloc(7*m_nRecs, sizeof(char));	
}

UDBI::JFieldDate::~JFieldDate()
{
	free(date);
}

void UDBI::JFieldDate::Define(OCIStmt* vpStmt, int fldPos)
{
	CHECKERR(m_errhp, OCIDefineByPos(vpStmt, &m_definehp, m_errhp, fldPos+1, 
              date, m_ColSize, m_ReqType, (void *)m_NullIndicator, 0, (ub2 *)0, OCI_DEFAULT));	
}

void UDBI::JFieldDate::GetString(string& val, int curRec)
{
	if (!date) THROW_C_P1(EUdbiException, 90008, "%s:结果集数据为空！", m_ColName.c_str());
	
	char buf[20];
	unsigned char cc,yy,mm,dd,hh,mi,ss;
	cc = (date+7*curRec)[0];	
	yy = (date+7*curRec)[1];
	mm = (date+7*curRec)[2];
	dd = (date+7*curRec)[3];
	hh = (date+7*curRec)[4]-1;
	mi = (date+7*curRec)[5]-1;
	ss = (date+7*curRec)[6]-1;
	
	cc = (unsigned char)abs(cc-100);
	yy = (unsigned char)abs(yy-100);
	
	sprintf(buf, "%d-%02d-%02d %02d:%02d:%02d", (unsigned int)cc*100+(unsigned int)yy,(int)mm,(int)dd,(int)hh,(int)mi,(int)ss);
	val.assign(buf);	
}

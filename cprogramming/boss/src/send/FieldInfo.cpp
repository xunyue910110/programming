// ##########################################
// Source file : FieldInfo.cpp
// System      : Mobile Billing System
// Version     : 0.1.5
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010911
// Update      : 20020415
// Copyright(C): 2002 by Yang Zhang, Linkage.
// ##########################################

#include "FieldInfo.h"

//##ModelId=3B9EC5A2025F
//int FieldInfo::reset()
//{
//	m_IsFormat			= false;
//	m_IsCheck			= false;
//    m_errorCode         = 0;
//	m_FieldLength		= 0;
//	
//	//memset(m_FieldSrcValue,0,sizeof(m_FieldSrcValue));
//	m_FieldSrcValue[0]  = 0;
//	m_padding           = 0;
//	m_order             = 0;
//
//	//m_FieldDesValue.clear();  by chenmin 08-27 for hp
//	//m_dataType.clear();
//	m_FieldDesValue="";
//	m_dataType="";
//
//	return 0;
//}
#include <iostream>
//##ModelId=3BD6296403BC
FieldInfo::FieldInfo(int iBufferSize)
{
    if(iBufferSize>1)
    {
    	m_FieldSrcValue = new unsigned char[iBufferSize];
    	memset(m_FieldSrcValue,0,iBufferSize);
	}
	else
	{
		m_FieldSrcValue = new unsigned char[5];
    	memset(m_FieldSrcValue,0,5);	
	}
//	m_IsFormat			= false;
//	m_IsCheck			= false;
    m_errorCode         = 0;
	m_FieldLength		= 0;
	
	m_padding           = 0;
	m_order             = 0;

	//m_FieldDesValue.clear();  by chenmin 08-27 for hp
	//m_dataType.clear();
	m_FieldDesValue="";
	m_dataType="";
}

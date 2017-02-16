// ##########################################
// Source file : GAsnCdrExtractor.h
// System      : Mobile Billing System
// Version     : 0.1.15
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010816
// Update      : 20011025
// Copyright(C): 2001 by Yang Zhang, Linkage.
// ##########################################

#ifndef GASNEXTRACTCDR_H_INCLUDED_C4864087
#define GASNEXTRACTCDR_H_INCLUDED_C4864087

#include "config-all.h"

#include <string>
#include "CdrExtractor.h"
#include "AsnFormat.h"

USING_NAMESPACE(std)

#include "appfrm/ErrorHandle.h"
extern ErrorMessages * const theErrorMessages;

//## add by chenm 2005-3-17 
//## 与 AsnCdrExtractor 不同的是,该类处理每条cdr以不同的tag作为开始标志的ans.1格式的原始话单
//## ASN变长类型记录处理对象。通用类，继承自GeneralCdr抽象类。
class GAsnCdrExtractor : public CdrExtractor
{
	public:
		//## 该方法先调用ASN_1对象中的方法，求出记录中的全部元素，再根据字段定义得到字段值。
		virtual int getNextSrcCdr(unsigned char *&srcCdrBuffer);
		virtual bool initialize(const string theCdrParameter,const int theBlockHeaderSize) ;
		virtual ~GAsnCdrExtractor();
	
	private:	
		vector<int> m_nTags;
		vector<string> m_Tags;
		bool           m_isHuawGprs;

};

#endif 

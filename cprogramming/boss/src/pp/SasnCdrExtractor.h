// ##########################################
// Source file : SasnCdrExtractor.h
// System      : Mobile Billing System
// Version     : NG1.0
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Gao xile
// E_mail      : gaoxl@lianchuang.com
// Create      : 20080619
// Update      : 20080619
// Copyright(C): 2008 by Gao xile, Linkage.
// ##########################################

#ifndef SASNCDREXTRACTOR_H_HEADER_INCLUDED_C4864087
#define SASNCDREXTRACTOR_H_HEADER_INCLUDED_C4864087

#include "CdrExtractor.h"
#include "SasnFormat.h"
#include "config-all.h"
#include "appfrm/ErrorHandle.h"
#include <string>

USING_NAMESPACE(std);

extern ErrorMessages * const theErrorMessages;

//##ModelId=3B5145EF01A1
//##Documentation
//## SASN变长类型记录处理对象. 通用类,继承自CdrExtractor抽象类
class SasnCdrExtractor : public CdrExtractor
{
  public:
	//##ModelId=3B5152020084
	//##Documentation
	//## 该方法先调用ASN_1对象中的方法,求出记录中的全部元素,再根据字段定义得到字段值。
	virtual int getNextSrcCdr(unsigned char *&srcCdrBuffer);

	//##ModelId=3B8209FE0181
	virtual bool initialize(const string &theCdrParameter,const int theBlockHeaderSize) ;

	//##ModelId=3B97358703D6
	virtual ~SasnCdrExtractor();

  private:

	//##ModelId=3B84EF1602EA
	string m_Tag;

};



#endif /* SASNCDREXTRACTOR_H_HEADER_INCLUDED_C4864087 */

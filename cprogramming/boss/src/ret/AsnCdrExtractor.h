// ##########################################
// Source file : AsnCdrExtractor.h
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

#ifndef ASNEXTRACTCDR_H_INCLUDED_C4864087
#define ASNEXTRACTCDR_H_INCLUDED_C4864087

#include "config-all.h"

#include <string>
#include "CdrExtractor.h"
#include "AsnFormat.h"

USING_NAMESPACE(std)

#include "appfrm/ErrorHandle.h"
extern ErrorMessages * const theErrorMessages;

//##ModelId=3B5145EF01A1
//##Documentation
//## ASN变长类型记录处理对象。通用类，继承自GeneralCdr抽象类。
class AsnCdrExtractor : public CdrExtractor
{
  public:
	//##ModelId=3B5152020084
	//##Documentation
	//## 该方法先调用ASN_1对象中的方法，求出记录中的全部元素，再根据字段定义得到字段值。
	virtual int getNextSrcCdr(unsigned char *&srcCdrBuffer);

	//##ModelId=3B8209FE0181
	virtual bool initialize(const string theCdrParameter,const int theBlockHeaderSize) ;

	//##ModelId=3B97358703D6
	virtual ~AsnCdrExtractor();

  private:

	//##ModelId=3B84EF1602EA
	string m_Tag;

};



#endif /* ASNEXTRACTCDR_H_INCLUDED_C4864087 */

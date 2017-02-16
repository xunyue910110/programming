// ##########################################
// Source file : CdrFlag.h
// System      : Mobile Billing System
// Version     : 0.1.3
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010928
// Update      : 20020101
// Copyright(C): 2001 by Yang Zhang, Linkage.
// ##########################################

#ifndef CDRFLAG_H_INCLUDED_C44BD2BA
#define CDRFLAG_H_INCLUDED_C44BD2BA

#include "config-all.h"

#include <string>
USING_NAMESPACE(std)


//##ModelId=3BB4288603BD
//##Documentation
//## 话单记录标志类，静态全局对象。
class CdrFlag
{
  public:
	//##ModelId=3BB42A560324
	void clear();

	//##ModelId=3BB42ADF02F9
	static CdrFlag *getInstance();

	//##ModelId=3BFCAD1602EE
	~CdrFlag();

	//##ModelId=3BB42A7701B9
	int m_errorCode;

	//##ModelId=3BB42A8501EB
	bool m_isInvalid;

	//##ModelId=3BB42ABF0158
	bool m_isError;

	//##ModelId=3BD82B630398
	//##Documentation
	//## 呼叫类型（两位字符）
	char m_callType[4];

	//##ModelId=3BFCAAD3006A
	int m_cdrNo;

	//##ModelId=3C35514E0374
	string m_fieldName;

	//##ModelId=3C35563801F7
	bool m_errorFlag;

  private:
	//##ModelId=3BB429AB021A
	static CdrFlag *m_cdrFlag;
	
	//##ModelId=3BFCACC90351
	CdrFlag();



};

#endif /* CDRFLAG_H_INCLUDED_C44BD2BA */

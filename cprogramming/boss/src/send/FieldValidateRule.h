// ##########################################
// Source file : FieldValidateRule.h
// System      : Mobile Billing System
// Version     : 0.1.4
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010921
// Update      : 20011122
// Copyright(C): 2001 by Yang Zhang, Linkage.
// ##########################################

#ifndef VALIDRULE_H_INCLUDED_C45561D8
#define VALIDRULE_H_INCLUDED_C45561D8

#include "config-all.h"
#include <string>
#include <map>
#include <vector>

#include "SendLog.h"
#include "CdrFlag.h"
#include "FieldInfo.h"
#include "CdrErrorCode.h"
#include "AlertCode.h"

USING_NAMESPACE(std)

#include "appfrm/ErrorHandle.h"
extern ErrorMessages * const theErrorMessages;

//##ModelId=3BAAA3F10190
class FieldValidateRule
{
  public:
	//##ModelId=3BD62966007F
   FieldValidateRule();

	//##ModelId=3BCC3A8302FB
   virtual ~FieldValidateRule(){;};

	//##ModelId=3BAAA7CA0006
	virtual bool initialize(string ruleText) = 0;

	//##ModelId=3BAAAA0F016A
	virtual int validField(multimap<MyString,FieldInfo *> &theFieldInfoMaps) = 0;

	//##ModelId=3BFC6A250375
   void substr(string &srcString,string &desString);

	//##ModelId=3BFC6C320366
   void substr(char * src,char *des);

	//##ModelId=3BFC782F0387
	bool setSubstr(string &src);

  protected:
	//##ModelId=3BCC237C007E
   string lrTrim(string theString);

	//##ModelId=3C242F0603D6
   FieldInfo *getFieldMap(multimap<MyString,FieldInfo *> &theFieldInfoMaps,string &theFieldName);

  protected:
	//##ModelId=3BCC1A8B0090
	//##Documentation
	//## 要校验的字段名
	string m_fieldName;

	//##ModelId=3BFC69FE00B2
	int m_substart;

	//##ModelId=3BFC6A09011C
	int m_subLength;

	//##ModelId=3BFCAFA10088
    SendLog *m_SendLog_;

	//##ModelId=3BFCCC48000D
    CdrFlag *m_cdrFlag;

	//##ModelId=3C2429E201DF
	string m_callType;
	//##条件字段名称
	string m_conditionFieldName;
	//##条件字段值
	string m_conditionFieldValue;
  private:



};



#endif /* VALIDRULE_H_INCLUDED_C45561D8 */

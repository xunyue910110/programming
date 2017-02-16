// ##########################################
// Source file : FieldConvertRule_19.h
// System      : Mobile Billing System
// Version     : 0.1.0
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Tao Li
// E_mail      : litao@lianchuang.com
// mender      : li zhao
// E_mail      : lizhao@lianchuang.com
// Create      : 20070803
// Update      : 20080926
// Copyright(C): 2002 by Tao Li, Linkage.
// ##########################################

#ifndef CONVERTRULE_19_H_INCLUDED_C31CF69D
#define CONVERTRULE_19_H_INCLUDED_C31CF69D

#include "FieldConvertRule.h"
//#include "FieldInfo.h"
#include "DataFormat.h"
#include "CdrFlag.h"
#include "PpLog.h"
//#include "RepeatableFieldInfo.h"
//#include "FieldInfos.h"
#include <vector>
class FieldInfos;
class FieldInfo;
class OutputFile;

//##用于通用呼转话单拆分规则
class FieldConvertRule_19 : public FieldConvertRule
{
  public:
	bool initialize(string ruleText);

  	FieldConvertRule_19(const int iSubChannelNum):FieldConvertRule(iSubChannelNum){};
  		
	//virtual bool convertField(multimap<MyString,FieldInfo *> &theFieldInfoMaps);
	virtual ~FieldConvertRule_19();
	virtual bool convertField();

  protected:
    //##ModelId=485647E60271
   // multimap<MyString,FieldInfo *> m_vectFieldMaps;
   map<string,int> m_vectFieldMaps;
    //##ModelId=485647F7035B
  //  FieldInfo *m_FieldInfo;

  private:
	
    //##ModelId=48561581003E
   // bool getField(const string value);

    //##ModelId=4856481D0138
    //bool splitforw(multimap<MyString,FieldInfo *> &theVectFieldMap);
    bool splitforw();
    //##ModelId=485648450177
    string m_callType1;

    //##ModelId=4856485102FD
    string m_mscType;
    
    //##ModelId=4856159F02EE
    vector<string> m_conditionFieldName;

    //##ModelId=485615B10177
    vector<string> m_conditionFieldValue;

    //##ModelId=485615BA0157
    vector<string> m_conditionExp;

    //##ModelId=485615C6001F
    vector<string> m_dstFieldNames;

    //##ModelId=485615D40290
    vector<string> m_dstFieldValues;

    //##ModelId=485615E2000F
    vector<string> m_dstExps;
};

#endif

// ##########################################
// Source file : FieldConvertRule_13.h
// System      : Mobile Billing System
// Version     : 0.1.6
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : li zhao
// E_mail      : lizhao@lianchuang.com
// Create      : 20080903
// Update      : 20080926
// Copyright(C): 2008 by li zhao, Linkage.
// ##########################################


#ifndef FIELDCONVERTRULE_13_H_HEADER_INCLUDED_B7A67081
#define FIELDCONVERTRULE_13_H_HEADER_INCLUDED_B7A67081

#include "FieldConvertRule.h"
#include "RepeatableFieldInfo.h"
class FieldInfos;


//##ModelId=48561F43002E
//用于提供内容计费业务列表合并: serviceid|dataVolumeGPRSUp|dataVolumeGPRSDn
//          业务代码|上行流量|下行流量 的顺序不能改变
class FieldConvertRule_13 : public FieldConvertRule
{
  public:
  	FieldConvertRule_13(const int iSubChannelNum):FieldConvertRule(iSubChannelNum){};
    //##ModelId=48561F5C0261
    virtual bool initialize(string ruleText);

    //##ModelId=48561F640186
    virtual bool convertField();
    
    virtual ~FieldConvertRule_13() {}

  protected:
  	virtual void getFieldObj();

  private:
    // 配置文件中record段中标识用户流量内容的字段名称
	string m_serviceIdName;
		
	// 配置文件中record段中上下行流量字段名称和时长字段名称
	string m_DataUpName;
	string m_DataDnName;
	string m_DurationName;
		
	// 配置文件中outPut段中 标识:"业务代码|上行流量|下行流量"的字段名称
	string m_cbc;
		
	FieldInfo *m_pFieldCbcInfo;
    RepeatableFieldInfo *m_pServiceID;
    RepeatableFieldInfo *m_pDataFlowUps;
    RepeatableFieldInfo *m_pDataFlowDns;
    RepeatableFieldInfo *m_pCbcDu;
   
};



#endif /* FIELDCONVERTRULE_13_H_HEADER_INCLUDED_B7A67081 */

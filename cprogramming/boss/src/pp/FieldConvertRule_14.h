// ##########################################
// Source file : FieldConvertRule_14.h
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

#ifndef FIELDCONVERTRULE_14_H_HEADER_INCLUDED_B7A61CA6
#define FIELDCONVERTRULE_14_H_HEADER_INCLUDED_B7A61CA6

#include "config-all.h"

#include <vector>
#include "FieldConvertRule.h"
#include "FieldInfo.h"
class FieldInfos;
class RepeatableFieldInfo;

USING_NAMESPACE(std)

//cbc流量结构
class CbcListOfFlow
{
	public:
		CbcListOfFlow()
		{
			m_serviceId = "";
			m_dataUp    = 0;
			m_dataDn    = 0;
			m_duration  = 0;
		}
	public:
		string m_serviceId;
		int    m_dataUp;
		int    m_dataDn;
		long   m_duration;
};
//##ModelId=48561FDD003E
//gprs 内容计费表示形式：(均采用十进制)业务代码|上行流量|下行流量多个业务直接用;隔开
//用于提供gprs流量链表中的上下行流量和时长,根据"service_Id"累计的功能
class FieldConvertRule_14 : public FieldConvertRule
{
  public:
  	FieldConvertRule_14(const int iSubChannelNum):FieldConvertRule(iSubChannelNum){};
    //##ModelId=4856200D00DA
    virtual bool initialize(string ruleText);

    //##ModelId=4856201601D4
    virtual bool convertField();

    virtual ~FieldConvertRule_14() {}

  protected:
  	virtual void getFieldObj();
    
 private:
 	    
    bool getFields(const string &ruleStr,string &totalStr,string &srcStr);
    
    long diffSecond(const string & dt1,const string & dt2);
	
 private:
	// 配置文件中record段中标识用户流量发生时间的字段名称
	string m_serviceID;
	
	// 配置文件中record段中上下行流量字段名称
	string m_srcDtName;
	string m_srcDtDnName;
	string m_srcDuratName;
	
	// 配置文件中outPut段中上下行流量字段名称和时长字段名称
	string m_totalDtUpName;
	string m_totalDtDnName;
	string m_totalDuratName;
          
	vector<CbcListOfFlow *> m_pCbcLists; 

    RepeatableFieldInfo *m_pSrcServiceID;
    RepeatableFieldInfo *m_pSrcDataFlowUps;
    RepeatableFieldInfo *m_pSrcDataFlowDns;
    RepeatableFieldInfo *m_pSrcCbcDu;

    RepeatableFieldInfo *m_pDesServiceID;
    RepeatableFieldInfo *m_pDesDataFlowUps;
    RepeatableFieldInfo *m_pDesDataFlowDns;
    RepeatableFieldInfo *m_pDesCbcDu;

  
};



#endif /* FIELDCONVERTRULE_14_H_HEADER_INCLUDED_B7A61CA6 */

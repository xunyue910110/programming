#ifndef BILLING_COMMON_H_HEADER_INCLUDED
#define BILLING_COMMON_H_HEADER_INCLUDED

//#include "base/Types.h"
#include "string.h"
#include "FieldsXX.h"
#include "BillingConstDef.h"
#include <vector>
#include <map>

const int MAX_CYC_NUM = 5;//向前向后初始化的账期数

struct EventServAndSubCycType
{
	T_BIZ_TYPE m_eventId;
	int        m_servId;
	char       m_subCycType;	
};


class ProcSysParam
{
  public:
    T_SYSPARAMCODE   m_sysParamCode; //参数标识
    T_CHANNEL        m_channelNo;    //通道号
    long             m_valueN;       //数值参数
    T_SYSVALUE_STR   m_valueS;       //字符参数
    T_DATE_TIME      m_valueD;       //时间参数
    long             m_preValueN1;   //预留数值1
    long             m_preValueN2;   //预留数值2
    long             m_preValueN3;   //预留数值3
    T_SYSVALUE_STR   m_preValueS1;   //预留字符1
    T_SYSVALUE_STR   m_preValueS2;   //预留字符2
    T_SYSVALUE_STR   m_preValueS3;   //预留字符3
    T_DATE_TIME      m_preValueD1;   //预留时间1
    T_DATE_TIME      m_preValueD2;   //预留时间2
    T_DATE_TIME      m_preValueD3;   //预留时间3
    T_FLAG           m_useTag;       //使用标记
};

class CycParam
{
  public:
    int             m_dbNo;     //库号 add by gaojf 2009-2-13 18:50
    T_EPARCHY_CODE	m_eparchyCode;	//地市编码 add by limeng 2009-8-14
    T_DATE_TIME			m_discntJudgeTime;	//优惠判断时间 add by limeng 2009-8-14
    String<4>			m_monthAcctStatus;	//月结状态标志 add by limeng 2009-8-14
    String<10>			m_auxAcctStatus;	//月结状态辅助标志集 add by limeng 2009-8-14
    T_FLAG			m_acctTag;	//出帐标志 add by limeng 2009-8-14
    T_FLAG			m_fileCreateTag;	//综合账单生成标记 add by limeng 2009-8-14
    T_DATE_TIME			m_openAcctDate;	//出帐标志 add by limeng 2009-8-14
  	T_DATE          m_cycleId;  //帐期YYYYMM
  	T_DATE_TIME     m_beginTime;//起始时间
  	T_DATE_TIME     m_halfTime; //半月时间点
  	T_DATE_TIME     m_endTime;  //结束时间
  	T_FLAG          m_useTag;   //使用标志：代表该帐期全部帐单是否已经生成，可以供用户缴费。
  	T_FLAG          m_addTag;   //附加标志：系统预留的字段，供月末出帐时使用。
  	friend ostream& operator<<(ostream& os,const CycParam& po)
  	{
  	  os<<" cycleId="<<po.m_cycleId
  	    <<" eparchyCode="<<po.m_eparchyCode
  	    <<" acctTag="<<po.m_acctTag
  	    <<" fileCreateTag="<<po.m_fileCreateTag
  	    <<" beginTime="<<po.m_beginTime
  	    <<" halfTime="<<po.m_halfTime
  	    <<" endTime="<<po.m_endTime
  	    <<" useTag="<<po.m_useTag
  	    <<" addTag="<<po.m_addTag;
  	  return os;
  	}
};

struct SubCycPara //增加
{
	int      m_cycId;
	int      m_subCycId;
	char     m_cycTypeCode;
	int      m_relativeCyc;
	int      m_sequenceNo;
	int      m_dayNum;
	T_DATE   m_startTime;
	T_DATE   m_endTime;
};

struct IdDiscount
{
	 T_TPOBJ_ID  m_tpObjId; // 资费实例标识
    T_TP_ID     m_discountId;
    T_DATE_TIME  m_startDate;
    T_DATE_TIME  m_endDate;
    T_EPARCHY_CODE  m_eparchyCode;
};

class UserServParam
{
	public:
				
		//Added by Malone 2009-11-28 13:19 start
		friend int operator<(const UserServParam &r_left,const UserServParam& r_right)
		{
			if ( r_left.m_servParamId == r_right.m_servParamId )
			{
					//参数ID相同时，根据开始逆向排序，获取时取排好序后的第一条
					return (r_left.m_startDate>r_right.m_startDate);
			}
			else
			{
					return (r_left.m_servParamId < r_right.m_servParamId);
			}
		}
		//Added by Malone 2009-11-28 13:19 end

		bool isValid(const T_DATE_TIME &r_startDate,const T_DATE_TIME &r_endDate) const
		{
		 	 if (m_endDate <r_startDate||
			     m_startDate>=r_endDate)
		 	 	return false;
		 	return true; 	
		}
	public:
		long  m_servInsId;
		int  m_servParamId;
		string m_servParamValue;
		T_DATE_TIME m_startDate;
		T_DATE_TIME m_endDate;
};

class UserServ
{
	public:
		
   //根据参数ID取对应的值，找不到返回NULL
   const char* getParamValue(const int &r_paramId) const
   {
   		for(vector<UserServParam>::const_iterator t_itr=m_userServParamList.begin();
      	t_itr!=m_userServParamList.end();++t_itr)
  		{
    		if(t_itr->m_servParamId == r_paramId)
    		{
    			if (t_itr->isValid(m_startDate,m_endDate))
       			return t_itr->m_servParamValue.c_str();
    		}
  		}
  
  	return NULL;
   }
   
	public:
		long       m_servInsId;
		T_SERVER_ID  m_userId;
		int  m_servId;
		char m_mainTag;
		T_DATE_TIME m_startDate;
		T_DATE_TIME m_endDate;
		
		vector <UserServParam> m_userServParamList;
};


//从CVS同步2009-8-19 11:02:38
class UserServState
{
  public:
  			friend bool operator < (const UserServState &r_left,const UserServState &r_right)
    {
      if (r_left.m_serviceId == r_right.m_serviceId)
      {
        return (r_left.m_startDate < r_right.m_startDate);
      }
      else
      {
        return (r_left.m_serviceId < r_right.m_serviceId);
      }        
    } 
    T_SERVER_ID  m_userId;         //用户标识
    int          m_serviceId;      //服务标识
    char         m_mainTag;        //主服务标志	
    char         m_servStateCode;  //服务状态编码      	      
    T_DATE_TIME  m_startDate;      //生效时间             
    T_DATE_TIME  m_endDate;        //失效时间      
};

struct DiscountInfo
{
	enum DISCNT_TYPE
  	{
  		DISCNT_RATE=1,    //批价用
  		DISCNT_MONFEE =2,    //sp包月费用
  		DISCNT_COMMON = 3,    //共用
  		DISCNT_NODEFINED = 0    //未定义
  	};
	T_TP_ID     m_discountId;
  	int     m_bdiscountId;
  	T_DATE_TIME  m_startDate;
  	T_DATE_TIME  m_endDate;
  	DISCNT_TYPE    m_discountType;
	T_RULE_ID     m_specDealTag;
	T_RULE_ID     m_ruleId; 
	long     m_discountValue; //宽带包年费用，如果不是包年转换成包年费用，仅适用于宽带包年add by taijq 2009-12-16 
};


//子帐期类型
const char DAY_CYCTYPE_CODE       = ADDUP_CYCLE_TYPE_DAY          ;   // 日收取周期类型
const char WEEK_CYCTYPE_CODE      = ADDUP_CYCLE_TYPE_WEEK         ;   // 周收取周期类型
const char TENDAY_CYCTYPE_CODE    = ADDUP_CYCLE_TYPE_DAY_TEN      ;   // 旬收取周期类型
const char HALFMONTH_CYCTYPE_CODE = ADDUP_CYCLE_TYPE_MONTH_HALF   ;   // 半月收取周期类型
const char MONTH_CYCTYPE_CODE     = ADDUP_CYCLE_TYPE_MONTH        ;   // 月收取周期类型
const char TWOMONTH_CYCTYPE_CODE  = ADDUP_CYCLE_TYPE_MONTH_DOUBLE ;   // 双月收取周期类型
const char QUARTER_CYCTYPE_CODE   = ADDUP_CYCLE_TYPE_QUARTER      ;   // 季度收取周期类型
const char HALFYEAR_CYCTYPE_CODE  = ADDUP_CYCLE_TYPE_YEAR_HALF    ;   // 半年收取周期类型
const char YEAR_CYCTYPE_CODE      = ADDUP_CYCLE_TYPE_YEAR         ;   // 年收取周期类型

const char CYCTYPE_CODE_MIN       = DAY_CYCTYPE_CODE              ;   // 子账期类型起始值

const int MAX_SUBCYC_CODE = 20;

class ReAccountEventInfo
{    
public:        
    ReAccountEventInfo()
    {
        m_userId="";
        m_msisdn="";
        m_cycId=0;
        m_monthEndTag='0';
        m_eparchyCode="";
        m_openDate="";
        m_idType='0';
    }
    
    ~ReAccountEventInfo() {}    
    
    ReAccountEventInfo& operator=(const ReAccountEventInfo& rhs)
    {
      if (this != &rhs) {
        m_userId = rhs.m_userId;
        m_msisdn = rhs.m_msisdn;
        m_cycId = rhs.m_cycId;
        m_monthEndTag = rhs.m_monthEndTag;
        m_eparchyCode = rhs.m_eparchyCode;
        m_openDate = rhs.m_openDate;
        m_idType = rhs.m_idType;
      }
        
      return *this;
    }   
    
    T_USER_ID               m_userId;
    T_MSISDN                m_msisdn;
    T_DATE                  m_cycId;
    char                    m_monthEndTag;
    T_EPARCHY_CODE          m_eparchyCode;
    T_DATE_TIME             m_openDate;
    char                    m_idType;
};

//Added By Sunam 2009/8/8 13:30:40
//FeePolicyRemidInfo 免费资源提醒配置结构信息
const char FIXVALUE    = '1';
const char OFFSETVALUE = '2';

struct FeePolicyRemidInfo 
{
	T_TP_ID m_feePolicyId;
	int     m_billId;
	int     m_remindLevel;
	char    m_valueType;
	long    m_value;
};

//limeng add 20090818 批价通道信息
struct UserChannel 
{
    Integer       m_beginPartition;
    Integer       m_endPartition;
    T_EPARCHY_CODE   m_eparchyCode;
    T_CHANNEL     m_channelNo;
    int				m_dbNo;
    string			m_ratePath;
};

#endif /* BILLING_COMMON_H_HEADER_INCLUDED*/

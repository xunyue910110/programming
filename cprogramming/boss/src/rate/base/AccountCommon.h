#ifndef ACCOUNTCOMMON_H
#define ACCOUNTCOMMON_H

//帐务处理的数据结构，宏，常量在此定义

#include "config-all.h"
#include "RateCdr.h"//add for rentfeeItem 2011-3-23
#include <stdio.h> 
#include <stdarg.h>

#include <ctime>
#include <cstdio>
#include <string.h>
#include <assert.h>

#include <memory>
#include <vector>
#include <string>
#include <map>
#include <utility>
#include <set>

#if defined(STDIOSTREAM_DISABLE)
    #include <fstream.h>
    #include <strstream.h>
    #include <iostream.h>
#else
    #include <fstream>
    #include <strstream>
    #include <iostream>
#endif

#include "base/Types.h"
#include "FieldsXX.h"
#include "DetailBillMgr.h"
#include "RateError.h"
#include "TpInfo.h"
#include "CondBase.h"
#include "BillingCommon.h"

//生效时间标志,可以在Makefile中定义
//#ifndef  LAST_TIME_VALID
//#define  LAST_TIME_VALID
//#endif


//0 :优惠 1:调账  3：用户间费用转移 其他:特殊处理
enum T_DISCNT_TYPE 
{
 DISCNT_TYPE='0',
 ADJUST_BEFORE_TYPE,
 USER_FEE_TRANS_TYPE,
 ACCT_FEE_TRANS_TYPE,
 OTHER_TYPE 
};

//定义优惠对象类型
//'0':用户 '1':帐户 '2':客户
enum DISCNT_OBJECT_TYPE 
{
 USER_OBJECT_TYPE = '0', 
 ACCT_OBJECT_TYPE, 
 CUST_OBJECT_TYPE
};


typedef T_SERVER_ID  T_USER_ID;
typedef T_SERVER_ID  T_ACCT_ID;
typedef T_SERVER_ID  T_CUST_ID;

//定义无效的用户、帐户、客户、帐单标识值
const T_USER_ID   INVALID_USERID = "\0";
const T_ACCT_ID   INVALID_ACCTID = "\0";
const T_CUST_ID   INVALID_CUSTID = "\0";
const T_OTHER_PARTY NULL_PARTY = "\0";

//系统费用帐目项最大值及相关常量
const int MAX_ITEM   = 100000;
const int MIN_ITEMID = 1;
const int MAX_ITEMID = 99999;

//系统费用来源最大值及相关常量
const int MAX_SOURCETYPE    = 3;
const int MIN_SOURCETYPETAG = 0;
const int MAX_SOURCETYPETAG = 2;

//系统地市标识最大值及相关常量
const int MAX_EPARCHY   = 20;
const int MIN_EPARCHYID = 1;
const int MAX_EPARCHYID = 19;


const unsigned char	INIT_VALID_Tag_2 = '2';	//帐单有效标志初始值，表示有效帐单。
const unsigned char	INIT_VALID_Tag_3 = '3';	//帐单有效标志初始值，表示无效帐单。

//const unsigned char	INIT_FEETYPE = 'Y';	//费用类型默认值Y


//定义默认付费帐目编码
const int DEFAULT_PAYITEM_CODE = -1;

//定义数据流量转成费用后全免流量体现的优惠编码
const int REMOVEDATA_DISCNT_ID = -1;

//默认优惠编码
extern int DEFAULT_DISCNT_CODE;

//用户未生效标志
const char USER_NOTOPEN_TAG = '2';

//定义其他系统常量
const int DATETIMELEN = 12;                   //YYMMDDHHMMSS
const int DAYLEN = 6;                         //YYMMDD
const int BILLSEQUENCE_LEN = 8;               //bill sequence length

const int PRECISION_FACTOR=100;

//最大序列号
const long MAX_SEQ_NO  =  9999;


//帐单费用舍取规则
const char CARRY_MODE_ROUND  = '0';                          //四舍五入    
const char CARRY_MODE_CUT 	 = '1';                          //去尾数法    
const char CARRY_MODE_ADD    = '2';                          //直接进位法 

//互斥类型add by taijq 2009-12-31
const char MUTEX_TYPE_NORMAL = '0';         //完全互斥
const char MUTEX_TYPE_ENDFOLLOW = '1';      //高优先级订购结束月低优先级开始时间紧跟高优先级结束时间

//用户级优惠模式
const char USER_DISCNTMODE_NORMAL = '0';//正常的用户级优惠顺序
const char USER_DISCNTMODE_AFERTACCT = '1';//账户级优惠后的用户级优惠顺序

//定义系统参数信息结构
struct SysParamInfo
{
   string   m_sysParamCode;
   int      m_channelNo;
   int      m_valueN;
   string   m_valueS;
   string   m_valueD;
   int      m_preValueN1;
   int      m_preValueN2;
   int      m_preValueN3;
   string   m_preValueS1;
   string   m_preValueS2;
   string   m_preValueS3;
   string   m_preValueD1;
   string   m_preValueD2;
   string   m_preValueD3;
   char     m_useTag; 
};

//add for rent new version begin
class CondBase;
class RentEvent
{
  public:
    RentEvent(){}
    ~RentEvent(){}
    
    T_BIZ_TYPE      m_eventCode;    //事件类型编码
    T_SUBCYC_TYPE   m_subCycType;   //事件对应的子帐期类型
    T_SERVICE_ID    m_serviceId;    //事件对应的服务编码
    T_COND_ID       m_condId;       //事件是否通过的条件编码
    CondBase *      m_condRule;     //事件条件指针
    
    friend bool operator<(const RentEvent &left,const RentEvent& right)
    {
        if(left.m_subCycType==right.m_subCycType)
        {
            if(left.m_serviceId==right.m_serviceId)
            {
                return (left.m_eventCode < right.m_eventCode);
            }
            else
            {
                return left.m_serviceId<right.m_serviceId;
            }
        }
        else
        {
            return (left.m_subCycType<right.m_subCycType);
        }
    }
};

class RentBillInfo
{
    public:
    RentBillInfo(){}
    ~RentBillInfo(){}
                        
        bool operator < (const RentBillInfo &rentBillInfo) 
        {
            if (m_itemCode == rentBillInfo.m_itemCode)
            {
                if (m_orderNo == rentBillInfo.m_orderNo)
                {                
                    return (m_eventTypeId < rentBillInfo.m_eventTypeId);                    
                }
                else
                {
                    return (m_orderNo < rentBillInfo.m_orderNo);
                }
            }
            else
            {
                return (m_itemCode < rentBillInfo.m_itemCode);
            }        
        }
                    
    public:
            
        int      m_itemCode;
        int      m_eventTypeId;
        int      m_orderNo;
        int      m_sumMode;
        long     m_value;
        
    friend bool operator<(const RentBillInfo &left,const RentBillInfo& right)
    {
        if(left.m_itemCode==right.m_itemCode)
        {
            if(left.m_orderNo==right.m_orderNo)
            {
                return (left.m_eventTypeId < right.m_eventTypeId);
            }
            else
            {
                return left.m_orderNo<right.m_orderNo;
            }
        }
        else
        {
            return (left.m_itemCode<right.m_itemCode);
        }
    }
};

//add for rent new version end


//定义优惠信息
class Discount
{
    public:
        Discount():m_discountId(0),m_adjustInsId(0),m_adjustModeType('1'),m_feepolicyCompId(0) 
		{
			m_condRule = NULL;
		}
        ~Discount() {}
                        
        bool operator < (const Discount &discount) const
        {
            if (m_adjustModeType == discount.m_adjustModeType)
            {
                if (m_priority == discount.m_priority)
                {
                    if (m_idTypeCode == discount.m_idTypeCode)
                    {                        
                        if ((m_idTypeCode == DISCNT_TYPE)|| (m_idTypeCode == OTHER_TYPE))
                        {
                        	if(m_rateTpInfo.m_rateTpId == discount.m_rateTpInfo.m_rateTpId)
                        	{
                        		if (m_discountId ==discount.m_discountId )
                        	  		return (m_feepolicyCompId < discount.m_feepolicyCompId);
                        	  	
                            	return (m_discountId < discount.m_discountId);                        		
                        	}

                            return m_rateTpInfo.m_rateTpId < discount.m_rateTpInfo.m_rateTpId;                       
                        }       
                        else//ADJUSET_BEFORE_TYPE
                        {
                            return (m_adjustInsId < discount.m_adjustInsId);
                        }    
                    }
                    else
                    {
                        return (m_idTypeCode < discount.m_idTypeCode);
                    }       
                }
                else
                {    
                    return (m_priority < discount.m_priority);
                }    
            }
            else
            {
                return (m_adjustModeType < discount.m_adjustModeType);
            }        
        }
                    
    public:
        long       			m_discountId;     //资费实例标识
        T_TP_ID         m_feepolicyCompId; //资费构成标识 add 2009-9-9 20:04:34
        char               m_userDiscntMode;//0:正常的用户级优惠顺序，1:账户级优惠后的用户级优惠顺序add by xuzy 2011.2.7
        long    				m_adjustInsId;    //帐前调帐标识
        int     				m_startCycId;     //起始帐期
        T_DATE_TIME     m_startDate;      //开始时间
        T_DATE_TIME  	  m_endDate;        //结束时间
        int     				m_invalidStateId; //优惠无效状态标识
        T_DISCNT_TYPE  	m_idTypeCode;     //0 :优惠 1:调账  其他:特殊处理
        char    				m_adjustModeType; //调整类型：标明该类型调整记录应该在帐务优惠之前处理还是帐务优惠之后处理。0：帐务优惠前 1：帐务优惠一起 2：帐务优惠后
        int     				m_priority;       //优先级
        char    				m_ifShowNegative; //优惠负值显示标志：针对补收优惠方式，出现的负值优惠是否在综合帐单中进行体现。0：体现 1：不体现
        char            m_effectTimeTag;  //作用时间标志. 0-立即生效  1-实时不生效，月末生效 2-实时生效，月末不生效  
        int 			  		m_effectItem;    //帐前调整作用帐目
        int             m_referItem;     //帐前调整参考帐目
        int         		m_addupItem;  	 //帐前调整累计帐目 
        T_MEMBER_TYPE   m_roleCode;      //作用角色编码  -1:关联资费(虚拟用户下所有用户) 0:普通资费(个人用户)，关联资费(虚拟用户) >0:具体角色编码 
        RateTpInfo      m_rateTpInfo;    //资费政策信息
        T_RULE_ID     	m_ruleId; 			//条件ID  add 2009-9-9 20:04:38
        CondBase        *m_condRule; 		 //条件指针	add  2009-9-9 20:04:39
        T_RULE_ID       m_specDealTag;   //特殊处理标记  

 	  friend ostream & operator <<(ostream &os ,const Discount &po)
    {
    	if (po.m_idTypeCode==DISCNT_TYPE)
          os  <<  " feepolicy_ins_id : " << po.m_discountId << " , "
              <<  " feepolicy_comp_id : " << po.m_feepolicyCompId << " , "
              <<  " feepolicy_id : "  << po.m_rateTpInfo.m_rateTpId << " , "
              << "  event_priority : " << po.m_priority;
      else 
          os  << "  adjustInsId : " << po.m_adjustInsId << " , "
              << "  event_priority : " << po.m_priority;
              
        return os;
    }      
        
};

//定义用户的帐户帐单格式
class UserAccountBill
{
	public:
    T_ACCT_ID               m_acctId;
    int                     m_userPriority;
    char                    m_defaultTag;
    vector<DetailItemFee>   m_detailItemFeeVector;
  public:
  	 friend int operator<(const UserAccountBill &left,const UserAccountBill& right)
    {
      if(left.m_userPriority == right.m_userPriority)
      {
      	return (left.m_acctId < right.m_acctId);
      }
      
      return (left.m_userPriority < right.m_userPriority);
    }
    friend int operator==(const UserAccountBill &left,const UserAccountBill& right);
    void operator=(const UserAccountBill& right);
    
    
    friend ostream & operator <<(ostream &os ,const UserAccountBill &po)
    {
         os << " m_acctId : " << po.m_acctId << " , "
      			 << " m_userPriority : " << po.m_userPriority << " , "
             << " m_defaultTag : " << po.m_defaultTag << " , "<<endl;

            for(vector<DetailItemFee>::const_iterator t_it=po.m_detailItemFeeVector.begin();
                 t_it!=po.m_detailItemFeeVector.end();t_it++)
                 os<<*t_it<<endl;

        return os;

    }

};


//系统资费
class  sysTariff
{
public:
	T_TP_ID     m_tpId;         //资费标识
	T_DATE_TIME m_beginTime;    //开始时间
	T_DATE_TIME m_endTime;      //结束时间
  T_ATTRI_VAL m_homeAreaCode; //归属地市
};

//成员信息
typedef struct MemberInfo
{
	public:
    T_MEMBER_TYPE   m_memberRole;     // 成员角色
    T_PRI           m_orderNo;        // 成员优惠优先级   
}T_MEMBERINFO_TYPE;



//固定费计算明细日志，对应以前的OL目录下L文件内容
class UserRentPriceLog
{
   public:  
   	UserRentPriceLog(){}
   	~UserRentPriceLog(){}
   	
   	T_USER_ID m_userid;    //用户
   	int  m_item;           //帐目
   	int  m_subCycId;       //子帐期Id
   	int  m_svcId;          //服务
   	char m_svcStateCode;   //状态
   	int  m_policyCompId;   //资费构成Id
   	long  m_subFee;        //子帐期单元费用
   	
   	friend ostream& operator << (ostream& os, const UserRentPriceLog& userRentPriceLog)
      	{
           os << userRentPriceLog.m_userid       << ","
              << userRentPriceLog.m_item         << ","               
              << userRentPriceLog.m_subCycId     << ","  
              << userRentPriceLog.m_svcId        << ","  
              << userRentPriceLog.m_svcStateCode << ","  
              << userRentPriceLog.m_policyCompId << ","  
              << userRentPriceLog.m_subFee; 
           
           return os;
      	}  
};

//add by rentfeeItem 2011-3-23
class RentOutBillCdr
{
    public:
    //##ModelId=4BE3D2C40309
    RentBillInfo m_rentBillInfo;
    //##ModelId=4BE3D34000B1
    bool m_isOutPut;
    //##ModelId=4BE3D34D0098
    PString m_rentCdr;

  friend bool operator<(const RentOutBillCdr &left,const RentOutBillCdr& right)
    {
       return left.m_rentBillInfo < right.m_rentBillInfo;
    }	
};
//end 2011-3-23

class FDateTime
{
public:
  
  long  fullFillToMonthEnd(const int  &r_yearMonth)
  {     	
    if(r_yearMonth<190001||r_yearMonth>300001)
    {
#ifdef _DEBUG_
    	cout<<"yearMonth is error!"<<endl;
#endif
    	return r_yearMonth;
    }
    
	 if (r_yearMonth%100>12)
    {
#ifdef _DEBUG_
	  cout<<"r_yearMonth is error!"<<endl;
#endif
	  return r_yearMonth;
		}

  	int l_year  = r_yearMonth/100;
  	int l_month = r_yearMonth%100;
  	
  	int l_monthDay = getMonthDay(l_year,l_month);
  	 
  	sprintf(m_strBuff,"%d%02d%d235959",l_year,l_month,l_monthDay);
    
    m_strBuff[14]='\0';

#ifdef _DEBUG_
	 cout<<m_strBuff<<endl;
#endif  	
  	return  atol(m_strBuff);
  }
  
  long  fullFillToMonthBegin(const int  &r_yearMonth)
  {
      if(r_yearMonth<190001||r_yearMonth>300001)
    {
#ifdef _DEBUG_    	
    	cout<<"yearMonth is error!"<<endl;
#endif
    	return r_yearMonth;
    }
     
    if (r_yearMonth%100>12)
    {
#ifdef _DEBUG_     	
	  cout<<"r_yearMonth is error!"<<endl;
#endif
	  return r_yearMonth;
	}

    sprintf(m_strBuff,"%d",r_yearMonth);
    m_strBuff[6]='\0';
    strncat(m_strBuff,"01000000",8);
    m_strBuff[14]='\0'; 

#ifdef _DEBUG_      
   cout<<m_strBuff<<endl;
#endif
   
   return atol(m_strBuff); 
  }
  
  bool isLeapYear(const int &r_year)
	{   
   	if((r_year % 4 == 0 && r_year % 100 != 0 )|| r_year % 400 == 0) 
     return true;
         
  	return false;
	}
	
	int getMonthDay(const int &r_year,const int &r_month)
	{

		switch (r_month)
		{
		 case  1 :
		 case  3 :
		 case  5 :
		 case  7 :
		 case  8 :
		 case  10 :
		 case  12 :
		    return 31;
		 case  4 :
		 case  6 :
		 case  9 :
		 case  11 : 
		    return 30;
		 case  2 :
		   if (isLeapYear(r_year))
		   return 29;
		  else
		   return 28; 
		}
		 
		 return 0;
		
	}
  
  //把秒转换成HHMMSS， sec 秒 ，sTime 小时分钟秒HHMMSS
  static  void secToTimeString(int sec, string& sTime);
 
  
private:
  char m_strBuff[20];

};

void DEBUG_PRINT(const char *fmt,...);
int MinusCycle(const int &left_cycle ,const int &right_cycle);


struct T_VFILE
{
	CycParam m_cycParam;
	int m_errno;
};

//begin add by taijq 2009-12-31
struct MutexTp
{
	T_TP_ID m_lowpriorId;
	T_TP_ID m_highpriorId;
	char m_mutexType;
};
//end add by taijq 2009-12-31
//包月费新增
//以下定义文件操作宏
const String<2> MONFEE_SOURCE_TYPE = "61";	//数据源      
const String<4> MONFEE_BIZ_TYPE = "6";     //话单业务类型
const String<100> MONFEE_FID = "";	//过滤信息
const char MONFEE_RR_FLAG = '0';	//排重标记

const char     FIELD_SPLIT = '\t';
const char     FIELD_SPLIT_OUT = ',';
//输入文件类型
const char     FILE_TYPE_IN_A = 'A';    // ADC业务,MAS业务订购关系文件 (TF_F_USER_GRPMBMP)
const char     FILE_TYPE_IN_B = 'B';    // ADC业务订购关系             (TF_F_USER_GRPMBMP_SUB)
const char     FILE_TYPE_IN_M = 'M';    // SP梦网信息订购关系文件      (TF_F_USER_MBMP_SUB)
const char     FILE_TYPE_IN_H = 'H';    // 行业应用,MAS业务订购关系文件(TF_F_USER_SIGN)
const char     FILE_TYPE_IN_U = 'U';    // 重出账文件
const char     FILE_TYPE_IN_E = 'E';    // 订购关系异常终止触发费用清零文件(TF_F_USER_SP_CLEAR)

const char     FILE_TYPE_OUT_D = 'D';   // 包月费用明细帐单文件
const char     FILE_TYPE_OUT_P = 'P';   // SP费用清单文件      
const char     FILE_TYPE_OUT_I = 'I';   // ADC费用清单文件     
const char     FILE_TYPE_OUT_B = 'B';   // 包月费用帐单文件    
const char     FILE_TYPE_OUT_C = 'C';   // 稽核文件    
const char     FILE_TYPE_OUT_N = 'N';   // 无主文件    

const String<2> SOURCE_TYPE = "61";	//数据源      
const String<4> BIZ_TYPE = "6";     //话单业务类型
const String<100> FID = "";	//过滤信息
const char RR_FLAG = '0';	//排重标记	



const int      MAX_MONTHFEE_OUTFILETYPE = 3;
//输出文件类型
//const char     WORK_FILE_D = 0;    // 包月费用明细帐单文件
//const char     WORK_FILE_P = 1;    // SP费用清单文件      
//const char     WORK_FILE_I = 2;    // ADC费用清单文件     
const char     WORK_FILE_B = 0;    // 包月费用帐单文件
const char     WORK_FILE_C = 1;    // 稽核文件
const char     WORK_FILE_N = 2;    // 无主文件

//判断单高用户使用
const int 		AVERAGEFEE          = 2000;	//平均单条使用记录金额，单位：厘

//业务状态编码
const char    NORMAL_BIZSTATE_CODE = 'A';   // 正常商用 
//定义资费信息
class MonfeeTariff
{
    public:
        MonfeeTariff() {m_feeType=INIT_FEETYPE;}
        ~MonfeeTariff() {}
                        
        bool operator < (const MonfeeTariff &tariff) const
        {
            if (m_dataTypeCode == tariff.m_dataTypeCode)
            {              
                if(m_spCode == tariff.m_spCode) 
                {
                    return (m_spCode < tariff.m_spCode);
                }
                else
                {
                    return (m_spBizCode < tariff.m_spBizCode);                    
                }
            }
            else
            {
                return (m_dataTypeCode < tariff.m_dataTypeCode);
            }        
        }
                    
    public:        
    
        char     m_dataTypeCode;    
        char     m_displayType;             
        char     m_recvObjectType;  
        char     m_billingCycle;
        char     m_newUserRecvType; 
        
        int      m_itemCode;                
        int      m_relativeCycle;   
        char     m_price[MIN_STR_LENGTH+1];                  
        int      m_judgeHalfTime;   
        char        m_halfPrice[MIN_STR_LENGTH+1];           
        int      m_validStartCycle; 
        int      m_validEndCycle;      
                
        String<4> m_bizTypeCode;
        string   m_spCode;          
        string   m_spBizCode; 
        String<14>   m_startDate;
        String<14>   m_endDate;
        
  		  char        m_judgeSilence;
  		  char        m_judgeAverage; 
        char        m_feeType;
};

//定义优惠信息
class MonfeeDiscount
{
    public:
        MonfeeDiscount() {}
        ~MonfeeDiscount() {}
                        
        bool operator < (const MonfeeDiscount &discount) const
        {
        	if (m_priority == discount.m_priority)
        	{
            	if (m_discountId == discount.m_discountId)
            	{                
            	    return (m_orderNo < discount.m_orderNo);                    
            	}
            	else
            	{
            	    return (m_discountId < discount.m_discountId);
            	}
            }
            else
            {
            	return (m_priority < discount.m_priority);
            }        
        }
                    
    public:
        T_TPOBJ_ID  m_tpObjId; // 资费实例标识
        char    m_discountType;
        char    m_discountValueType;
        char		m_discountMode;
        char    m_dataTypeCode;
        char    m_validType;        
        
        int     m_discountId;
        int     m_orderNo;
        int     m_priority;
        int     m_itemCode;
        char    m_discountValue[MIN_STR_LENGTH+1]; 
        int     m_startCycId;
        int     m_endCycId;        
        int     m_minValue;    //额定用户数最小值
        int     m_maxValue;    //额定用户数最大值
        int     m_referValue;  //超出额定数量部分的每个用户的费用。        
        
        char    m_addupMethod;//累计方式（根据业务规则添加，0：普通方式）
  		  char    m_addupType;//累计类型（0：天、1：月、2：年）
        int     m_addupValue;//累计参考值
          
        string  m_spCode;
        string  m_spBizCode;
        String<14>  m_startDate;
        String<14>  m_endDate;
};


//定义用户原始帐单结构.
class UserDetailBillMonfee
{
public:
    UserDetailBillMonfee()
    {  
        m_itemCode=0;
        m_fee=0;        
        m_disPlayFee=0;
        m_discntFee=0;
        m_discntCode=0;
        m_discntType='0'; 
        m_displayType='0';
        m_feeType='Y';
    }
    
    int operator < (const UserDetailBillMonfee &userDetailBill) const
    {
        if (m_feeType < userDetailBill.m_feeType) return true;
        if (m_feeType > userDetailBill.m_feeType) return false;
        if (m_itemCode < userDetailBill.m_itemCode) return true;
        if (m_itemCode > userDetailBill.m_itemCode) return false;
        return true;
    }    
    char m_discntType; 
    char m_displayType;    
    
    int m_itemCode;
    int m_fee;  
    int m_discntFee;  
    int m_disPlayFee;
    int m_discntCode;
    char m_feeType;
};



class UserSubInfo
{    
public:        
    UserSubInfo()
    {
    	  m_userId="";
        m_spBizCode="";
        m_spServCode="";
        m_bizTypeCode="";
        m_spCode="";
        m_sourceType="";
        m_ecSerialNumber="";
        m_startDate="";             
        m_endDate="";               
        m_bizStateCode='A';
        m_billFlag='0';
        m_ecUserId=0;
        
        m_userDetailBill.clear();
    }
    
    ~UserSubInfo()
    {
        m_userDetailBill.clear();
    }
    
    
    UserSubInfo& operator=(const UserSubInfo& rhs)
    {
      if (this != &rhs) {
      	m_userId = rhs.m_userId;
        m_spBizCode = rhs.m_spBizCode;
        m_spServCode = rhs.m_spServCode;
        m_spCode = rhs.m_spCode;
        m_bizTypeCode = rhs.m_bizTypeCode;
        m_sourceType = rhs.m_sourceType;
        m_ecSerialNumber = rhs.m_ecSerialNumber;
        m_startDate = rhs.m_startDate;
        m_endDate = rhs.m_endDate; 
        m_bizStateCode = rhs.m_bizStateCode; 
        m_billFlag = rhs.m_billFlag; 
        m_ecUserId = rhs.m_ecUserId; 
        
        m_userDetailBill.clear();
        for(int i=0;i<rhs.m_userDetailBill.size();i++)
        {
            m_userDetailBill.push_back(rhs.m_userDetailBill[i]);
        }                
      }
        
      return *this;
    }   
    T_SERVER_ID  m_userId;   //用户表示 
    String<18>  m_spBizCode; //SP业务代码
    String<18>  m_spCode;      //SP(SI)编码        
    String<18>  m_spServCode;//SP服务代码        
    String<3>   m_sourceType;    //定购渠道 
    String<14>  m_startDate;//最小开始时间A状态       
    String<14>  m_endDate;//最大结束时间A状态
    char    m_bizStateCode;
    char    m_billFlag;
    String<20>  m_ecSerialNumber;//集团客户标识 
    T_SERVER_ID			m_ecUserId;			//集团用户标识 
    String<14>	m_priorTime;     //历史首次定购时间，对M文件有效
		String<4>   m_bizTypeCode;//业务类型代码    
    String<14>  m_lastTime;      //最后一次定购时间，对M文件有效
		
    vector<UserDetailBillMonfee> m_userDetailBill;
};

typedef vector<UserSubInfo>::iterator UserSubInfoIter;
typedef vector<UserSubInfoIter> UserSubInfoIterVector;

//用户Sp订购关系扩展字段信息
class UserSubInfoExtra
{
	public:
	UserSubInfoExtra()
    {
    	  m_isValid=0;
        m_subDateSign=0;
        m_subStopSign=0;
        m_subUseDayNum=0;
        m_isRepeat=0;
        m_isAllPause=0;
        m_cancelTime="";
        m_startTime="";             
        m_endTime="";               
        m_laststartTime="";
        m_lastendTime="";
    }
    
    ~UserSubInfoExtra()
    {
    }	
    
   UserSubInfoExtra& operator=(const UserSubInfoExtra& rhs)
    {
      if (this != &rhs) 
      	{
          m_isValid        = rhs.m_isValid      ;       
          m_subDateSign    = rhs.m_subDateSign  ;   
          m_subStopSign    = rhs.m_subStopSign  ;   
          m_subUseDayNum   = rhs.m_subUseDayNum ;  
          m_isRepeat       = rhs.m_isRepeat     ;      
          m_isAllPause     = rhs.m_isAllPause   ;    
          m_cancelTime     = rhs.m_cancelTime   ;    
          m_startTime      = rhs.m_startTime    ;     
          m_endTime        = rhs.m_endTime      ;       
          m_laststartTime  = rhs.m_laststartTime; 
          m_lastendTime    = rhs.m_lastendTime  ;                 
       }
       return *this;
    }
	int m_isValid;//是否存在有效的订购记录
	int m_subDateSign;//订购时间,1:往月订购;2:本月上半月订购;3:本月下半月订购 
  int m_subStopSign;//终止时间,1:当月上半月终止;2:当月下半月终止;3:非当月终止
  int m_subUseDayNum;//当月正常使用天数
  int m_isRepeat;//是否重复订购,1:当月重复订购
  String<14> m_cancelTime;//当月订购后的第一次终止的开始时间（暂停也算）（状态‘E’，‘P’）
  int m_isAllPause;//是否全月暂停，1：当月全月暂停
  String<14> m_startTime;//当月第一次订购开始时间（状态‘A’，‘N’）
  String<14> m_endTime;//当月第一次订购结束时间（状态‘A’，‘N’）
  String<14> m_laststartTime;//当月最后一次订购开始时间（状态‘A’，‘N’）
  String<14> m_lastendTime;//当月最后一次订购结束时间（状态‘A’，‘N’）	
};

class CdrBill
{
    public:
        CdrBill()
        {
        	  m_userId = "";
            m_serialNumber = "";
            m_spServCode = "";
            m_spCode = "";
            m_spBizCode = "";
            m_bizTypeCode = "";
            m_ecSerialNumber = "";
            m_sourceType = "";
            m_fee = 0;            
            m_discntFee = 0;
            m_validTag = 0;
        }
                    
        ~CdrBill() {;}
        
        CdrBill (const CdrBill& cdrFeeBill)
        {
        	  m_userId       = cdrFeeBill.m_userId;
            m_serialNumber = cdrFeeBill.m_serialNumber;
            m_ecSerialNumber = cdrFeeBill.m_ecSerialNumber;
            m_spServCode = cdrFeeBill.m_spServCode;            
            m_spBizCode = cdrFeeBill.m_spBizCode;
            m_bizTypeCode = cdrFeeBill.m_bizTypeCode;
            m_spCode = cdrFeeBill.m_spCode;                        
            m_fee = cdrFeeBill.m_fee;            
            m_discntFee = cdrFeeBill.m_discntFee;            
            m_validTag= cdrFeeBill.m_validTag;
            m_sourceType = cdrFeeBill.m_sourceType;
        }    
    
    public:
        CdrBill& operator =(const CdrBill& cdrFeeBill)
        {
            if (this != &cdrFeeBill)
            {
            	  m_userId       = cdrFeeBill.m_userId;
                m_serialNumber = cdrFeeBill.m_serialNumber;
                m_ecSerialNumber = cdrFeeBill.m_ecSerialNumber;
                m_spServCode = cdrFeeBill.m_spServCode;
                m_spBizCode = cdrFeeBill.m_spBizCode;
                m_bizTypeCode = cdrFeeBill.m_bizTypeCode;
                m_spCode = cdrFeeBill.m_spCode;                                
                m_fee = cdrFeeBill.m_fee;                
                m_discntFee = cdrFeeBill.m_discntFee;            
                m_validTag= cdrFeeBill.m_validTag;
                m_sourceType = cdrFeeBill.m_sourceType;
            }
            return *this;
        }         
        
        bool operator==(const CdrBill& right)
        {
            return ( (m_serialNumber == right.m_serialNumber) &&
                     (m_spCode == right.m_spCode) &&
                     (m_spBizCode == right.m_spBizCode) );
        }
        
        bool operator<(const CdrBill& right)
        {
            if (m_serialNumber == right.m_serialNumber)
            {
                if(m_spCode == right.m_spCode)
                {
                    return (m_spBizCode < right.m_spBizCode);
                }
                else
                {
                    return (m_spCode < right.m_spCode);
                }
            }
            else
            {
                return (m_serialNumber < right.m_serialNumber);
            }            
        }        
        
    public:
    	  T_SERVER_ID  m_userId;   //用户表示 
        unsigned char      m_validTag;                
        int                m_fee;
        int                m_discntFee;
        
        String<20>         m_ecSerialNumber;
        String<20>         m_serialNumber;
        String<2>          m_sourceType;
        String<4>          m_bizTypeCode;
        String<18>         m_spCode;
        String<18>         m_spBizCode;        
        String<18>         m_spServCode;           
};

class OutBill
{
    public:
        OutBill()
        {
        	  m_userId="";
            m_serialNumber = "";
            m_spServCode = "";
            m_spCode = "";
            m_spBizCode = "";
            m_bizTypeCode = "";
            m_ecSerialNumber = "";
            m_sourceType = "";
            m_itemCode = 0;
            m_fee = 0;            
            m_discntFee = 0;
            m_validTag = 0;
            m_displayType = '0';
            m_feeType = 'Y';
        }
                    
        ~OutBill() {;}
        
        OutBill (const OutBill& outBill)
        {
        	  m_userId  = outBill.m_userId;
            m_serialNumber = outBill.m_serialNumber;
            m_ecSerialNumber = outBill.m_ecSerialNumber;
            m_spServCode = outBill.m_spServCode;            
            m_spBizCode = outBill.m_spBizCode;
            m_bizTypeCode = outBill.m_bizTypeCode;
            m_spCode = outBill.m_spCode;    
            m_itemCode = outBill.m_itemCode;                    
            m_fee = outBill.m_fee;            
            m_discntFee = outBill.m_discntFee;            
            m_validTag= outBill.m_validTag;
            m_sourceType = outBill.m_sourceType;
            m_displayType = outBill.m_displayType;
            m_feeType = outBill.m_feeType;
        }    
    
    public:
    	   bool isZero()const{return (m_fee==0 &&m_discntFee==0); }
        OutBill& operator =(const OutBill& outBill)
        {
            if (this != &outBill)
            {
            	  m_userId = outBill.m_userId;
                m_serialNumber = outBill.m_serialNumber;
                m_ecSerialNumber = outBill.m_ecSerialNumber;
                m_spServCode = outBill.m_spServCode;
                m_spBizCode = outBill.m_spBizCode;
                m_bizTypeCode = outBill.m_bizTypeCode;
                m_spCode = outBill.m_spCode;     
            		m_itemCode = outBill.m_itemCode;                             
                m_fee = outBill.m_fee;                
                m_discntFee = outBill.m_discntFee;            
                m_validTag= outBill.m_validTag;
                m_sourceType = outBill.m_sourceType;
                m_displayType = outBill.m_displayType;
                m_feeType = outBill.m_feeType;
            }
            return *this;
        }         
        
     OutBill& operator +=(const OutBill& outBill)
        {                            
           m_fee += outBill.m_fee;                
           m_discntFee += outBill.m_discntFee;            
           return *this;
        } 
        
        bool operator==(const OutBill& right)
        {
            return ( (m_serialNumber == right.m_serialNumber) &&
                     (m_spCode == right.m_spCode) &&
  
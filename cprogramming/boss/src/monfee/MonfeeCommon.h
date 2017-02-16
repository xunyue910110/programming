/**
* Copyright (c) 2001,linkage
* All rights reserved.
*
* 文件名称：MonfeeCommon.h
* 文件标识：
* 摘    要：完成包月费计算中通用的数据、结构、类的定义
*
* 当前版本：1.0
* 作    者：cheny
* 完成日期：2007年3月27日
*
* 取代版本：
* 原作者  ：
* 完成日期：
*/

#ifndef MonfeeCommon_H
#define MonfeeCommon_H

//#include "base/Types.h"
#include "FieldsXX.h"
#include "MdbInfo.h"

#include <vector>
#include <map>
#include "string.h"

#if defined(STDIOSTREAM_DISABLE)
#include <fstream.h>
#include <strstream.h>
#include <iostream.h>
#else
#include <fstream>
#include <strstream>
#include <iostream>
#endif

typedef T_SERVER_ID USERID;
typedef T_SERVER_ID CUSTID;

const USERID INVALID_USERID = 0;

//以下定义文件操作宏

const char     FIELD_SPLIT = '\t';
const char     FIELD_SPLIT_OUT = ',';
//输入文件类型
const char     FILE_TYPE_IN_A = 'A';    // ADC业务,MAS业务订购关系文件 (TF_F_USER_GRPMBMP)
const char     FILE_TYPE_IN_B = 'B';    // ADC业务订购关系             (TF_F_USER_GRPMBMP_SUB)
const char     FILE_TYPE_IN_M = 'M';    // SP梦网信息订购关系文件      (TF_F_USER_MBMP_SUB)
const char     FILE_TYPE_IN_H = 'H';    // 行业应用,MAS业务订购关系文件(TF_F_USER_SIGN)
const char     FILE_TYPE_IN_U = 'U';    // 重出账文件

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

const int EMONTHFEEACCOUNTBASE          = 221000;

const int ERROR_OPEN_FILE           = EMONTHFEEACCOUNTBASE + 1; //文件打开错误 
const int ERROR_LINK_UNLINKE        = EMONTHFEEACCOUNTBASE + 2; //文件搬迁错误        
const int ERROR_INVALID_FILE        = EMONTHFEEACCOUNTBASE + 3; //帐单文件无效

const int ERROR_PARAM_FILE          = EMONTHFEEACCOUNTBASE + 4; //文件系统参数错误 
const int ERROR_PARAM_INITIALIZE    = EMONTHFEEACCOUNTBASE + 5; //资料初始化错误
const int ERROR_PARAM_UPDATING      = EMONTHFEEACCOUNTBASE + 6; //资料更新错误
const int ERROR_PARAM_DATABASE      = EMONTHFEEACCOUNTBASE + 7; //数据库系统参数错误

const int ERROR_BILL_RECOVERY_OK    = EMONTHFEEACCOUNTBASE + 8; //帐单恢复错误
const int ERROR_BILL_NO_LOG_FILE    = EMONTHFEEACCOUNTBASE + 9; //帐单日志文件错误
const int ERROR_BILL_FILE           = EMONTHFEEACCOUNTBASE + 10;//帐单文件错误
const int ERROR_BILL_INITIALIZE     = EMONTHFEEACCOUNTBASE + 11;//帐单初始化错误
const int ERROR_BILL_SYNC           = EMONTHFEEACCOUNTBASE + 12;//帐单同步错误
const int ERROR_BILL_END            = EMONTHFEEACCOUNTBASE + 13;//帐单终止错误
  

//enum ACYCTYPE
//{
//    DAY_CYCTYPE_CODE         = '1',   // 日收取周期类型
//    WEEK_CYCTYPE_CODE,              // 周收取周期类型
//    TENDAY_CYCTYPE_CODE,            // 旬收取周期类型
//    HALFMONTH_CYCTYPE_CODE,         // 半月收取周期类型
//    MONTH_CYCTYPE_CODE,             // 月收取周期类型
//    TWOMONTH_CYCTYPE_CODE,          // 双月收取周期类型
//    QUARTER_CYCTYPE_CODE,           // 季度收取周期类型
//    HALFYEAR_CYCTYPE_CODE,          // 半年收取周期类型
//    YEAR_CYCTYPE_CODE               // 年收取周期类型
//};

//以下定义包月费计算涉及所有数据结构

/*! \struct MonthFeeAccountConfig
*   \brief   This is a struct record configinfo of fixed fee computer.
*/
struct MonfeeConfig
{
   string   m_hostName;
   int      m_interval;
   //add by yuxk for MT
   int      m_unitfee_M;
   int      m_unitfee_T;
   string   m_product_iphone;
   //add by yuxk for MT
   
   string   m_userName;
   string   m_passWord;
   string   m_servName;

   T_DATABASETYPE   m_mdbType;
   string   m_mdbServer;
   string   m_mdbHostName;
   int			m_mdbPort;
   string   m_mdbUserName;
   string   m_mdbPassWord;

   string   m_inputFilePath;
   string   m_invalidFilePath;
   string   m_workFilePath;
   string   m_outputFilePath_D;
   string   m_outputFilePath_P;
   string   m_outputFilePath_I;
   
   // Add by Malone 2007-7-19 10:08 start
   string   m_outputBakupFilePath_P;
   string   m_outputBakupFilePath_I;
   // Add by Malone 2007-7-19 10:08 end
   
   string   m_outputFilePath_B;
   string   m_outputFilePath_C;
   string   m_outputFilePath_N; 
   string   m_billLogFilePath;  
   string   m_totalBillFilePath;
   
   int      m_autoOutputBillUserNum;
   int      m_autoCommitBillUserNum;

   int      m_sleepTime;
   string   m_fileName;
   string   m_cdrFileName;
   char     m_sortFlag;
   int        m_channelNo;
   
   int 			m_dbNo;
   // Added by Malone 2009-11-1 17:53 start
   string   m_eparchyCode;
   // Added by Malone 2009-11-1 17:54 end
};

////定义帐期信息结构
//struct CycPara
//{
//   int      m_cycId;
//   String<14>   m_cycStartTime;
//   String<14>   m_cycEndTime;
//   String<14>   m_discountJudgeTime;
//   char     m_useTag;
//   char     m_addTag;
//};


///*! \class SubAcycPara
//*   \brief   This is a class which describe the SubAcycPara.
//*/
//class SubCycPara
//{
//    public:
//        bool operator < (const SubCycPara &subCycPara)   const
//        {
//           return m_cycTypeCode < subCycPara.m_cycTypeCode;
//        }
//
//        char* dump(char* buf) const
//        {
//            buf += sprintf(buf, "<<<-------- SubCycPara -------->>>\n");
//            buf += sprintf(buf, "subCycId : %d\n", m_subCycId);
//            buf += sprintf(buf, "cycTypeCode : %d\n", m_cycTypeCode);
//            buf += sprintf(buf, "relativeCyc : %d\n", m_relativeCyc);
//            buf += sprintf(buf, "sequenceNo : %d\n", m_sequenceNo);
//            buf += sprintf(buf, "dayNum : %d\n", m_dayNum);
//            buf += sprintf(buf, "startTime : %s\n", m_startTime.c_str());
//            buf += sprintf(buf, "endTime : %s\n", m_endTime.c_str());
//
//            return buf;
//        }
//
//    public:
//        int      m_subCycId;
//        int      m_cycTypeCode;
//        int      m_relativeCyc;
//        int      m_sequenceNo;
//        int      m_dayNum;
//        String<14>   m_startTime;
//        String<14>   m_endTime;
//};

//定义系统参数信息结构
struct SysParamInfo
{
   string   m_sysParamCode;
   int      m_channelNo;
   int      m_valueN;
   string   m_valueS;
   String<14>   m_valueD;
   int      m_preValueN1;
   int      m_preValueN2;
   int      m_preValueN3;
   string   m_preValueS1;
   string   m_preValueS2;
   string   m_preValueS3;
   String<14>   m_preValueD1;
   String<14>   m_preValueD2;
   String<14>   m_preValueD3;
   char     m_useTag; 
};

//定义资费信息
class MonfeeTariff
{
    public:
        MonfeeTariff() {}
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
        int      m_price;                   
        int      m_judgeHalfTime;   
        int      m_halfPrice;               
        int      m_validStartCycle; 
        int      m_validEndCycle;      
                
        String<4> m_bizTypeCode;
        string   m_spCode;          
        string   m_spBizCode; 
        String<14>   m_startDate;
        String<14>   m_endDate;
        
        // Add by Malone 2008-1-26 start
  		  char        m_judgeSilence;
  		  char        m_judgeAverage; 

        // Add by Malone 2008-1-26 end
    		  char        m_feeType;// add by yuxk for MT      
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
        char    m_discountType;
        char    m_discountValueType;
        char		m_discountMode;
        char    m_dataTypeCode;
        char    m_validType;        
        
        int     m_discountId;
        int     m_orderNo;
        int     m_priority;
        int     m_itemCode;
        int     m_discountValue;
        int     m_startCycId;
        int     m_endCycId;        
        int     m_minValue;    //额定用户数最小值
        int     m_maxValue;    //额定用户数最大值
        int     m_referValue;  //超出额定数量部分的每个用户的费用。        
        
        string  m_spCode;
        string  m_spBizCode;
        String<14>  m_startDate;
        String<14>  m_endDate;
};

////定义优惠信息
//struct IdDiscount
//{
//    int     m_discountId;
//    String<14>  m_startDate;
//    String<14>  m_endDate;
//    string  m_eparchyCode;
//};

//定义用户原始帐单结构.
class UserDetailBill
{
public:
    UserDetailBill()
    {    
        m_itemCode=0;
        m_fee=0;        
        m_disPlayFee=0;
        m_discntCode=0;
        m_discntType='0'; 
        m_displayType='0';
        m_feeType='0';	//add by yuxk for MT	
    }
    
    bool operator < (const UserDetailBill &userDetailBill) const
    {
        if (m_itemCode == userDetailBill.m_itemCode)
        {
            return (m_discntCode < userDetailBill.m_discntCode);
        }
        else
        {
            return (m_itemCode < userDetailBill.m_itemCode);
        }        
    }    
     
    char m_discntType; 
    char m_displayType;   
    char m_feeType;//add by yuxk for MT	
    
    int m_itemCode;
    int m_fee;    
    int m_disPlayFee;
    int m_discntCode;
    
};

class DetailBill
{
public:
    DetailBill()
    {    
        m_itemCode=0;
        m_fee=0;                
    }
    
    bool operator < (const DetailBill &detailBill) const
    {        
        return (m_itemCode < detailBill.m_itemCode);                
    }
    
    bool operator == (const DetailBill &detailBill) const
    {        
        return (m_itemCode == detailBill.m_itemCode);                
    }
    
    int m_itemCode;
    int m_fee;        
    int m_discntFee;        
};

class UserSubInfo
{    
public:        
    UserSubInfo()
    {
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
    
    
    String<18>  m_spBizCode; //SP业务代码
    String<18>  m_spCode;      //SP(SI)编码        
    String<18>  m_spServCode;//SP服务代码        
    String<3>   m_sourceType;    //定购渠道 
    String<14>  m_startDate;       
    String<14>  m_endDate; 
    char    m_bizStateCode;
    char    m_billFlag;
    String<20>  m_ecSerialNumber;//集团客户标识 
    USERID			m_ecUserId;			//集团用户标识 
    String<14>	m_priorTime;     //历史首次定购时间，对M文件有效
		String<4>   m_bizTypeCode;//业务类型代码    
    String<14>  m_lastTime;      //最后一次定购时间，对M文件有效
		
    vector<UserDetailBill> m_userDetailBill;
};

typedef vector<UserSubInfo>::iterator UserSubInfoIter;
typedef vector<UserSubInfoIter> UserSubInfoIterVector;

////定义用户信息
//struct UserInfo
//{
//    USERID      m_userId;
//    CUSTID      m_custId;
//    string      m_brandCode;
//    string      m_eparchyCode;
//    string      m_cityCode;
//    string      m_userStateCodeSet;
//    char        m_userTypeCode;
//    char        m_accountTag;
//    char        m_removeTag;
//    String<14>      m_openDate;
//    String<14>      m_preDestroyTime;
//    String<14>      m_destroyTime;
//    String<14>      m_lastStopTime;
//    int         m_scoreValue;
//    int         m_creditValue;
//    String<20>  m_serialNumber;
//};

class CdrBill
{
    public:
        CdrBill()
        {
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
            m_feeType = '0';//add by yuxk for MT				
        }
                    
        ~OutBill() {;}
        
        OutBill (const OutBill& outBill)
        {
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
            m_feeType = outBill.m_feeType;//add by yuxk for MT				
        }    
    
    public:
        OutBill& operator =(const OutBill& outBill)
        {
            if (this != &outBill)
            {
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
                m_feeType = outBill.m_feeType;//add by yuxk for MT							
            }
            return *this;
        }         
        
        bool operator==(const OutBill& right)
        {
            return ( (m_serialNumber == right.m_serialNumber) &&
                     (m_spCode == right.m_spCode) &&
                     (m_spBizCode == right.m_spBizCode) &&
                     (m_itemCode == right.m_itemCode) );
        }
        
        bool operator<(const OutBill& right)
        {
            if (m_serialNumber == right.m_serialNumber)
            {
                if(m_spCode == right.m_spCode)
                {
                    if(m_spBizCode == right.m_spBizCode)
                    {
                    	return (m_itemCode < right.m_itemCode);
                    }
                    else
                    {
                    	return (m_spBizCode < right.m_spBizCode);
                    }
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
        unsigned char      m_validTag;                
        char             m_feeType;   //add by yuxk for MT	  
        int                m_fee;
        int                m_discntFee;
        int								 m_itemCode;
        char        			 m_displayType;
        String<20>         m_ecSerialNumber;
        String<20>         m_serialNumber;
        String<2>          m_sourceType;
        String<4>          m_bizTypeCode;
        String<18>         m_spCode;
        String<18>         m_spBizCode;        
        String<18>         m_spServCode;           
};

//struct IdInfo
//{
//    Number m_id;
//    char   m_idTypeCode; //0:用户标识 1:帐户标识 2:用户群标识 3:帐户群标识    
//    string m_brandCode;
//    string m_eparchyCode;
//    string m_cityCode;
//    char   m_accountTag;
//    char   m_removeTag;  
//    String<14> m_preDestroyTime;
//    String<14> m_destroyTime;//zhaogc add 20070705
//    String<20> m_serialNumber;
//};

//struct UserSVCState
//{
//    T_SERVER_ID  m_userId;         //用户标识
//    int          m_serviceId;      //服务标识
//    char         m_mainTag;        //主服务标志	
//    char         m_servStateCode;  //服务状态编码      	      
//    T_DATE_TIME  m_startDate;      //生效时间             
//    T_DATE_TIME  m_endDate;        //失效时间 
//};

struct IdErrorInfo
{
    string  m_errorType;
    int     m_fValue;
    int     m_sValue;
};

//定义帐务处理核查信息结构
struct AccountCheckInfo
{
    string  m_checkContentCode;
    int     m_itemCode;
    int     m_factValue;
    int     m_minValue;
    int     m_maxValue;
    string  m_preValueD;
};

//定义输出文件信息结构
struct OutputFileInfo
{
    string   m_workFileName;
    ofstream m_workOfStream;
    string   m_outputFileFullName;
    string   m_outputFileName;
    
    // Add by Malone 2007-7-19 10:42 start
    string   m_outputBakupFileName;
    // Add by Malone 2007-7-19 10:42 end
    
    int      m_idNum;
    int      m_recordNum;
    int64_t  m_feeSum;
    char     m_fileStat;
};

//定义帐务处理累计信息结构
struct AccountAddupInfo
{
    int     m_idNum;
    int     m_errorIdNum;
    int     m_oweIdNum;
    int     m_recordNum;
    int     m_errorRecordNum;
    int     m_feeSum;
    int     m_afeeSum;
    int     m_bfeeSum;    
    int     m_oweFeeSum;
}; 

//定义帐务处理日志结构
struct AccountLogInfo
{
    int      m_idNum;
    int      m_errorIdNum;
    int      m_recordNum;
    int      m_errorRecordNum;
    int64_t  m_fee;
    int64_t  m_filterFee;
};

//定义SP处罚规则
struct SpPunish
{
    String<18>         m_spCode;        
    String<18>         m_spBizCode;        
    String<2>          m_punishType;
    String<14>             m_beginTime;
    String<14>             m_endTime;
};

//定义SP业务关键字
class SpSvcKey
{
public:
    String<18>         m_spCode;        
    String<18>         m_spBizCode;
};

//@手机证券业务

//定义SP关联优惠明细
class SpRelaDiscntDetail
{
    public:
        SpRelaDiscntDetail()
        {
            m_spCode = "";
            m_spBizCode = "";  
            m_itemCode = 0;
            m_priority = 0;
        }
                    
        ~SpRelaDiscntDetail() {;}
        
        SpRelaDiscntDetail (const SpRelaDiscntDetail& spRelaDiscntDetail)
        {
            m_spCode = spRelaDiscntDetail.m_spCode;
            m_spBizCode = spRelaDiscntDetail.m_spBizCode;
            m_priority = spRelaDiscntDetail.m_priority;            
            m_itemCode = spRelaDiscntDetail.m_itemCode;
        }    
    
    public:
    	    
			bool operator < (const SpRelaDiscntDetail &spRelaDiscntDetail) const
	    {
	    	if (m_priority == spRelaDiscntDetail.m_priority)
	    	{
	        	return (m_itemCode < spRelaDiscntDetail.m_itemCode);
        }
        else
        {
        	return (m_priority < spRelaDiscntDetail.m_priority);
        }        
	    }
    
    public:
	    int    m_priority;
	    int    m_itemCode;
	    string m_spCode;
	    string m_spBizCode;
};
typedef vector<SpRelaDiscntDetail> SpRelaDiscntDetailVector;
typedef map<int,SpRelaDiscntDetailVector>  SpRelaDiscntMap;
typedef map<int,SpRelaDiscntDetailVector>::iterator  spRelaDiscntMapIter;

////定义SP关联优惠
//class SpRelaDiscnt
//{
//    public:
//        SpRelaDiscnt()
//        {
//            m_discntId = 0;
//            m_priority = 0; 
//            m_discntMode = ""; 
//            m_effectValueType = ""; 
//            m_effectValue = 0;
//            m_startDate = ""; 
//            m_endDate = ""; 
//            m_spRelaDiscntDetailVector.clear();
//        }
//                    
//        ~SpRelaDiscnt() 
//        {
//        	m_spRelaDiscntDetailVector.clear();
//        }
//        
//        SpRelaDiscnt (const SpRelaDiscnt& spRelaDiscnt)
//        {
//            m_discntId = spRelaDiscnt.m_discntId;
//            m_priority = spRelaDiscnt.m_priority;
//            m_discntMode = spRelaDiscnt.m_discntMode;            
//            m_effectValueType = spRelaDiscnt.m_effectValueType;
//            m_effectValue = spRelaDiscnt.m_effectValue;
//            m_startDate = spRelaDiscnt.m_startDate;
//            m_endDate = spRelaDiscnt.m_endDate;
//            
//            m_spRelaDiscntDetailVector.clear();
//		        for(int i=0;i<spRelaDiscnt.m_spRelaDiscntDetailVector.size();i++)
//		        {
//		            m_spRelaDiscntDetailVector.push_back(spRelaDiscnt.m_spRelaDiscntDetailVector[i]);
//		        }                
//        }    
//    
//    public:
//    	    
//			bool operator < (const SpRelaDiscntlaDiscntDetail &spRelaDiscnt) const
//	    {
//	    	if (m_priority == spRelaDiscnt.m_priority)
//	    	{
//	        	return (m_discntId < spRelaDiscnt.m_discntId);
//        }
//        else
//        {
//        	return (m_priority < spRelaDiscnt.m_priority);
//        }        
//	    }
//    
//    public:
//	    int			m_discntId;
//	    int			m_priority;
//	    char		m_discntMode;								//优惠类型 0 按金额几何平均 1 按账目优先级
//	    char		m_effectValueType;           //作用类型 0 金额 1 比例
//	                                            //作为比例时默认作用在优先级最高的明细上
//	                            
//	    int			m_effectValue;               //作用值
//	    String<14>	m_startDate;
//	    String<14>	m_endDate;
//	
//	    vector<SpRelaDiscntDetail> m_spRelaDiscntDetailVector;
//};

struct SpState
{
    String<2>          m_punishType;
    String<14>             m_beginTime;
    String<14>             m_endTime;
};
typedef vector<SpState> SpStateVector;
typedef map<SpSvcKey,SpStateVector>  SpPunishMap;
typedef map<SpSvcKey,SpStateVector>::iterator  SpPunishMapIter;
	

#endif   // !MonfeeCommon_H


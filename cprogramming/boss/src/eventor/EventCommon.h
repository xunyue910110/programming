/**
* Copyright (c) 2001,linkage
* All rights reserved.
*
* 文件名称：EventCommon.h
* 文件标识：
* 摘    要：
*
* 取代版本：
* 原作者  ：
* 完成日期：
*/

#ifndef EventCommon_H
#define EventCommon_H

//#include "base/Types.h"
#include "FieldsXX.h"

#include <vector>
#include <map>
#include "string.h"
#include "PM_Billing.h"
#include "base/TimeUtil.h"

#if defined(STDIOSTREAM_DISABLE)
#include <fstream.h>
#include <strstream.h>
#include <iostream.h>
#else
#include <fstream>
#include <strstream>
#include <iostream>
#endif

//以下定义文件操作宏

const char     FIELD_SPLIT = '\t';
const char     FIELD_SPLIT_OUT = ',';

const int      MAX_OUTFILE_CHANNELNO = 1000;

const int EEVENTORBASE          = 222000;

const int ERROR_OPEN_FILE           = EEVENTORBASE + 1; //文件打开错误 
const int ERROR_LINK_UNLINK            = EEVENTORBASE + 2; //文件搬迁错误        
const int ERROR_INVALID_FILE        = EEVENTORBASE + 3; //帐单文件无效

const int ERROR_PARAM_FILE          = EEVENTORBASE + 4; //文件系统参数错误 
const int ERROR_PARAM_INITIALIZE    = EEVENTORBASE + 5; //资料初始化错误
const int ERROR_PARAM_UPDATING      = EEVENTORBASE + 6; //资料更新错误
const int ERROR_PARAM_DATABASE      = EEVENTORBASE + 7; //数据库系统参数错误

const int ERROR_EVENT_DEAL                = EEVENTORBASE + 8;

const int ERROR_DB_CONNECT                = EEVENTORBASE + 9;
const int ERROR_DB_EXECSQL                = EEVENTORBASE + 10;

//added by jlxu@NeiMeng Nov18,2009 
const int ERROR_PARAM_EPARCHYCODE = EEVENTORBASE + 11;//ORACLE与MDB用户中地州编码不一致
const int ERROR_PARAM_EPARCHYCODE2 = EEVENTORBASE + 12; //ORACLE中对应地市的帐期信息未配置

//#ifdef __PROV019__  //如果是山西
const int ERROR_EVENTOR_CALLPROC  =  EEVENTORBASE + 12; //调用存储过程失败    
//add by xuf 2010-1-28 15:35:01
#define MAXTHREADCONNUM   10             //多线程执行存储过程最大线程数
#define MAXBUFFLEN      256              //字符串最大长度 
#define THREADSTACKSIZE   10*1024*1024   //子线程栈大小 
//#endif

/*! \struct EventConfig
*   \brief   This is a struct record configinfo of fixed fee computer.
*/
struct EventConfig
{
   string   m_hostName;
   int      m_interval;

   string   m_userName;
   string   m_passWord;
   string   m_servName;

   string   m_paramUserName;
   string   m_paramPassWord;
   string   m_paramServName;

   //内存数据库信息
   T_DATABASETYPE  m_mdbType;
   string          m_mdbHostName;
   string          m_mdbUserName;
   string          m_mdbPassWord;
   int             m_mdbPort;
   string          m_mdbServer;   

   string   m_outputEventFilePath;
   string   m_outputRateFilePath;	//limeng 20090814	批价输入根目录
   string   m_backupFilePath;//add by wanglu 2009-12-7 备份文件目录
   string   m_eparchyCode;	//limeng 20090817	地市编码
   string   m_runTime;	//limeng 20091225	实时模式下运行时间
   string   m_workFilePath;
   string   m_eventorType;
   string   m_netTypeCodes; //不参于出帐的网别编码
   string   m_fixNetTypeCodes; //参与出账的固网网别//Added by jlxu@neimeng Dec16,2009
   int      m_isBackupValidFile; //是否备份合法文件标志//Added for backup@neimeng Dec13,2009
   
   long      m_autoOutputFileUserNum;
   
   char     m_sortFlag;
   int      m_channelNo;
   int      m_dbNo;
   
   int      m_sleepTime;
   
};

/*改在BillingCommon.h内定义
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
      }
        
      return *this;
    }   
    
    T_USER_ID                m_userId;
    T_MSISDN                m_msisdn;
    int                     m_cycId;
    char                        m_monthEndTag;
    T_EPARCHY_CODE    m_eparchyCode;
    T_DATE_TIME            m_openDate;
};
*/

class MonfeeSpEventInfo
{    
public:        
    MonfeeSpEventInfo()
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
        m_lastTime="";
    }
    
    ~MonfeeSpEventInfo()
    {
    }
    
    
    MonfeeSpEventInfo& operator=(const MonfeeSpEventInfo& rhs)
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
      }
        
      return *this;
    }   
    T_SERVER_ID  m_userId;   //用户表示
    T_CYCLE     m_cycle;    //账期id 
    String<18>  m_spBizCode; //SP业务代码
    String<18>  m_spCode;      //SP(SI)编码        
    String<18>  m_spServCode;//SP服务代码        
    String<3>   m_sourceType;    //定购渠道 
    String<14>  m_startDate;//最小开始时间A状态       
    String<14>  m_endDate;//最大结束时间A状态
    char        m_bizStateCode;
    char        m_billFlag;
    String<20>  m_ecSerialNumber;//集团客户标识 
    T_SERVER_ID	m_ecUserId;			//集团用户标识 
    String<14>	m_priorTime;     //历史首次定购时间，对M文件有效
		String<4>   m_bizTypeCode;//业务类型代码    
    String<14>  m_lastTime;      //最后一次定购时间，对M文件有效
    char m_preString[512];//保留字串，用来存储订购关系扩展信息（分隔符|）
};

class OtpEventInfo
{   
  public:        
    OtpEventInfo()
    {
        m_userId="";
        m_msisdn="";
        m_cycId=0;
        //m_monthEndTag='0';
        m_eparchyCode="";
        m_openDate="";
        m_itemCode = 0;
        m_fee = 0;
    }
    
    ~OtpEventInfo() {}    
    
    OtpEventInfo& operator=(const OtpEventInfo& rhs)
    {
      if (this != &rhs) {
        m_userId = rhs.m_userId;
        m_msisdn = rhs.m_msisdn;
        m_cycId = rhs.m_cycId;
        //m_monthEndTag = rhs.m_monthEndTag;
        m_eparchyCode = rhs.m_eparchyCode;
        m_openDate = rhs.m_openDate;
        m_itemCode = rhs.m_itemCode;
        m_fee = rhs.m_fee;
      }
        
      return *this;
    }   
    
    T_USER_ID               m_userId;
    T_MSISDN                m_msisdn;
    int                     m_cycId;
    //char                    m_monthEndTag;
    T_EPARCHY_CODE          m_eparchyCode;
    T_DATE_TIME             m_openDate;
    int                     m_itemCode;
    long                    m_fee;
    
     
  friend ostream& operator<<(ostream& os, const OtpEventInfo& otpEventInfo)
  {
  	os<<otpEventInfo.m_userId		<<" , "<<
  		otpEventInfo.m_msisdn		<<" , "<<
  		otpEventInfo.m_cycId	    <<" , "<<
  		otpEventInfo.m_eparchyCode	<<" , "<<
  		otpEventInfo.m_openDate		<<" , "<<
  		otpEventInfo.m_itemCode		<<" , "<<
  		otpEventInfo.m_fee; 
  
  	return os;
  }
};




//定义输出文件信息结构
struct OutputFileInfo
{
    string   m_workFileName;
    ofstream m_workOfStream;
    string   m_outputFileName;
    string   m_outputPath;	//limeng add 20090820 批价输出路径
    
    // Add by Malone 2007-7-19 10:42 start
    string   m_outputBakupFileName;
    // Add by Malone 2007-7-19 10:42 end
    
    int             m_batchNo;
    int      m_idNum;
    char     m_fileStat;
};

#endif   // !EventCommon_H

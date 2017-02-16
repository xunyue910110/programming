// ##########################################
// Source file : CCdrConverstion.h
// System      : Mobile Billing System
// Version     : bss1.0
// Language    : ANSI C++
// OS Platform : HPUX
// Authors     : jianchun Shi
// E_mail      : shijc@lianchuang.com
// Create      : 20090807
// Update      : 20090807
// Copyright(C): 2009 by ShiJianChun, Linkage.
// ##########################################

#ifndef  _CDRCONVERSION_H
#define _CDRCONVERSION_H

#include "base/supdebug.h"
#include "base/Directory.h"
#include "config-all.h"
#include "dbi/dbi.h"
#include "dbi/COcciAPI.h"
#include "base/OcciUtil.h"

#include <stdlib.h>

#include <string>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/stat.h>

#include "CCdrBase.h"
#include "appfrm/ErrorHandle.h"
#include "CConvertConfig.h"
#include "systemPara.h"

USING_NAMESPACE(std);

extern ErrorMessages * const theErrorMessages;
//extern int MAX_SIZE;
double gettime();


//格式转换类——处理跳表
class CCdrConverstion : public CCdrBase
{
	typedef struct 
	{
		string 	 sSwitchName ; //交换机名		
		int     cMeterType;//跳表类型		0,区内；1，区间；2，上网
		long 	 lFullScale_area ;//区内满表值 
		long 	 lFullScale_interval ;//区间满表值  
		long   lFullScale_net ; //上网满表值	 	
	}record_Meter_CFG;

	typedef struct 
	{
		  string   sCallNumber;     //呼叫号码
			long 	   lCdrCount ;      //话单中跳次刻度
			string   sRecDate;        //话单采集日期
			char     cMeterType[2];   //跳表类型
			char 	 	 nCdrType[2];		    //跳表话单类型描述
			long     lMonthScale;     //月份刻度值
			string 	 sMonth ;         //月份值 	
			char     cFileType[2];    //文件类型，暂时不用
			string   sDes;            //描述，暂时设置为全ffff
	}RECORD_METER_COUNT;

  public:
    //##ModelId=48508FB1030D
    CCdrConverstion();   

    //##ModelId=4850C2310148
    ~CCdrConverstion();
    
		static CCdrConverstion* Create();
		bool Init();
		bool GetCDR(); //读取话单文件中的话单,文件由m_fstream打开
	//	bool OutputCDR() ;//将处理后的话单输出到输出通道
		bool ValidCDR(int iRuleChannelIndex);  //校验话单的有效性,
																					 //以iRuleChannelIndex子通道的配置为校验参数
		bool ProcessMeterCDR( const int& iRuleChannelIndex,const int& nCdrType );	//处理话单
		bool ProcessJingMeterCDR( const int& iRuleChannelIndex,const int& nCdrType ); //处理净跳次的话单
		bool ProcessLeiJiMeterCDR( const int& iRuleChannelIndex,const int& nCdrType  );//	处理累计跳次的话单
		bool ProcessFanZhuanMeterCDR( const int& iRuleChannelIndex,const int& nCdrType);//处理翻转跳次的话单
		//bool ConnectParaDB(char* sService,char* sUserName,char* ,sPassWord);		//连接参数库
		//bool ConnectBillDB(char* sService,char* sUserName,char* ,sPassWord);		//连接清单库
	//	bool GetSwitchFullMeter(char *sWitchName,vector<record_Meter_CFG> &vtRecord_Meter_Cfg); //获取交换机的所有跳表的配置信息.暂时不用
		long  GetCdrMonth(int iRuleChannelIndex);
		bool isPassCurrentCDR(int iChannelIndex ) ;                                  //判断是否跳过当前获取的话单, 从库中捞取数据
		bool isPassCurrentCDREx(int iChannelIndex ) ;                                //判断是否跳过当前获取的话单，从容器中捞取数据
		bool isOutputCdr(const string & sPhoneNo);
		bool GetAllNbr();                   //获得所有的未上移话单，存入m_vecNbr中                      
		bool GetSecitonCodeFromFile(const string & sCdrFileName); //从文件名中获取营业区。
		bool GetSectionCodeFromCallNum(const string &sCallNum,string &sSectionCode); //根据呼叫号码从库中获取对应的营业区
    //##ModelId=4850C3D401F4
    virtual bool process();   //处理主函数
    virtual bool process(string sCdrFileName,string sCdrFileNameOut, int iChannelIndex);
    bool   batchProcessCdr( int iChannelIndex,int nCdrType );
		static CCdrConverstion *stCdrCV_Instance ;
		static CCdrConverstion *getInstance();  //获取该类的静态实例
  private:   
    class VecSort
    {
    	public:
    	  bool operator()(const long &left,const long &right); //const string &left,const string &right
    };
    
    class itrEqual
    {
    	long lVal;
    	public:
    	itrEqual(const long s): lVal(s){}   
    	  bool  operator()(const long & lValue)
    	  { if (lVal == lValue) 
    	     return true;
    	    else 
    	     return false;
    	  };    	
    };
    
    Directory              m_dir; //目录管理类
	  int m_pLog;
	  long m_nActiveDate;          //活动账期
	  string m_sSectionCode_File ;      //当前处理话单文件所属的营业区
	  string m_sSectionCode_Real;  //话单文件名中的营业区对应库中的实际营业区号
	  string m_sIndbSectionCode;   //入表底库的营业区代码 ,为兼容天津特殊需求而设置
    char m_sCdrBuf[MAX_SIZE];    //正在处理的一条话单的话单内容
    string m_sCallNumber ;        //正在处理的一条话单内容中指示的呼叫号码
    fstream  m_fstream;					//打开源输入文件的流
    fstream  m_fstreamOut;      //输出文件的输入流
    fstream  m_fstreamError;		//错误话单输出流
    fstream  m_fstreamExcpt;
    string   m_sFileName ;			//处理的话单源文件
    DbInterface m_dbPara;				//连接参数库，目前暂时不用
    DbInterface m_dbBill;				//连接清单库 ，目前暂时不用
    OCCIConnect  m_cdrCon ;     //到清单库的oracle连接对象  
    OCCIConnect  m_cdrConPara ;     //到参数库的oracle连接对象   
    OCCISQL *m_pOccisql;
    
    Environment *m_pEnv;
    ConnectionPtr  m_pConn ;
    ConnectionPtr  m_pConnPara ; 
    StatementVar   *m_pStatMent;
	  vector<RECORD_METER_COUNT>  m_vecRecMeterCount;   //容器，存放话单跳次信息
	  vector<long>  m_vecNbr ;   //容器，存放所有未上移话单
    map<string,string> m_vecSecFileToDb ;//存放话单文件名指示的营业区号——计费使用的营业区号.天津专用
#endif 
   
};

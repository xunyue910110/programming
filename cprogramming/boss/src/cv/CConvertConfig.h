// ##########################################
// Source file : CCdrBase.h
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

#ifndef  _CONVERT_CONFIG_H
#define _CONVERT_CONFIG_H

#include "base/supdebug.h"
#include "base/SysParam.h"

#include "config-all.h"

#include <string>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/stat.h>

#include "systemPara.h"
#include "appfrm/ErrorHandle.h"

USING_NAMESPACE(std);

extern ErrorMessages * const theErrorMessages;
double gettime();


//## 话单处理基类
class CConvertConfig
{
  public:
    //##ModelId=48508FB1030D
    CConvertConfig();   

    //##ModelId=4850C2310148
    ~CConvertConfig();
	typedef struct _ChannelInfo
	{
		int m_iMeterCount ;  				//	该交换机跳表话单的跳表数量，用于校验
		string m_sSwitchName;				//	交换机名
		string m_sEparchy ;         //地市名或代码
		int    m_iCdrType;
		string m_sServereName_para;			//  数据库服务名	
		string m_sUserName_para;					//	连接数据库用户名
		string m_sPassword_para;					//	连接数据库密码
		string m_sServereName_bill;			//  数据库服务名	
		string m_sUserName_bill;					//	连接数据库用户名
		string m_sPassword_bill;					//	连接数据库密码
		string m_cSeprateToken;			//	话单分隔符序列
	  int    m_nCdrMonth ;        //  话单所属的月份
		bool 	 m_bCheck_cdrNo	;			//	是否校验话单
		bool   m_bBackupSrcFile;		//	是否备份源文件	
		bool   m_bBackupDesFile;		//	是否备份输出文件
		struct stuctPath						//	保存文件路径的结构体
		{
			string m_sSourcePath;
			string m_sSrcBackupPath;
			string m_sDestPath;
			string m_sDstBakupPath;
			string m_sErrPath;
		};
		struct stuctPath  m_Path;		
	}CHANNEL_INFO;
	
	
	typedef struct 
	{
		string 	sSwitchName ; //交换机名				
		long 	 lFullScale_area ;//区内满表值 
		long 	 lFullScale_interval ;//区间满表值  
		long   lFullScale_net ; //上网满表值	 	
	}Meter_CFG;
	
	//获取配置文件子通道项下 path节点的配置信息
  bool    getPath(SysParam *pSysParam,string subPath,CHANNEL_INFO &channel_Info );
  
  //获取系统配置信息
  bool		GetSystemConfigInfo(SysParam *pSysParam,string sProcName,int iChannelNo) ;  
  
  //获取参数的数据库的连接参数
	bool		GetDbLinkInfo_para(string &sSevername,string &sUser, string &sPassword)const;	
	
	//获取清单的数据库的连接参数
	bool		GetDbLinkInfo_bill(string &sSevername,string &sUser, string &sPassword)const;
	
	//获得索引标识得子通道的跳表满刻度值
	int			GetMeterCount(int iIndex)const;
	
	//获得索引标识得子通道的交换机名
	string	GetSwitchName(int iIndex)const;
	
	//获取索引标识得子通道的地市名
	string  GetEparchy(int iIndex);
	
	//获取索引标识得子通道的跳表话单类型。净跳次，累计跳次 或 翻转跳次
	int     GetCdrType( int iIndex);
	
	//获得索引标识得子通道的话单分割序列符合
	string	GetSeparateToken(int iIndex)const;
	
	//获得索引标识得子通道的源路径
	string	GetSourcePath(int iIndex)const;
	
	//获得索引标识得子通道的备份源路径
	string	GetSrcBakupPath(int iIndex)const;
	
	//获得索引标识得子通道的目标输出路径
	string	GetDestPath(int iIndex)const;
	
	//获得索引标识得子通道的备份输出路径
	string	GetDstBackupPath(int iIndex)const;	
	
	//获得索引标识子通道的错误话单输出路径
	string  GetErrCdrOutputPath(int iIndex) const;
	
	//获得索引标识得子通道的源文件是否备份，true校验份；false 不备份
	bool		IsBackupSrcFile(int iIndex)const;
	
	// 备获得索引标识得子通道的输出文件是否备份，true校验份；false 不备份
	bool		IsBackupDstFile(int iIndex)const;
	
	//获得索引标识得子通道的话单是否校验，true校验；false 不校验
	bool    IsCheckCDR(int iIndex)const;
	
	//设置活动账期的数值
	bool    SetCdrMonth() ;
	
	//获取活动账期的数值
	long  GetCdrMonth( );
	
	//获取通道下子通道的数量
	int 	 GetChannelCount();
	
	//获取特定交换机的所有满表值
	bool getSwitchFullMeter(const string & sSectionCode,long & lFullScaleVar);
		
	//获取特定号段的营业区号
	bool getSectionCodeByExchange(const string & sExchange,string &sSectionCode);
	
  static CConvertConfig* 		Create(); 
	static CConvertConfig *stCovertCfg_Instance;
	
	//获取单体实例的对象指针
	static CConvertConfig*	GetInstance() ;
	
  private:  
  
  bool GetAllParaInfo(); //获取所有参数信息。包含所有交换机的满表值，号段的营业区
  vector<CHANNEL_INFO> m_vecChannelInfo; 
  int m_channelSequnce;
  int m_nActiveDate ;
  vector<Meter_CFG> m_vecMeter_Cfg;
  map<string,string>  m_mapSection ;//存放各个号段对应的营业区信息 
  map<string ,string> m_mapFullScale ; //存放各个营业区跳次的满刻度值
};

#endif 
#ifndef _InfoMMApp_H_
#define _InfoMMApp_H_


#include "InfoPublic.h"
#include "appfrm/Application.h"
#include "sys/param.h"
#include "base/Directory.h"
#include "log4cpp/Logger.h"
#include "FileDeal.h"
#include "DestRuleParse.h"
#include "InfoProcess.h"
#include "ParamProcess.h"
#include "DbInterface.h"
#include "MmInterface.h"
#include "BatchInDBProcess.h"//add by wangfc@lianchuang.com
#include "CrmtoAct.h" //add by xueyf@lianchuang.com

class DbInterface;
class MmInterface;
class DestRule;
class FileDeal;
class DestRuleParse;
class InfoProcess;
class CrmtoAct;//add by xueyf@lianchuang.com

/**
* InfoMMApp:资料载入框架应用类,完成资料载入功能。
* 	InfoMMApp:			资料载入框架应用类构造函数
*	~InfoMMApp:			资料载入框架应用类析构函数
*	initialization:		完成应用相关资源的初始化工作
*	loopProcess:		完成资料装载工作
*	end:				完成应用相关资源的释放工作
*/
class InfoMMApp : public Application
{
	public:
		
		/**
		* InfoMMApp:资料载入框架应用类构造函数
		* @param	无
		* @return
		*/
		InfoMMApp();
		
		/**
		* ~InfoMMApp:资料载入框架应用类析构函数
		* @param	无
		* @return
		*/
		~InfoMMApp();


	protected:
		/**
		* initialization:完成应用相关资源的初始化工作
		* @param	无
		* @return	返回初始化结果 true:成功 false:失败
		*/
		bool initialization();
		
		/**
		* loopProcess:完成资料装载工作
		* @param	无
		* @return	返回装载处理结果 true:成功 false:失败
		*/		
		bool loopProcess();
		
		/**
		* end:完成应用相关资源的释放工作
		* @param	无
		* @return	返回终止处理结果 true:成功 false:失败
		*/
		bool end();


	private:
	
		/**
		* run:完成资料装载处理
		* @param	无
		* @return	资料装载结果 true:成功 false:失败
		*/		
		bool run();

		/**
		* getfilefullname:根据输入的路径和文件名获取文件全名
		* @param	filepath		路径
		* @param	filename		文件名
		* @param	filefullname	文件全名
		* @return	无
		*/	
		int getFileName();
		
		/**
		* getcurrtime:获取当前时间
		* @param	无
		* @return
		*/		
		void getcurrtime();

		/**
		* getfilefullname:根据输入的路径和文件名获取文件全名
		* @param	filepath		路径
		* @param	filename		文件名
		* @param	filefullname	文件全名
		* @return	无
		*/
		void getfilefullname(const char* filepath, const char* filename, char* filefullname);

		/**
		* getfilefullname:根据输入的路径和文件名获取文件全名
		* @param	filepath		路径
		* @param	filename		文件名
		* @param	filefullname	文件全名
		* @return	无
		*/		
		void addTime(string &baseTimeStr, const int& addSeconds);


	private:
		
		//默认系统数据库描述信息
		DbDesc							m_sysDbDesc;
		DbInterface						m_sysdbi;
		//物理数据库、内存数据库描述信息集合
		map<string, DbDesc>				m_dbDescMap;
		map<string, MmDesc>				m_mmDescMap;
		map<string, DbInterface*>		m_dbiMap;
		map<string, MmInterface*>		m_mmiMap;
		
		vector<DbInterface*>			m_dbisPro;
		vector<MmInterface*>			m_mmisPro;
		//资料信息列数、目标规则集合
		int								m_infolens[MAX_INFOTYPE_VALUE+1];
		LoadTable						m_loadtables[MAX_INFOTYPE_VALUE+1];
		vector<DestRule>				m_destRuleVector;
		//文件处理类、目标库分析类、信息处理类、参数处理类
		FileDeal						m_fileDeal;
		DestRuleParse					m_destRuleParse;
		InfoProcess						m_infoProcess;
		ParamProcess					m_paramProcess;
		//当前时间、最后一次处理的文件名称
		string							m_currtime;
		string							m_lastdealfilename;
		//文件路径、备份文件路径、错误文件路径、文件名称、文件分隔符号、休眠时间
		string							m_filepath;
		string							m_bakfilepath;
		string							m_errfilepath;
		string							m_filename;
		string							m_seperator;
		int							    m_sleeptime;
		int									m_tag;
		//允许的最大错误记录数
		int									m_errnum;
public://批量处理增加部分 wangfc@lianchuang.com 2010.3.30
    /*
      复位destrule相关的标志位
    */
    void   ResetBatchdestRuleParam();
    int    writeStatus(const char *msg,const char stat);
    void   reportErrmsgToDb(CException& e,string& chgid,int& infotype);
public:
  
   BatchInDBProcess m_BatchInDbProcess ;  
   int              m_Batchmaxrecordsize;
   string           m_hostName;
   /* CRM到ACT无缝全量专用  xueyf@lianchuang.com*/
   map<string, DbDesc>         m_dbcrmDescMap;
   map<string, DbInterface*>   m_dbcrmMap;
   int                         m_crmnum;
   int                         m_actnum;
   CrmtoAct                    m_CrmtoAct;
   //end
};


#endif


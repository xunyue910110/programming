// #################################################################
// Language:    C++
// OS Platform: UNIX
// Authors:     BOSS_DR GROUP
// Editors:     Chen Min  <chenm@lianchuang.com>
// Update       20040405
// Copyright (C) 2001 by Weiguo Wang, Linkage. All Rights Reserved.
// Comments: 
// #################################################################

#ifndef MYAPPLICATION_H_INCLUDED
#define MYAPPLICATION_H_INCLUDED



#include "appfrm/Application.h"
#include "log4cpp/Logger.h"
#include "base/Directory.h"
#include "base/SysParam.h"
#include "ChanInfo.h"
#include "Log.h"
#include "BaseCdrProcessor.h"
#include "GsmCdrProcessor.h"
#include "GprsCdrProcessor.h"
#include "InOutPut.h"

const int NAMELEN_MAX    = 1000;    // 最长文件名字符数
const int MAX_FILE_COUNT = 10000;   // 每通道一次处理的最大文件数
const int SLEEP_TIME     = 10;
                                    
class MergeApp : public Application
{
	public:
    	virtual ~MergeApp(){};

 	protected:
  		virtual bool initialization();
  		virtual bool loopProcess();
  		virtual bool end();
  		  		
  		// 得到配置文件中两大段的配置信息
  		bool getFileTypeConf();       //得到文件类型的配置信息
  		bool getChannelConf();        //得到通道的配置信息
  		
  		// 定义读取文件参数方法 
  		string getParameter(const char* path, const char* name);
  		int getFile(char *filename, const int length);
  		
		//推测该文件的类型
		bool determineFileType(const char* filename);
		// 预处理文件：查找文件类型、校验文件连续性
		int preProessChannel(char *fileName) ; 
		bool process(const char *fileName);
		// 得到下一处理通道信息(如:chan_0)
		bool getNextChanInfo();

		
		void writeLog();
		
		void getLastFilesSerial();
		
  		
 	private:  		  		  		 		
  		// 日志
  		Log *m_pLog;
  		
  		// 记录一个输入通道下累计处理的文件数
  		int m_fileSum;
  		int m_subChanNo;
  		
  		string m_savedPoolPath;
  		string m_beforeUnitePath; //存放已经合并了的话单 add by lizhao 2008-10-25
  		
  		// 所处理的通道信息集合
  		vector<ChanInfo *> m_chanInfos;
  		// 记录当前处理的通道信息
  		ChanInfo * m_pChanInfo; 
  		
  		// 处理器
  		BaseCdrProcessor  *m_pCdrProcessor;
  		vector<BaseCdrProcessor *> m_vectCdrProc;
  		
  		// 输入输出
  		InOutPut *m_inOutPut;
  		Directory m_dir;
};


#endif 

// ##########################################
// Source file : InOutPut.h
// System      : Mobile Billing System
// Version     : 0.1.4
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20011029
// Update      : 20020106
// Copyright(C): 2001 by Yang Zhang, Linkage.
// ##########################################

#ifndef INOUTPUT_H_INCLUDED_C422EF75
#define INOUTPUT_H_INCLUDED_C422EF75

#include "config-all.h"
#include "base/supdebug.h"
#include <unistd.h>


#include <vector>
#include <string>
#include <stdio.h>

#include "BaseCdrProcessor.h"
#include "ChanInfo.h"
#include "appfrm/ErrorHandle.h"
#include "MergeAlertCode.h"

class  BaseCdrProcessor;

extern ErrorMessages * const theErrorMessages;

const string TOKEN(",");

class InOutPut
{
	public:
		InOutPut(const string &path,const string &path1,const int chanNo);

		// 当进程启动时,从log目录下的日志文件中获得文件序号
		void readSavedPools( vector<ChanInfo *> &chanInfos, vector<BaseCdrProcessor *> &vectCdrProc );
		// 保存缓存中的记录到文件中
		bool savePools2Files(const string &strPoolType,const int subChanNo,CDR_POOL *cdrPool,multimap<string,vector<string> > *lastCdr, multimap<string, vector<string> > *needUnite);
		
		bool readCdr(char * cdrBuffer,const int n);
		bool writeCdr(const char* cdr );
		bool writeLogFile(const string &strPoolType,const int subChanNo, multimap<string, vector<string> > logvect); //add by lizhao 2008-10-25 写日志文件

		// 打开输入输出文件
		bool openFile(const char * cpFileName);
		//关闭输入输出路径下的文件
		void closeFile(const char * cpFileName);

		void  reset(const string &srcPath,const string &desPath );
		
		void addWrite2file() {m_write2file++;}   //写入输出文件的话单数
		
		int getWrite2file()  {return m_write2file;}
		
		static string m_LogFileNameWithPath;
		
		int m_totalfieldcount;
		int m_FlagPosi;
			
	private:
		// 缓存存放目录
		string m_savedPoolPath;
		string m_beforeUnitePath; //存放合并话单以前的记录 add by lizhao 2008-10-25
		
		
		// 进程通道号
		string m_channelNo;
		
		string m_tmpFileHead;
		string m_srcPath ;
		string m_desPath ;
		
		int m_write2file;
		

		
		FILE * m_inputFp;
		FILE * m_outPutFp;
		FILE * m_LogFp; // 话单日志文件 add by lizhao 2008-10-25
		 
		
		string m_fileName;
};

#endif 

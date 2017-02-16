// ##########################################
// Source file : MyApplication..cpp
// System      : NEW GENERATION BILLING SYSTEM
// Version     : 0.1
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Zhao Liyang
// E_mail      : zhaoly@lianchuang.com
// Create      : 20081018
// Update      : 20081029
// Copyright(C): 2008 by Zhao Liyang, Linkage.
// ##########################################

#ifndef SendApp_H_INCLUDED_C4864891
#define SendApp_H_INCLUDED_C4864891

#include "base/supdebug.h"
#include "config-all.h"
#include "base/Directory.h"
#include "appfrm/Application.h"
#include "appfrm/ErrorHandle.h"

#include <string>
#include <stdlib.h>
#include <sys/time.h>

#include "SendLog.h"
#include "CdrFlag.h"
#include "SendParameterFromFile.h"
#include "RawCdrProcessor.h"
#include "RawFileProcessor.h"
#include "GeneralFile.h"
#include "OutputFile.h"

#include "TokenRawCdrProcessor.h"
#include "TokenFormat.h"

#include "FieldValidators.h"
#include "FieldInfo.h"

#include "FormatConversion.h"
#include "CdrErrorCode.h"

USING_NAMESPACE(std)
  
extern ErrorMessages * const theErrorMessages;
double gettime();
#define NOFILE_IDLE  20900    //原始目录无文件信号

//## 文件管理类
//## 此类负责查找所有的原始话单文件，调用RawFileProcess对象，对话单文件进行处理。
//## 此类由外部实现，在此仅表示逻辑关系。
class SendApp
{
  public:
  //add by gaojf 2009-2-27 13:43
  bool setSrcPath(const char * r_srcFilePath);  
    
	int mainProcess(int flag,int iFileNo,string aSrcFilePath,int isHeadOrTail);
	int run(string aSrcFileName);
	SendApp(string theDataSource,int isShow);
	SendApp(int theDataSource,int isShow);
	~SendApp();
  int getOutFileName(int iFileNo,int iFileNoLen,string &sOutFileName);
  int getOutFileNameRt(const string &aSrcFilePath,string &sOutFileName);
	//static OutputFile * getOutPutInst()
	//{
	//	return &m_OutputFile;	
	//};

	Directory m_dir;

  public:
	bool initialize();
	bool initialize(string theSwitchName,vector <string> &pathMessage,
                   float errorRate,const string isBackupSrc,const string isBackupDes,
                   const string isIndb,string isCheckCdrNo,bool bCheckFilename);
	RawFileProcessor *m_theRawFileProcess;
	RawCdrProcessor *m_theCdrProcess;
	//vector<FieldInfo> m_FieldInfoMaps;
	multimap<MyString,FieldInfo *> m_FieldInfoMaps;
	static OutputFile m_OutputFile;
	FieldValidators m_FieldValidators;
	//PreProApi m_PreProApi;

  private:
	RawFileProcessor *createFileObject();
	RawCdrProcessor *createCdrProcessObject();
	void showSrcCdrBuffer(const unsigned char *theSrcCdrBuffer,int size);
	//void showSrcFields(vector<FieldInfo> &theFieldInfoMaps);
	void showSrcFields(multimap<MyString,FieldInfo *> &theFieldInfoMaps);
	bool reset(string srcFileName);
  void writeSendLog();   
  void setNowTime(void);
  int getDataFromFile(string aSrcFilePath);
  int getDataFromFileRt(string aSrcFileName);
  //int getOutFileName(int iFileNo,int iFileNoLen,string &sOutFileName);

  private:
	SendLog *m_SendLog_;
	SendParameterFromFile *m_theSendParameter_;
	unsigned char *m_srcCdrBuffer;
	CdrFlag *m_cdrFlag;
	int m_dataSource;
  bool m_AlertByEmptyFile;
  int m_output_filehead_size;
    
  //输出文件名属性
  string m_nowTime;
  string m_outfile_name;
  string m_filename_head;
  string m_file_dateinfo;
  string m_file_namefield3;
  string m_filename_end;
  
  //add by zhaoly for 3G-DM-IMEI 2009-4-28 0:22:19
   string m_file_namefield2;

  //判断文件名的格式
  bool mb_CheckFileName; //是否对输入文件格式进行判断
  string m_filepattern; //文件名head 
  
  int m_errcode_pos;
    
};

#endif /* SendApp_H_INCLUDED_C4864891 */


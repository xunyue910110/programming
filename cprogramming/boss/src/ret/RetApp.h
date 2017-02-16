// ##########################################
// Source file : RetApp.h
// System      : Mobile Billing System
// Version     : 0.1.25
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010816
// Update      : 20020415
// Copyright(C): 2002 by Yang Zhang, Linkage.
// ##########################################

#ifndef RETAPP_H_INCLUDED_C4864891
#define RETAPP_H_INCLUDED_C4864891

#include "base/supdebug.h"
#include "config-all.h"

#include <string>
#include <stdlib.h>
#include <sys/time.h>

#include "RetLog.h"
#include "CdrFlag.h"
#include "RawCdrProcessor.h"
#include "OutputFile.h"
#include "CdrErrorCode.h"
#include "RawFileProcessor.h"
#include "RetParameterFromFile.h"

#include "AsnRawCdrProcessor.h"
#include "FixedRawCdrProcessor.h"
#include "TokenRawCdrProcessor.h"
#include "FieldInfo.h"
#include "GeneralFile.h"


#include "FieldValidators.h"
//#include "PreProApi.h"
#include "AsnFormat.h"
#include "TokenFormat.h"

#include "appfrm/ErrorHandle.h"
#include "Baf.h"

USING_NAMESPACE(std)

extern ErrorMessages * const theErrorMessages;
extern Baf g_theBaf;        //处理nortel等话单中的module by chenmin at 20020415

double gettime();


//##ModelId=3B4E92B7021B
//##Documentation
//## 文件管理类
//## 此类负责查找所有的原始话单文件，调用RawFileProcess对象，对话单文件进行处理。
//## 此类由外部实现，在此仅表示逻辑关系。
//##
class RetApp
{
  public:
	//int mainProcess(int flag, int iFileNo, int iFileNoLen, string aSrcFilePath, string aRcdFileName);
  int mainProcess(int iRetFileType,string aSrcPath,string aSrcFileName,string sErrorType,
                  string sErrorCode,string sRetHead,string sRetTail);
                  
	//##ModelId=3B4EB03E03D3
	int run(string aSrcFileName);

	//##ModelId=3B6E475B01F6
	RetApp(string theDataSource,int isShow);

	//##ModelId=3BD629650146
	RetApp(int theDataSource,int isShow);

	//##ModelId=3B6E476701B7
	~RetApp();

	//static OutputFile * getOutPutInst()
	//{
	//	return &m_OutputFile;	
	//};
  int judgeErrType(string sFullSrcFileName);
  
  public:
	//##ModelId=3B66827100F1
	bool initialize();

	//##ModelId=3BD42636015F
	bool initialize(string theSwitchName,vector <string> &pathMessage,
                   float errorRate,const string isBackupSrc,const string isBackupDes,string isCheckCdrNo);

	//##ModelId=3B676FF80107
	RawFileProcessor *m_theRawFileProcess;

	//##ModelId=3B676FFC012B
	RawCdrProcessor *m_theCdrProcess;

	//##ModelId=3B8E48400392
	multimap<MyString,FieldInfo *> m_FieldInfoMaps;

	//##ModelId=3BB2E1F4008F
	//static OutputFile m_OutputFile;
	OutputFile m_OutputFile;

	//##ModelId=3BCCE99A01F9
	FieldValidators m_FieldValidators;

	//##ModelId=3C29CA5601D6
	//PreProApi m_PreProApi;

  private:
	//##ModelId=3B6F84CF0102
	//RawFileProcessor *createFileObject(int rawFileType);
  RawFileProcessor *createFileObject();
  
	//##ModelId=3B6F84D000D2
	RawCdrProcessor *createCdrProcessObject(int cdrType);

	//##ModelId=3B936C65007B
	void showSrcCdrBuffer(const unsigned char *theSrcCdrBuffer,int size);

	//##ModelId=3B936C65025B
	void showSrcFields(multimap<MyString,FieldInfo *> &theFieldInfoMaps);

	//##ModelId=3BB322560003
	bool reset(string srcFileName);

	//##ModelId=3BD82E8E00CD
   void writeRetLog();

  private:
	//##ModelId=3B6EAA81009F
	RetLog *m_RetLog_;

	//##ModelId=3B8E0F0D0274
	RetParameterFromFile *m_theRetParameter_;

	void getRetFileName(string aSrcFileName);

	//##ModelId=3BB31A1B0053
	unsigned char *m_srcCdrBuffer;

	//##ModelId=3BD906B30047
	CdrFlag *m_cdrFlag;

	//##ModelId=3C3037ED02B3
	int m_cdrType;

	// 定长话单块大小判断 add by gaoxl 20080305
	int m_file_head_size;
	int m_file_tail_size;
	int m_cdr_size;
	int m_real_block_size;
 
  //回执文件名获取
  int m_retfile_type;
  string m_switch_part;
  int m_part_length;
  string m_retfile_name;	
  
  //上发回执相关
  int m_errBegin;
  int m_errCodeLen;
  int m_retFlag;
  string m_retCode;

	//##ModelId=3C3D7D9602D5
	int m_dataSource;
	// add by chenm 2006-6-23 空文件是否告警
	bool m_AlertByEmptyFile;

};

#endif /* RETAPP_H_INCLUDED_C4864891 */

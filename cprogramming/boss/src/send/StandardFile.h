// ##########################################
// Source file : StandardFile.h
// System      : Mobile Billing System
// Version     : 0.1.10
// Language	   : ANSI C++
// OS Platform : HPUX,AIX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010925
// Update      : 20011227
// Copyright(C): 2001 by Yang Zhang, Linkage.
// ##########################################

#ifndef STANDARDFILE_H_INCLUDED_C44F9EA9
#define STANDARDFILE_H_INCLUDED_C44F9EA9

#include "config-all.h"

#if defined(STDIOSTREAM_DISABLE)
    #include <iostream.h>
    #include <fstream.h>
#else
    #include <iostream>
    #include <fstream>
#endif

#include <string>
#include <vector>
#include <map>
#include <stdio.h>

#include "CdrFlag.h"
#include "FieldInfo.h"
#include "StandardFile.h"
#include "CdrErrorCode.h"
#include "StandardField.h"
#include "FormatConversion.h"

#include "appfrm/ErrorHandle.h"
extern ErrorMessages * const theErrorMessages;


USING_NAMESPACE(std)

//##ModelId=3B4EB63C0017
//##Documentation
//## 标准话单记录文件输出类

const string TMP_PRIFIX ("#");
const int MAX_STDFILE_CDR_NUM (20000);
class StandardFile
{
  public:

	//##ModelId=3BB08CC9004D
	//int write(vector<FieldInfo> &theFieldInfoMaps);
  int write(multimap<MyString,FieldInfo *> &theFieldInfoMaps);
  int writeFileHead(int fileno);
  int writeFileTail(int fileno);
    
	//##ModelId=3BB18C7F01EA
	bool reset(const string outputFileName);

	//##ModelId=3BB18D0900E4
	void close(int flag);

	//##ModelId=3BB1954002BF
	StandardFile();

	//##ModelId=3BB198BA02BF
		/* modify by dingj 2010-3-28 09:52:17
	 * add parameter outFileHType
	 */
	bool initialize(vector<string> &theStandardFile,vector<string> &theStandardFileHead,
	          vector<string> &theStandardFileTail,string isBackup,string isIndb,string outFileType,string outFileHType,
	          string fileFreq,string outFileHSize,//add for vpdn 2010-4-9 14:28:51 by zhaoly
						string desPath,string desBakPath,string invalidPath,string indbPath);
  
  public:

  protected:
	//## 标准话单分隔符
	char  m_token[3];
	/*
	 * add by dingj 2010-3-28 10:08:41
	 */
	char  m_htoken[3];
	
  private:
  //void getfieldValue( vector<FieldInfo> &theFieldInfoMaps,int i,char *const&outFields);
  void getfieldValue( multimap<MyString,FieldInfo *> &theFieldInfoMaps,int i,char *const&outFields);
	//const FieldInfo *getfieldValue( vector<FieldInfo> &theFieldInfoMaps,string &fieldName);
  const FieldInfo *getfieldValue( multimap<MyString,FieldInfo *> &theFieldInfoMaps,string &fieldName);
  //bool getOutCdrWithToken( vector<FieldInfo> &theFieldInfoMaps,char *outFields);
  bool getOutCdrWithToken( multimap<MyString,FieldInfo *> &theFieldInfoMaps,char *outFields);
  //bool getOutCdrWithFix( vector<FieldInfo> &theFieldInfoMaps,char *outFields);
  bool getOutCdrWithFix( multimap<MyString,FieldInfo *> &theFieldInfoMaps,char *outFields);
   
  bool getFileHead(int fileno,char *outFields);
  bool getFileTail(int fileno,char *outFields);
  
  //void getNowTime(void);
  void setNowTime(void);
  
  bool conveyCdr(const int iFileNameSequnce,const int iFileSize);
  
  bool splitStdFile(void);
  int setErrorCode(  char *buf ,int errorCode,  char split, int index);

  private:
  //输出目标文件句柄
	FILE *m_fDes;
	FILE *m_fIndb;
	FILE *m_fInvalid;

	//##ModelId=3BC443CB02FF
	char m_desBuffer[1024*4];        // is 4kB file buffer

	//##ModelId=3BC543B1019F
	char m_invalidBuffer[1024*4];    // is 4kB file buffer
	
	char m_indbBuffer[1024*4];    // is 4kB file buffer	

	//##ModelId=3BB32CE50371
	string m_desBakPath;

	//##ModelId=3BB1760C02B9
	string m_desPath;
	string m_errPath;
	string m_indbPath;

	//add for vpdn 2010-4-9 14:28:51 by zhaoly
	int m_fileFreq;
	int m_outFileHSize;
	
	//##ModelId=3BC53EED0335
	string m_invalidPath;

	//##ModelId=3BC24C8502E3
	string m_FileName;
	//string m_errFileName;
	string m_invalidFileName;
  string m_indbFileName;
  
	//##ModelId=3BB3F5D4023F
	bool m_isBackupDesFile;
	bool m_isIndb;

	//##ModelId=3BC53A6403E1
    CdrFlag *m_cdrFlag;
    
    SendLog   *m_SendLog;

	//##ModelId=3BB0817F02F1
	vector<StandardField> m_StandardField;

	vector<StandardField> m_StandardFieldHead;
  
	vector<StandardField> m_StandardFieldTail;
	
	//##ModelId=3BFB6DE30028
	float m_errorRate;

	//##ModelId=3C4CF68301F0
	string m_outFileType;
	/* add by dingj 2010-3-28 10:10:39
	 */
	string m_outFileHType;	

	string m_nowTime;
	//char m_nowDate[6+1];
	
	string m_startDate;
	string m_startTime;
};




#endif /* STANDARDFILE_H_INCLUDED_C44F9EA9 */

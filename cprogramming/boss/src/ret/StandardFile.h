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
	int write(multimap<MyString,FieldInfo *> &theFieldInfoMaps);
  int writeFileHead(string sRetHead);
	int writeFileTail(string sRetTail);
	
	//##ModelId=3BB18C7F01EA
	bool reset(const string outputFileName);

	//##ModelId=3BB18D0900E4
	void close(int flag);

	//##ModelId=3BB1954002BF
	StandardFile();

	//##ModelId=3BB198BA02BF
	bool initialize(vector<string> &theStandardFile,string isBackup,string outFileType,
						string desPath,string desBakPath,string invalidPath);

  public:

  protected:
	//##ModelId=3BB075860203
	//##Documentation
	//## 标准话单分隔符
	char  m_token[3];

  private:

	//##ModelId=3BC25A92030E
   void getfieldValue( multimap<MyString,FieldInfo *> &theFieldInfoMaps,int i,char *const&outFields);

	//##ModelId=3BC262700115
	const FieldInfo *getfieldValue( multimap<MyString,FieldInfo *> &theFieldInfoMaps,string &fieldName);

	//##ModelId=3C4CF6830222
   bool getOutCdrWithToken( multimap<MyString,FieldInfo *> &theFieldInfoMaps,char *outFields);

	//##ModelId=3C4CF6830376
   bool getOutCdrWithFix( multimap<MyString,FieldInfo *> &theFieldInfoMaps,char *outFields);
   
   void getNowTime(void);
   
   bool conveyCdr(const int iFileNameSequnce,const int iFileSize);
   
   bool splitStdFile(void);

  private:
	//##ModelId=3BB18A1F0308
	FILE *m_fDes;

	//##ModelId=3BC53A64035E
	FILE *m_fInvalid;

	//##ModelId=3BC443CB02FF
	char m_desBuffer[1024*4];        // is 4kB file buffer

	//##ModelId=3BC543B1019F
	char m_invalidBuffer[1024*4];    // is 4kB file buffer

	//##ModelId=3BB32CE50371
	string m_desBakPath;

	//##ModelId=3BB1760C02B9
	string m_desPath;

	//##ModelId=3BC53EED0335
	string m_invalidPath;

	//##ModelId=3BC24C8502E3
	string m_FileName;

	//##ModelId=3BB3F5D4023F
	bool m_isBackupDesFile;

	//##ModelId=3BC53A6403E1
    CdrFlag *m_cdrFlag;
    
    RetLog   *m_RetLog;

	//##ModelId=3BB0817F02F1
	vector<StandardField> m_StandardField;

	//##ModelId=3BFB6DE30028
	float m_errorRate;

	//##ModelId=3C4CF68301F0
	string m_outFileType;

	char m_nowTime[11];
	
	string m_startDate;
	string m_startTime;

};




#endif /* STANDARDFILE_H_INCLUDED_C44F9EA9 */

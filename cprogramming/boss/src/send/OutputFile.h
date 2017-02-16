// ##########################################
// Source file : OutputFile.h
// System      : Mobile Billing System
// Version     : 0.1.7
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010925
// Update      : 20011127
// Copyright(C): 2001 by Yang Zhang, Linkage.
// ##########################################

#ifndef OUTPUTFILE_H_INCLUDED_C44F6859
#define OUTPUTFILE_H_INCLUDED_C44F6859

#include "config-all.h"

#if defined(STDIOSTREAM_DISABLE)
#include <iostream.h>
#include <fstream.h>
#else
#include <iostream>
#include <fstream>
#endif

#include <vector>
#include <string>
#include <map>
#include "SendLog.h"
#include "CdrFlag.h"
#include "FieldInfo.h"
#include "StandardFile.h"

#include "appfrm/ErrorHandle.h"
extern ErrorMessages * const theErrorMessages;

USING_NAMESPACE(std);

//##ModelId=3BB087870247
//##Documentation
//## 输出文件处理类
class OutputFile
{
  public:
	//int write(vector<FieldInfo> &theFieldInfoMaps);
  int write(multimap<MyString,FieldInfo *> &theFieldInfoMaps);


  int writeFileHead(int fileno);
  int writeFileTail(int fileno);
  
	//##ModelId=3BB14B5D034D
	bool reset(string fileName);

	//##ModelId=3BB30DE50044
	int setFileBuffer(string aSrcFileName,unsigned char *&srcFileBuffer);

	//##ModelId=3BB14B8A01D5
	//##Documentation
	//## 关闭全部输出文件句柄
	//## flag=0,正确 ； flag =1,错误
	void closeSrcFiles(int flag);
	void closeDesFiles(int flag);

	//##ModelId=3BB18B4C0257
	~OutputFile();

	//##ModelId=3BC516710364
	OutputFile();

	//##ModelId=3BB19885038B
	bool initialize(vector<string> aStandardFile,vector<string> aStandardFileHead,
	            vector<string> aStandardFileTail,vector<string> aFilePath,
					    string isBackupSrc,string isBackupDes,string isIndb,
					    string isCheckCdrNo,string outFileType,string outFileHType,
					    string fileFreq,string outFileHSize,float errorRate);//add fileFreq,outFileHSize for vpdn 2010-4-9 14:16:49 by zhaoly

	//##ModelId=3BFCB54F018C
	void mvToErrDir(string & filename);
	
	// add by gaoxl 20080305 定长格式话单块大小出错，文件移至错误目录
	void mvToErrorDir(string & filename);

  private:
	//##ModelId=3BB3D0CB0373
	string getPath(vector<string> aFilePath,string pathName);

  private:
	//##ModelId=3BB08BF90329
	StandardFile m_StandardFile;

	//##ModelId=3BB30EAD03D1
	string m_srcFilePath;

	//##ModelId=3BB30EAE01F2
	int m_srcfd;

	//##ModelId=3BB30EAE0333
	int m_fileLength;

	//##ModelId=3BB32D5D026F
	string m_srcFileBakPath;

	//##ModelId=3BB3386D013B
	unsigned char *m_srcFileBuffer;

	//##ModelId=3BB32D980396
	string m_fileErrorPath;

	//##ModelId=3BB330640039
	string m_srcFileName;

	//##ModelId=3BB3F5AC00B1
	bool m_isBackupSrcFile;

	//##ModelId=3BC51749012A
    CdrFlag *m_cdrFlag;

	//##ModelId=3BFB5E7D0341
	float m_errorRate;

	//##ModelId=3BFB6A790220
    SendLog *m_SendLog_;

	//##ModelId=3C218740009D
	bool m_isCheckCdrNo;

};

#endif /* OUTPUTFILE_H_INCLUDED_C44F6859 */

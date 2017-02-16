// ##########################################
// Source file : RawFileProcessor.h
// System      : Mobile Billing System
// Version     : 1.05
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010816
// Update      : 20010830
// Copyright(C): 2001 by Yang Zhang, Linkage.
// ##########################################

#ifndef RAWFILEPROCESS_H_INCLUDED_C4845358
#define RAWFILEPROCESS_H_INCLUDED_C4845358

#include "config-all.h"

#include <string>
#include "Block.h"
//#include "FileHead.h"
#include "CdrExtractor.h"
//#include "AsnCdrExtractor.h"
//#include "GAsnCdrExtractor.h"
//#include "FixedCdrExtractor.h"
#include "TokenCdrExtractor.h"
#include "SendParameterFromFile.h"

USING_NAMESPACE(std)

//##ModelId=3B4E68040029
//##Documentation
//## 原始话单文件处理方法类。此类保存了处理单个话单文件的处理方法和属性。其输入为一个原始话单文件，原始话单记录容器指针。
//## 此类为抽象类，具体有其继承类实现，继承类有GeneralFile和Nortel_DMS-MSC。
//##
//##
class RawFileProcessor {
  public:

	//##ModelId=3B820A00013E
  	RawFileProcessor()
  	{
  		m_SendLog=SendLog::getInstance();
		if ( m_SendLog == 0 )
			exit(0);
	};

	//##ModelId=3B978CA7000C
	virtual ~RawFileProcessor();


	//##ModelId=3B4EAB2502BE
	//##Documentation
	//## 得到文件原始记录的方法，该方法实际上调用的是ExtractCdrMethod类里的同名方法。
	virtual int getCdr(unsigned char *&CdrBuffer) = 0;

	//##ModelId=3B60C5A603E4
	//##Documentation
	//## 文件校验方法
	virtual int postValid();

	//##ModelId=3B6759BC00B3
	//##Documentation
	//## 文件处理初始化方法，由主控对象(PreProcApplication)调用.
	virtual bool initialize(SendParameterFromFile *aSendParameter,int cdrType) = 0;

	//##ModelId=3B71083B00EA
	bool attach(char *filename);

	//##ModelId=3B710D0B0073
	int preValid();

	//##ModelId=3B8217ED00A4
	virtual bool setRawFile(const string SrcFileName,const unsigned char *FileBuffer,const int FileLength);
	
	virtual void setToken(const string token)=0;
  protected:

	//##ModelId=3B820A0000EF
	SendLog *m_SendLog;

  protected:
	//##ModelId=3B57BD820265
	//##Documentation
	//## 原始文件名
	string m_FileName;

	//##ModelId=3B836FB80180
	const unsigned char *m_FileBuffer;

	//##ModelId=3B836FB80375
	int m_FileLength;


  private:


};



#endif /* RAWFILEPROCESS_H_INCLUDED_C4845358 */

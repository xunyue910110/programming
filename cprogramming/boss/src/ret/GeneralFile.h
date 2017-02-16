// ##########################################
// Source file : GeneralFile.h
// System      : Mobile Billing System
// Version     : 0.1.6
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010816
// Update      : 20011025
// Copyright(C): 2001 by Yang Zhang, Linkage.
// ##########################################


#ifndef GENERALFILE_H_INCLUDED_C4861D6D
#define GENERALFILE_H_INCLUDED_C4861D6D

#include "config-all.h"

#include <string>
#include "RawFileProcessor.h"

#include "appfrm/ErrorHandle.h"
extern ErrorMessages * const theErrorMessages;

USING_NAMESPACE(std)

//##ModelId=3B5D2EDE0051
//##Documentation
//## 通用话单文件处理类。此类支持定长格式话单和ASN变长格式话单。
//##
class GeneralFile : public RawFileProcessor {
  public:
	//##ModelId=3B6216660176
	GeneralFile();

	//##ModelId=3B6216660248
	virtual ~GeneralFile();

	//##ModelId=3B832B7B02EA
	virtual int getCdr(unsigned char *&CdrBuffer) ;

	//##ModelId=3B839E6F03CC
	virtual bool setRawFile(const string SrcFileName,const unsigned char *FileBuffer,const int FileLength);
	//##ModelId=3B832B7C00B0
	//##Documentation
	//## 文件校验方法
	virtual int postValid();

	//##ModelId=3B832B7C011F
	//##Documentation
	//## 文件处理初始化方法，由主控对象(PreProcApplication)调用.
	virtual bool initialize(RetParameterFromFile *aRetParameter,int cdrType) ;
	
	virtual void setToken(const string token);
  protected:

  private:
	//##ModelId=3B820A000243
	CdrExtractor *createExtractCdr(int aCdrType);

	//##ModelId=3B83782C03C7
	//##ModelId=3B83782C03C7
	int getNextBlock();

  private:
	//##ModelId=3B64CD4B02F0
	Block m_theBlock;

	//##ModelId=3B64CFFA0262
	//FileHead m_theFileHead;

	//##ModelId=3B669F960212
	CdrExtractor *m_theCdrExtractor;

  private:
	//##ModelId=3B56586A0388
	//##Documentation
	//## 记录块尺寸
	//##
	int m_BlockSize;

	//##ModelId=3B5657FF0302
	//##Documentation
	//## 文件头尺寸
	int m_FileHeaderSize;

	//##ModelId=3B8366480082
	//##Documentation
	//## 当前记录块号
	int m_BlockNo;

	//##ModelId=3B5658130076
	//##Documentation
	//## 文件尾尺寸
	int m_FileTailSize;

	//##ModelId=3B83664801EB
	//##Documentation
	//## 当前块缓存
	unsigned char *m_BlockBuffer;

	//##ModelId=3B8366470059
	//##Documentation
	//## 实际记录块长度
	int m_RealBlockSize;

	//##ModelId=3B8475B201A4
	//##Documentation
	//## 当前记录块偏移量
	int m_BufferOffset;

	//##ModelId=3B8366470316
	//##Documentation
	//## 记录块总数
	int m_TotalBlocks;
	
	// 根据配置,显示是否为摩托罗拉的gprs话单
	bool m_isMotoGprs;
	
	// add by chenm 2006-6-23 空文件是否告警
	bool m_AlertByEmptyFile;
	
	// 根据配置,显示是否为CNG话单
	bool m_isCNGggsn;
};



#endif /* GENERALFILE_H_INCLUDED_C4861D6D */

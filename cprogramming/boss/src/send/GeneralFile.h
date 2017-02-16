// ##########################################
// Source file : GeneralFile.h
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

#ifndef GENERALFILE_H_INCLUDED_C4861D6D
#define GENERALFILE_H_INCLUDED_C4861D6D

#include "config-all.h"
#include <string>
#include "RawFileProcessor.h"
#include "appfrm/ErrorHandle.h"
#include "base/supdebug.h"

extern ErrorMessages * const theErrorMessages;
USING_NAMESPACE(std)

// 通用话单文件处理类.
class GeneralFile : public RawFileProcessor {
  public:
	GeneralFile();
	virtual ~GeneralFile();
	virtual int getCdr(unsigned char *&CdrBuffer) ;
	virtual bool setRawFile(const string SrcFileName,const unsigned char *FileBuffer,const int FileLength);
	// 文件校验方法
	virtual int postValid();
	// 文件处理初始化方法，由主控对象(SendApp)调用.
	virtual bool initialize(SendParameterFromFile *aSendParameter,int cdrType) ;
	virtual void setToken(const string token);

  protected:

  private:
	CdrExtractor *createExtractCdr(int aCdrType);
	//CdrExtractor *createExtractCdr();
	int getNextBlock();

  private:
	Block m_theBlock;
	CdrExtractor *m_theCdrExtractor;

  private:
	// 记录块尺寸
	int m_BlockSize;
	// 当前记录块号
	int m_BlockNo;
	// 当前块缓存
	unsigned char *m_BlockBuffer;
	// 实际记录块长度
	int m_RealBlockSize;
	// 当前记录块偏移量
	int m_BufferOffset;
	// 记录块总数
	int m_TotalBlocks;
	//CDMA话单块的不定长特殊性 yuanjun 20060703
	int m_BlockFlag; //1代表需要计算，0（默认）是数字
};



#endif /* GENERALFILE_H_INCLUDED_C4861D6D */

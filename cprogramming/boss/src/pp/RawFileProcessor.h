// ##########################################
// Source file : RawFileProcessor.h
// System      : Mobile Billing System
// Version     : NG1.0
// Language    : ANSI C++
// OS Platform : HPUX
// Authors     : Gao xile
// E_mail      : gaoxl@lianchuang.com
// Create      : 20080619
// Update      : 20080619
// Copyright(C): 2008 by Gao xile, Linkage.
// ##########################################

#ifndef RAWFILEPROCESSOR_H_HEADER_INCLUDED_B7A6521B
#define RAWFILEPROCESSOR_H_HEADER_INCLUDED_B7A6521B

#include "config-all.h"
#include "Block.h"
#include "CdrExtractor.h"
#include "AsnCdrExtractor.h"
#include "GAsnCdrExtractor.h"
#include "FixedCdrExtractor.h"
#include "SasnCdrExtractor.h"
#include "TokenCdrExtractor.h"
#include "PpLog.h"
#include "PpParameterFromFile.h"
#include <string>

USING_NAMESPACE(std);

//##ModelId=4850D5700186
//##Documentation
//## 原始话单文件处理方法类.此类保存了处理单个话单文件的处理方法和属性
//## 其输入为一个原始话单文件,原始话单记录容器指针
//## 此类为抽象类,具体有其继承类实现,继承类有GeneralFile
class RawFileProcessor
{
  public:
    //##ModelId=4850ECD10271
    RawFileProcessor()
    {
        m_PpLog = PpLog::getInstance();
		if (m_PpLog == 0)
		{
			exit(0);
		}
    }

    //##ModelId=4850ECD8035B
    virtual ~RawFileProcessor();

    //##ModelId=4850ED420213
    //##Documentation
    //## 得到文件原始记录的方法,该方法实际上调用的是CdrExtractor类里的同名方法
    virtual int getCdr(unsigned char *&CdrBuffer) = 0;

    //##ModelId=4850ED7E037A
    //##Documentation
    //## 文件校验方法
    virtual int postValid();

    //##ModelId=4850EDBE031C
    //##Documentation
    //## 文件处理初始化方法,由主控对象(PreProcApplication)调用.
    virtual bool initialize(PpParameterFromFile *aPpParameter, int cdrType) = 0;

    //##ModelId=4850EDF10186
    bool attach(char *filename);

    //##ModelId=4850EE0B00EA
    int preValid();

    //##ModelId=4850FA8902DE
    virtual bool setRawFile(const string &SrcFileName, const unsigned char *FileBuffer, const int FileLength);

    //##ModelId=4850FAC801C5
    virtual void setToken(const string token) = 0;

  protected:
    //##ModelId=4850FAF4036B
    PpLog *m_PpLog;

    //##ModelId=4850FB0302DE
    string m_FileName;

    //##ModelId=4850FB0F0109
    const unsigned char *m_FileBuffer;

    //##ModelId=4850FB1D0280
    int m_FileLength;

  public:
    //##ModelId=4850FB1D0281
    unsigned char *m_FileHeadBuffer;

    //##ModelId=4850FB1D0282
	unsigned char *m_FileTailBuffer;

};



#endif /* RAWFILEPROCESSOR_H_HEADER_INCLUDED_B7A6521B */

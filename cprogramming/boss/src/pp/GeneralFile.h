// ##########################################
// Source file : GeneralFile.h
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

#ifndef GENERALFILE_H_HEADER_INCLUDED_B7A65AC5
#define GENERALFILE_H_HEADER_INCLUDED_B7A65AC5

#include "RawFileProcessor.h"
#include "config-all.h"
#include "appfrm/ErrorHandle.h"
#include <string>

extern ErrorMessages * const theErrorMessages;

USING_NAMESPACE(std);

//##ModelId=48523A750213
//##Documentation
//## 通用话单文件处理类。此类支持定长格式话单和ASN变长格式话单
class GeneralFile : public RawFileProcessor
{
  public:
    //##ModelId=48523A7F0177
    GeneralFile();

    //##ModelId=48523A8102BF
    virtual ~GeneralFile();

    //##ModelId=4855B547034B
    virtual int getCdr(unsigned char *&CdrBuffer);

    //##ModelId=4855B56A00FA
    virtual bool setRawFile(const string &SrcFileName, const unsigned char *FileBuffer, const int FileLength);

    //##ModelId=4855B5A30138
    //##Documentation
    //## 文件校验方法
    virtual int postValid();

    //##ModelId=4855B5BB02DE
    //##Documentation
    //## 文件处理初始化方法，由主控对象(PreProcApplication)调用.
    virtual bool initialize(PpParameterFromFile *aPpParameter, int cdrType);

    //##ModelId=4855B5DD01A5
    virtual void setToken(const string token);

  private:
    //##ModelId=4855B612036B
    CdrExtractor *createExtractCdr(int aCdrType);

    //##ModelId=4855B612036C
    CdrExtractor *createExtractFileHead(int aCdrType);

    //##ModelId=4855B612036D
    CdrExtractor *createExtractFileTail(int aCdrType);

    //##ModelId=4855B6290280
    int getNextBlock();

    //##ModelId=4855B656032C
    // 针对摩托罗拉gprs文件头的特殊处理
    void dealMotoGprsFileHead(const unsigned char *FileBuffer);

    //##ModelId=4855B6AB0261
    // 针对CNG文件头的特殊处理
    void dealCngGgsnFileHead(const unsigned char *FileBuffer);

    //##ModelId=4855B6BD00FA
    // 针对华为软交换文件尾的特殊处理
    void dealHuawTail(const unsigned char *FileBuffer);

    // 针对华为TD交换机文件尾的特殊处理	added by zaiww 20090805
    void dealHuaweiTDFileTail(const unsigned char *FileBuffer);

  private:
    //##ModelId=4855B7030000
    Block m_theBlock;

    //##ModelId=4855B723001F
    CdrExtractor *m_theCdrExtractor;

    //##ModelId=4855B7230020
    CdrExtractor *m_theFileHeadExtractor;

    //##ModelId=4855B7230021
    CdrExtractor *m_theFileTailExtractor;

  private:
    //##ModelId=4855B731032C
    //##Documentation
    //## 记录块尺寸
    int m_BlockSize;

    //##ModelId=4855B73B033C
    //##Documentation
    //## 文件头尺寸
    int m_FileHeaderSize;

    //##ModelId=4855B7440157
    //##Documentation
    //## 当前记录块号
    int m_BlockNo;

    //##ModelId=4855B74F033C
    //##Documentation
    //## 文件尾尺寸
    int m_FileTailSize;

    //##ModelId=4855B75B0232
    //##Documentation
    //## 当前块缓存
    unsigned char *m_BlockBuffer;

    //##ModelId=4855B7690167
    //##Documentation
    //## 实际记录块长度
    int m_RealBlockSize;

    //##ModelId=4855B7720128
    //##Documentation
    //## 当前记录块偏移量
    int m_BufferOffset;

    //##ModelId=4855B77C007D
    //##Documentation
    //## 记录块总数
    int m_TotalBlocks;

    //##ModelId=4855B78401F4
    //##Documentation
    // 根据配置,显示是否为摩托罗拉的gprs话单
    bool m_isMotoGprs;

    //##ModelId=4855B78E0399
    //##Documentation
    // 根据配置,显示是否为CNG话单
    bool m_isCNGggsn;

    //##ModelId=4855B7970290
    //##Documentation
    // 根据配置，现实文件尾是否为华为软交换话单
    bool m_isHuawTail;

    //处理华为TD交换机话单文件尾	added by zaiww 20090805
    bool m_isHuawTDTail;

    //##ModelId=486AD69B00FA
    //##Documentation
    // CDMA话单块的不定长特殊性 1代表需要计算,0（默认）是数字
    int m_BlockFlag;

    //##ModelId=486AD69B00FB
    //##Documentation
    // 文件头是否需要解析
    bool m_isParseFileHead;

    //##ModelId=486AD69B00FC
    //##Documentation
    // 文件尾是否需要解析
    bool m_isParseFileTail;

    //##ModelId=486AD69B00FD
    //##Documentation
    // 文件头内容偏移量
    int m_FHBufferOffset;

    //##ModelId=486AD69B00FE
    //##Documentation
    // 文件尾内容偏移量
    int m_FTBufferOffset;

    //##ModelId=486AD69B00FF
    //##Documentation
    // 空文件是否告警
    bool m_AlertByEmptyFile;


};



#endif /* GENERALFILE_H_HEADER_INCLUDED_B7A65AC5 */

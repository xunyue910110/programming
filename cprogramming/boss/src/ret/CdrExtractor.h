// ##########################################
// Source file : CdrExtractor.h
// System      : Mobile Billing System
// Version     : 0.1.14
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010816
// Update      : 20020108
// Copyright(C): 2001 by Yang Zhang, Linkage.
// ##########################################

#ifndef EXTRACTCDR_H_INCLUDED_C48638EB
#define EXTRACTCDR_H_INCLUDED_C48638EB

#include "config-all.h"

#include <string>
#include "RetLog.h"
#include "FieldExtractRule.h"
#include "RecordFormat.h"
#include "AlertCode.h"

#include "appfrm/ErrorHandle.h"
extern ErrorMessages * const theErrorMessages;

USING_NAMESPACE(std)

//##ModelId=3B565FA502C1
//##Documentation
//## 通用话单记录处理抽象类
//##
class CdrExtractor
{
  public:
	//##ModelId=3C3A5AFF032A
    CdrExtractor();

	//##ModelId=3B84F1AF02DC
	virtual ~CdrExtractor();

	//##ModelId=3B4EB42E013E
	//##Documentation
	//## 该方法是纯虚方法，由其子类具体实现。
	virtual int getNextSrcCdr(unsigned char *&srcCdrBuffer) = 0;

	//##ModelId=3B846DDD0075
	virtual bool setBlockBuffer(const unsigned char *BlockBuffer,int RealBlockLength);

	//##ModelId=3B5799BA037C
	//##Documentation
	//## 用于初始化的方法。
	//##
	virtual bool initialize(const string theCdrParameter,const int theBlockHeaderSize) = 0;

	virtual void setToken(const string token){}
  public:

  protected:
	//##ModelId=3B846F5302C1
	//##Documentation
	//## 用于保存实际记录块的长度
	int m_RealBlockLength;

	//##ModelId=3B846F5200DF
	//##Documentation
	//## 用于保存话单记录块的缓存指针
	unsigned char *m_BlockBuffer;

	//##ModelId=3B84B5BA012D
	int m_BlockHeaderSize;

	//##ModelId=3B66B02201C3
	RecordFormat *theRecordFormat;

	//##ModelId=3B872344019B
	int m_BlockOffset;

	//##ModelId=3C3A5AA90308
   RetLog *m_RetLog_;

  private:

};



#endif /* EXTRACTCDR_H_INCLUDED_C48638EB */

// ##########################################
// Source file : RecordFormat.h
// System      : Mobile Billing System
// Version     : 0.1.7
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010823
// Update      : 20020529
// Copyright(C): 2002 by Yang Zhang, Linkage.
// ##########################################

#ifndef RECORDFORMAT_H_INCLUDED_C490203D
#define RECORDFORMAT_H_INCLUDED_C490203D

#include "config-all.h"

#include <string>
#include <vector>

#include "RetLog.h"
#include "AlertCode.h"

USING_NAMESPACE(std)

#include "appfrm/ErrorHandle.h"
extern ErrorMessages * const theErrorMessages;

//##ModelId=3B60290E0369
//##Documentation
//## 记录格式处理类，抽象公共方法类
//## 此类用于对原始话单记录进行解包运算，得到记录内所有的字段标识和字段值，用字段标识作为键值把字段值存入一个容器内，供进一步处理。
class RecordFormat {
  public:
	//##ModelId=3B94E33C02BA
	//##Documentation
	//## 对象类型(=0:基类对象；=1:AsnFormat对象;=2:TokenFormat对象)
  	RecordFormat();

	//##ModelId=3B94E33D0077
  	virtual ~RecordFormat();

	//##ModelId=3B60292C0380
	//##Documentation
	//## 得到记录全部字段的键值和值
	virtual void getContents() = 0;

	//##ModelId=3B84FD0D02B8
	//##Documentation
	//## 得到一条记录
	virtual int getCdrSize(const char *theSign,const char *cdrTmp,int &aCdrOffset) = 0;

	//##ModelId=3B95873402EB
	virtual int getValue(unsigned char * theSign,unsigned char *aValue,int position) = 0;

	//##ModelId=3B94E0D00345
	void reset(const unsigned char *aCdrBuffer,int cdrSize,int cdrOffset,int flag );

	//##ModelId=3B96E434037F
	virtual void initialize() = 0 ;
  public:
	//##ModelId=3C048D2F0387
	int getFormatType() const;

	//##ModelId=3B983762008B
	bool isUsed;

	//##ModelId=3B9972B10211
  	int m_subOffset;

	//##ModelId=3B94DE9F0208
	int m_bufferOffset;

	//##ModelId=3B95EB96024D
	int m_cdrSize;

  protected:
	//##ModelId=3B96FAF00374
  	RecordFormat *getNextPointer();

  protected:
	//##ModelId=3B96FAEE035D
  	static vector<RecordFormat *> m_RecordFormatPointer;

	//##ModelId=3B94E0AE036E
	const unsigned char *m_CdrBuffer;

	//##ModelId=3C048CB30389
	//##Documentation
   //## 对象类型(=0:基类对象；=1:AsnFormat对象;=2:TokenFormat对象)
	int m_FormatType;

	//##ModelId=3C3A703C004E
   RetLog *m_RetLog_;

};


#endif /* RECORDFORMAT_H_INCLUDED_C490203D */

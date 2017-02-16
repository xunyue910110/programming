// ##########################################
// Source file : AsnFormat.h
// System      : Mobile Billing System
// Version     : 0.1.9
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010823
// Update      : 20020529
// Copyright(C): 2002 by Yang Zhang, Linkage.
// ##########################################

#ifndef ASNFORMAT_H_INCLUDED_C490206F
#define ASNFORMAT_H_INCLUDED_C490206F

#include "RecordFormat.h"

//##ModelId=3B5153B60121
//##Documentation
//## 为纯方法类，通用类。
//## 包含ASN.1协议的解包算法。
class AsnFormat : public RecordFormat {
  public:

	//##ModelId=3B97195503D7
	AsnFormat();
	//##ModelId=3B978CA6010F
	virtual ~AsnFormat();
	//##ModelId=3B51546C03C1
	//##Documentation
	//## 使用ASN_1协议，对给定string求出全部的TAG和contents，通过一个map容器(使用TAG为键值)指针返回结果。本方法会解开全部的符合元素。返回元素个数。
	virtual void getContents();

	//##ModelId=3B84FD0C004A
	//##Documentation
	//## 使用ASN_1协议，根据给定的记录TAG标识，求出该记录的实际数据(存放在参数内)，
	//## 返回记录长度。
	virtual int getCdrSize(const char *theSign,const char *cdrTmp,int &aBlockOffset);

	//##ModelId=3B94E59900C6
	virtual int getValue(unsigned char * theSign,unsigned char *aValue,int position);

	//##ModelId=3B96E434014E
	virtual void initialize() ;

	//##ModelId=3B85155A0137
	//##Documentation
	//## 返回：ASN记录长度的字段数量。
	int getAsnLength(const unsigned char *lengthBuffer,int &aAsnCdrSize);

	//##ModelId=3C047E8700B1
	//##Documentation
	//## 返回：tag码长度
	int getAsnTag(const unsigned char *buffer,unsigned char *aTag);

  private:
  	int getZeroLen(const unsigned char * buffer);	

  private:
	//##ModelId=3B95E83C0203
	bool isCompositeTag(int tag);
};


#endif /* ASNFORMAT_H_INCLUDED_C490206F */

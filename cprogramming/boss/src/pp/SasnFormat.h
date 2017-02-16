// ##########################################
// Source file : SasnFormat.h
// System      : Mobile Billing System
// Version     : NG1.0
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Gao xile
// E_mail      : gaoxl@lianchuang.com
// Create      : 20080619
// Update      : 20080619
// Copyright(C): 2008 by Gao xile, Linkage.
// ##########################################

#ifndef SASNFORMAT_H_HEADER_INCLUDED_C490206F
#define SASNFORMAT_H_HEADER_INCLUDED_C490206F

#include "RecordFormat.h"

//##ModelId=3B5153B60121
//##Documentation
//## 为纯方法类,通用类  包含固网西门子无序变长格式的解包算法
class SasnFormat : public RecordFormat
{
  public:
	//##ModelId=3B97195503D7
	SasnFormat();

	//##ModelId=3B978CA6010F
	virtual ~SasnFormat();

	//##ModelId=3B51546C03C1
	//##Documentation
	//## 使用固网西门子无序变长格式对给定string求出全部的TAG和contents,通过一个map容器(使用TAG为键值)指针返回结果.
	//## 本方法会解开全部的符合元素.返回元素个数
	virtual void getContents();

	//##ModelId=3B84FD0C004A
	//##Documentation
	//## 使用固网西门子无序变长格式,根据给定的记录TAG标识,求出该记录的实际数据(存放在参数内)
	//## 返回记录长度
	virtual int getCdrSize(const char *theSign, const char *cdrTmp, int &aBlockOffset);

	//##ModelId=3B94E59900C6
	virtual int getValue(unsigned char * theSign,FieldInfo *pFieldInfo,int position, int theLength, bool &tagAll);

	//##ModelId=3B96E434014E
	virtual void initialize();

	//##ModelId=3B85155A0137
	//##Documentation
	//## 返回：SASN记录长度的字段数量
	int getAsnLength(const unsigned char *lengthBuffer, int &aAsnCdrSize, int &aValueSize);

	//##ModelId=3C047E8700B1
	//##Documentation
	//## 返回：tag码长度
	int getAsnTag(const unsigned char *buffer,unsigned char *aTag);

  private:
	//##ModelId=3B95E83C0203
	bool isCompositeTag(int tag);
};


#endif /* SASNFORMAT_H_HEADER_INCLUDED_C490206F */

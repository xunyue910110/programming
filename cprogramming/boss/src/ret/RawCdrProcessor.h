// ##########################################
// Source file : RawCdrProcessor.h
// System      : Mobile Billing System
// Version     : 0.1.21
// Language	   : ANSI C++
// OS Platform : HPUX,AIX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010816
// Update      : 20011127
// Copyright(C): 2001 by Yang Zhang, Linkage.
// ##########################################

#ifndef CDRPROCESS_H_INCLUDED_C4875408
#define CDRPROCESS_H_INCLUDED_C4875408

#include "base/supdebug.h"
#include "config-all.h"

#if defined(STDIOSTREAM_DISABLE)
#include <iostream.h>
#else
#include <iostream>
#endif

#include <string>
#include <vector>

#include "RetLog.h"
#include "FieldExtractRule.h"
#include "CdrFlag.h"
#include "FieldValidators.h"
#include "FieldInfo.h"
#include "FieldConvertors.h"
#include "RecordFormat.h"
#include "CompositeFieldExtractRule.h"
#include "DateTime.h"
#include "DataFormat.h"

#include "appfrm/ErrorHandle.h"
extern  ErrorMessages  *const theErrorMessages;

USING_NAMESPACE(std);

//##ModelId=3B4E937A028A
//##Documentation
//## 话单记录处理抽象类
//## 此类用于处理话单原始记录，处理结果为已经格式化的标准话单数据。
//## 该类支持固定格式、ASN变长格式、分隔符格式三种话单记录。
class RawCdrProcessor
{
  public:
	//##ModelId=3B6A0D9E0214
	virtual ~RawCdrProcessor();

	//##ModelId=3B948D41001B
	RawCdrProcessor();

	//##ModelId=3B4EB5EC00E5
	//##Documentation
	//## 按顺序调用所有字段对象里的校验方法。
	bool validCdr(multimap<MyString,FieldInfo *> &theFieldInfoMaps);

	//##ModelId=3BAF001901EA
	bool convertCdr(multimap<MyString,FieldInfo *> &theFieldInfoMaps);

	//##ModelId=3B57B810036F
	//##Documentation
	//## 按顺序调用所有字段对象里的相应方法，求得字段的标准格式数据
	virtual int getDesCdr(const unsigned char *aCdrValue,int length,
						  multimap<MyString,FieldInfo *> &m_theFieldInfoMaps) = 0 ;

	//##ModelId=3B60CE4B03AA
	//##Documentation
	//## 字段处理初始化方法，由主控对象(PreProcApplication)调用.
	virtual bool initialize( vector<string> aParameterList)  ;

	//##ModelId=3BAEFEF20326
	bool setRule(vector<string> aConvertRule,vector<string> aValidRule);

  public:
	//##ModelId=3B9F334F0289
	bool formatField(multimap<MyString,FieldInfo *> &theFieldInfoMaps);

	//##ModelId=3B9D92EA028A
	FieldExtractRule *getNextCompositeField(string &theCdrSign);

	//##ModelId=3C329EBB025D
	bool setToken(string token);

	//##ModelId=3B66B0B0016D
	RecordFormat *m_theRecordFormat;

	//##ModelId=3B7D12E7031C
	CompositeFieldExtractRule m_theCompositeField;

	//##ModelId=3BA16C460221
	DataFormat m_DataFormat;

	//##ModelId=3BAAB8E10011
	FieldValidators m_FieldValidators;

	//##ModelId=3BAAC11501E7
	FieldConvertors m_FieldConvertors;

  protected:
	//##ModelId=3B510B590029
	//##Documentation
	//## 话单记录原始数据
	//##
	const unsigned char * m_SrcCdrValue;

	//##ModelId=3BAEFEF200D8
	RetLog *m_RetLog_;

	//##ModelId=3BC16F420227
	CdrFlag *m_cdrFlag;


  private:
	//##ModelId=3BA6F2CB01FA
	bool convertDateTime(multimap<MyString,FieldInfo *> &theFieldInfoMaps)	;

	//##ModelId=3BA708560240
	int getFieldValue(multimap<MyString,FieldInfo *> &theFieldInfoMaps,int &size,
						unsigned char * const &FieldSrcValue,const string &fieldName);

	//##ModelId=3BA71AF10285
	void addFieldMap(multimap<MyString,FieldInfo *> &theFieldInfoMaps,char *fieldName,
					string fieldValue,int filenLength,const unsigned char *FieldSrc);

};

#endif /* CDRPROCESS_H_INCLUDED_C4875408 */

// ##########################################
// Source file : FixedRawCdrProcessor.h
// System      : Mobile Billing System
// Version     : 1.02
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010817
// Update      : 20010906
// Copyright(C): 2001 by Yang Zhang, Linkage.
// ##########################################

#ifndef FIXEDCDR_H_INCLUDED_C4831DE1
#define FIXEDCDR_H_INCLUDED_C4831DE1

#include "config-all.h"

#include <string>
#include "RawCdrProcessor.h"

USING_NAMESPACE(std)

//##ModelId=3B6020B401B6
//##Documentation
//## 定长格式话单记录处理类
class FixedRawCdrProcessor : public RawCdrProcessor
{
  public:
	//##ModelId=3B60212E0324
	virtual int getDesCdr(const unsigned char *aCdrValue,int length,
						  multimap<MyString,FieldInfo *> &theFieldInfoMaps) ;

	//##ModelId=3B96EBE4023F
	virtual bool initialize( vector<string> aParameterList)  ;

	//##ModelId=3B66B7490053
	int getCdrSign();
};



#endif /* FIXEDCDR_H_INCLUDED_C4831DE1 */

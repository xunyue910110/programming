// ##########################################
// Source file : FixedFieldExtractRule
// System      : Mobile Billing System
// Version     : 1.31
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010813
// Update      : 20010903
// Copyright(C): 2001 by Yang Zhang, Linkage.
// ##########################################

#ifndef FIXEDFIELD_H_INCLUDED_C48808B2
#define FIXEDFIELD_H_INCLUDED_C48808B2

#include "config-all.h"

#include <vector>
#include <string>
#include "FieldExtractRule.h"

USING_NAMESPACE(std)

//##ModelId=3B51386F00B1
//##Documentation
//## 定长字段提取方法类，继承自Field抽象对象。该对象用于从原始记录中提取字段。

class FixedFieldExtractRule : public FieldExtractRule 
{
	public:
		//##ModelId=3B7D257F0385
		virtual bool initialize(vector<string> *argname, FieldExtractRule *aParentsField, const int aFloor);
		
		//##ModelId=3B77E2AB03B1
		virtual bool extract(const unsigned char *buffer, int length,
					   	 multimap<MyString,FieldInfo *> &afieldMap,RecordFormat &aRecordFormat);
	
		//##ModelId=3B513D7903B0
		//##Documentation
		//## 字段开始字节
		int m_StartByte_;
		//##ModelId=3B513D8401B7
		//##Documentation
		//## 字段开始比特
		int m_StartBit_;
		//##ModelId=3B513D920257
		//##Documentation
		//## 字段长度（单位：比特）
		int m_BitLength_;
	private:
		// add by chenm 2005-3-7 
		// 为记录nortel交换机附加模块在话单中出现的位置增加
		int m_position;
};



#endif /* FIXEDFIELD_H_INCLUDED_C48808B2 */

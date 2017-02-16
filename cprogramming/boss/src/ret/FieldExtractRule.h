// ##########################################
// Source file : FieldExtractRule.h
// System      : Mobile Billing System
// Version     : 0.1.7
// Language	   : ANSI C++
// OS Platform : HPUX,AIX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010813
// Update      : 20020108
// Copyright(C): 2001 by Yang Zhang, Linkage.
// ##########################################

#ifndef FIELD_H_INCLUDED_C4883D9F
#define FIELD_H_INCLUDED_C4883D9F

#include "config-all.h"

#if defined(STDIOSTREAM_DISABLE)
#include <iostream.h>
#else
#include <iostream>
#endif

#include <vector>
#include <string>
#include <map>
#include "RetLog.h"
#include "FieldInfo.h"
#include "RecordFormat.h"
#include "DataFormat.h"

USING_NAMESPACE(std);

#include "appfrm/ErrorHandle.h"
extern ErrorMessages * const theErrorMessages;

//##ModelId=3B5103E80110
//##Documentation
//## Composite(组合)模型的抽象字段类。
class FieldExtractRule {
  public:
	//##ModelId=3B6215D8003B
	FieldExtractRule();

	//##ModelId=3B6215D80199
	virtual ~FieldExtractRule();

	//##ModelId=3B513F650258
	//##Documentation
	//## 用于把原始字段值转换为ASCII值。
	//##
	void getFmtValue();

	//##ModelId=3B55105901E3
	//##Documentation
	//## 对象里的校验方法由PPParameter根据校验参数进行初始化。此方法使用这些参数对本字段对象里的值进行校验。
	void checkField();

	//##ModelId=3B77E4DC01A5
	virtual bool initialize(vector<string> *argname, FieldExtractRule *aParentsField, const int aFloor) = 0;

	//##ModelId=3B77E4DD02E7
	virtual bool extract(const unsigned char *buffer, int length,
						 multimap<MyString,FieldInfo *> &afieldMap,RecordFormat &aRecordFormat) = 0;
	//##ModelId=3B9DB40F0082
	string getFieldName() const;

	//##ModelId=3B9DC7E1011F
	string getSectionName() const;

	//##ModelId=3B9DC7E20329
	int getLaye() const;

	//##ModelId=3B9DC8D7037B
	void setSectionName(string left);

	//##ModelId=3C048A91003A
	FieldExtractRule *getMySelf() const;

	//##ModelId=3B77E5990034
	//##Documentation
	//## 保存指向父字段的指针。
	FieldExtractRule *m_theParentsField_;

	//##ModelId=3B77E6CA0065
	//##Documentation
	//## 对象树的层数
	int m_theLayer_;

	//##ModelId=3B77E4DC0050
	//##Documentation
	//## 配置文件中用于生成字段对象的标识名称。
	string m_SectionName_;

	//##ModelId=3B513E450092
	//##Documentation
	//## 字段数据类型
	string m_dataType;

	//##ModelId=3C329E34010E
	//##Documentation
	//## 字段分隔符
	char m_token;

  protected:
	//##ModelId=3B7DC4910231
	RetLog *m_RetLog_;

	//##ModelId=3B510AC4006B
	//##Documentation
	//## 字段原始值
	string m_SrcValue_;

	//##ModelId=3B510ACB01A1
	//##Documentation
	//## 转换后的字段值
	string m_FmtValue_;

	//##ModelId=3B510ADC0282
	//##Documentation
	//## 字段名称（标识）
	string m_FieldName_;


	//##ModelId=3BA02FDC00E7
	//##Documentation
	//## BCD码填充字符
	char m_padding;

	//##ModelId=3BA0300200D7
	//##Documentation
	//## 二进制数据高低位交换标志
	//## =0 不交换; =1 交换
	int m_order;

	//##ModelId=3BD4F8FB0139
	FieldInfo *m_FieldInfo;

	//##ModelId=3C047E870257
	//##Documentation
	//## 指向composite自身处理对象的指针。
	FieldExtractRule *m_CompositeFieldSelf_;


};



#endif /* FIELD_H_INCLUDED_C4883D9F */

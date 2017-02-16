// ##########################################
// Source file : FieldInfo.h
// System      : Mobile Billing System
// Version     : 0.1.7
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010821
// Update      : 20011023
// Copyright(C): 2001 by Yang Zhang, Linkage.
// ##########################################

#ifndef FIELDINFO_H_INCLUDED_C4849BA6
#define FIELDINFO_H_INCLUDED_C4849BA6

#include "config-all.h"
#include "PublicFunction.h"
#include <string>

USING_NAMESPACE(std)

//##ModelId=3B7B37AC02DA
//##Documentation
//## 处理字段保存类
class FieldInfo {
  public:
	//##ModelId=3B9EC5A2025F
	//int reset();

	//##ModelId=3BD6296403BC
	FieldInfo(int iBufferSize);
	~FieldInfo()
	{
		delete m_FieldSrcValue;	
	}
  public:
   //--------------以下为话单字段原始值、标准数据类型值的属性----------------
	//##ModelId=3B7B37BE031C
	//##Documentation
	//## 字段名称
	string m_FieldName_;

	//##ModelId=3B7B37CB032F
	//##Documentation
	//## 经数据格式化后的字段值
	string m_FieldDesValue;

	//##ModelId=3B93084F02C3
	//##Documentation
	//## 字段原始值
	unsigned char *m_FieldSrcValue;
                                     
	//##ModelId=3B93084F02EB
	//##Documentation
	//## 字段原始值长度
	int m_FieldLength;

   //---------------------以下为字段处理结果的属性-----------------------------
	//##ModelId=3B9EC4D201B5
	//##Documentation
	//## 字段数据标准化标志
	//bool m_IsFormat;

	//##ModelId=3B9EC4D40064
	//##Documentation
	//## 字段校验标志
	//bool m_IsCheck;

	//##ModelId=3BCD5C080084
	//##Documentation
	//## 错误代码
	int m_errorCode;

   //---------------------以下为数据类型转换使用的属性-----------------------------
	//##ModelId=3B9EC4D400E6
	//##Documentation
	//## 字段数据类型，包括以下几种：
	//## EBCD,PBCD,TBCD,UBCD,Binary,HEX,Ascii。
	string m_dataType;

	//##ModelId=3BA02E3902BB
	//##Documentation
	//## 填充字符（如：'F'），由BCD数据格式处理使用。
	char m_padding;

	//##ModelId=3BA02E7E036E
	//##Documentation
	//## 二进制数据高低位交换标志（＝0，不交换；＝1，交换）
	int m_order;

};

#endif /* FIELDINFO_H_INCLUDED_C4849BA6 */

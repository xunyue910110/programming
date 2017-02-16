// ##########################################
// Source file : DataFormat.h
// System      : Move Billing System
// Version     : 0.1.6
// Language	   : ANSI C++
// OS Platform : HPUX,AIX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010912
// Update      : 20020606
// Copyright(C): 2002 by Yang Zhang, Linkage.
// ##########################################

#ifndef DATAFORMAT_H_INCLUDED_C460B68D
#define DATAFORMAT_H_INCLUDED_C460B68D
#include "config-all.h"

#if defined(STDIOSTREAM_DISABLE)
#include <iostream.h>
#include <fstream.h>
#else
#include <iostream>
#include <fstream>
#endif

#include <math.h>
#include <string>

#include <stdio.h>

USING_NAMESPACE(std);

//##ModelId=3B4E93BA02DC
//##Documentation
//## 方法类，其中包含了数据格式转换用到的全部方法。
//## 包括以下方法：四种BCD码的转换；二进制数据的转换；字符串的转换等。
//## 通用类。
class DataFormat {
  public:
	//##ModelId=3B515F6F0389
	//##Documentation
	//## 用于对各类BCD码的转换
	bool getFmtValue(
		//##Documentation
		//## 保存转换后的数据
		string &desValue,
		//##Documentation
		//## 原始数据缓存
		const unsigned char *drcValue,
		//##Documentation
		//## 原始数据长度
		const int srcLength,
		//##Documentation
		//## 填充字符
		const int padding,
		//##Documentation
		//## BCD类型，共有四种：EBCD,PBCD,TBCD,UBCD
		const string &bcdType);

	//##ModelId=3BA083C90203
	//##Documentation
	//## 用于对二进制数据的转换
	bool getFmtValue(
		//##Documentation
		//## 保存转换后的数据
		string &desValue,
		//##Documentation
		//## 原始数据缓存
		const unsigned char *srcValue,
		//##Documentation
		//## 原始数据长度
		const int srcLength,
		//##Documentation
		//## 高低位交换标志：=0 不交换；=1 交换。
		const int order);

	//##ModelId=3BA1AE11009A
	//##Documentation
	//## 用于对 HEX 及 Ascii 数据的转换
	bool getFmtValue(
		//##Documentation
		//## 保存转换后的数据
		string &desValue,
		//##Documentation
		//## 原始数据缓存
		const unsigned char *srcValue,
		//##Documentation
		//## 原始数据长度
		const int srcLength,
		//##Documentation
		//## 格式类型,(=0:hex; =1:ascii; =2:HexToDec)
		const char* formatType,
		//##Documentation
		//## 高低位交换标志：=0 不交换；=1 交换。
		const int order);
		
	// 功能：提供gprs话单中特殊的accessPointNameNI数据类型的格式转换
	bool getGprsAscii(string &desValue,const unsigned char *srcValue,
							const int srcLength);
	// 功能：提供gprs话单中ggsn文本格式的IP地址,转换成16进制的IP地址
	bool getGprsHexIp(string &desValue,const unsigned char *srcValue,
							const int srcLength);
							
	~DataFormat();

	//##ModelId=3B9F5D9D023C
	DataFormat();

  private:

	//##ModelId=3C39B80B0221
	//##Documentation
	//## 本方法用于根据ASCII码转换为字符。
    int tt(char x);

	//##ModelId=3BA0A45301E3
	bool extractValue(char *theDesValue,const unsigned char *theSrcValue,
					 const int theSrcLength);

	//##ModelId=3CFF0F2602B0
	void movePadding(char *thestring,int padding);
};

#endif /* DATAFORMAT_H_INCLUDED_C460B68D */



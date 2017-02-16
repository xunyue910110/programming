// ##########################################
// Source file : DataFormat.h
// System      : Mobile Billing System
// Version     : NG1.0
// Language    : ANSI C++
// OS Platform : HPUX
// Authors     : Gao xile
// E_mail      : gaoxl@lianchuang.com
// Create      : 20080619
// Update      : 20080619
// Copyright(C): 2008 by Gao xile, Linkage.
// ##########################################

#ifndef DATAFORMAT_H_HEADER_INCLUDED_B7A624FF
#define DATAFORMAT_H_HEADER_INCLUDED_B7A624FF

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

class FieldInfo;

//##ModelId=4851CF5003C8
//##Documentation
//## 方法类,其中包含了数据格式转换用到的全部方法
//## 包括以下方法：四种BCD码的转换  二进制数据的转换  字符串的转换等
//## 通用类
class DataFormat
{
  public:
    //##ModelId=4851CF7901E4
    DataFormat(){}

    //##ModelId=4851CF7D0186
    ~DataFormat(){}

	bool getFmtValue(FieldInfo *pFieldInfo);

  private:
    //##ModelId=4851CF90006D
    //##Documentation
    //## 用于对各类BCD码的转换
    bool getBCDValue(FieldInfo *pFieldInfo);

    //##ModelId=4851D02A01C5
    //##Documentation
    //## 用于对二进制数据的转换
    bool getBinaryValue(FieldInfo *pFieldInfo);

    //##ModelId=4851D08D02DE
    //##Documentation
    //## 用于对 HEX 及 Ascii 数据的转换
    bool getHexAsciiValue(FieldInfo *pFieldInfo,const char *formatType);

    //##ModelId=4851D0EF03B9
    //##Documentation
    //## 功能：提供gprs话单中特殊的accessPointNameNI数据类型的格式转换
    bool getGprsAscii(FieldInfo *pFieldInfo);

    //##ModelId=4851D12F00DA
    //##Documentation
    //## 功能：提供gprs话单中ggsn文本格式的IP地址,转换成16进制的IP地址
    bool getGprsHexIp(FieldInfo *pFieldInfo);

    //##ModelId=4851D1AB0242
    //##Documentation
    //## 功能：提供gprs话单中ggsn16进制的IP地址,转换成10进制的IP地址，并且加点"."
    bool getGprsDecIp(FieldInfo *pFieldInfo);

  private:
    //##ModelId=4851D1EA033C
    //##Documentation
    //## 本方法用于根据ASCII码转换为字符
    int tt(char x);

    //##ModelId=4851D2060128
    bool extractValue(char *theDesValue, const unsigned char *theSrcValue, const int theSrcLength);

    //##ModelId=4851D24903A9
    void movePadding(char *thestring, int padding);

    //##ModelId=4851D2C502BF
    int CharToBin(char *Source, char *Dest);

};



#endif /* DATAFORMAT_H_HEADER_INCLUDED_B7A624FF */

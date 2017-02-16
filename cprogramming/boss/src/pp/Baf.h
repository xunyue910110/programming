// ##########################################
// Source file : Baf.h
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

#ifndef BAF_H_HEADER_INCLUDED_B7A6424D
#define BAF_H_HEADER_INCLUDED_B7A6424D

#include "config-all.h"

#if defined(STDIOSTREAM_DISABLE)
#include <iostream.h>
#include <fstream.h>
#else
#include <iostream>
#include <fstream>
#endif

#include <map>
#include <string>
#include <vector>
#include <stdio.h>

USING_NAMESPACE(std);

//##ModelId=4855C54B0157
class Baf
{
  public:
    //##ModelId=4855C5790167
    //##Documentation
    //## 将配置文件中的BAF段压入容器baf_map中
    bool initialize(vector<string> aParameterList);

    //##ModelId=4855C5900203
    //##Documentation
    //## 根据传入的mod_code,得到其实际的起始位置
    int getLenth(const unsigned char *buffer, const int length, const string &s_FieldName,
                    const int i_StartByte, const int i_sequence, int &moduleLength);

  private:
    //##ModelId=4855C5600232
    map <string, int> m_bafMap;

};


#endif /* BAF_H_HEADER_INCLUDED_B7A6424D */

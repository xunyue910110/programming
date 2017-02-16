// ##########################################
// Source file : baf.h
// System      : Mobile Billing System
// Version     : 0.1.24
// Language    : ANSI C++
// OS Platform : ux
// Authors     : chenm
// E_mail      : chenm@lianchuang.com
// Create      : 20020412
// Update      :
// Copyright(C): 2002 by chenmin, Linkage.
// ##########################################

#ifndef BAF_H_INCLUDED_C345974E
#define BAF_H_INCLUDED_C345974E

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

USING_NAMESPACE(std)

//##ModelId=3CBA5DD103B8
class Baf
{
  public:
	//##ModelId=3CBA5ECF024B
	//##Documentation
	//## 将配置文件中的BAF段压入容器baf_map中
    bool initialize( vector<string> aParameterList);

	//##ModelId=3CBA5ECF02CD
	//##Documentation
	//## 根据传入的mod_code,得到其实际的其实位置
    int getLenth(const unsigned char *buffer,const int length,
                 const string &s_FieldName,const int i_StartByte,const int i_sequence);

  private:
	//##ModelId=3CBA5FEB01BD
    map <string,int> m_bafMap;

};


#endif /* BAF_H_INCLUDED_C345974E */

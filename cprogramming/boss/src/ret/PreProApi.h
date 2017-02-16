// ##########################################
// Source file : PreProApi.h
// System      : Mobile Billing System
// Version     : 0.0.1
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20011226
// Update      : 20011226
// Copyright(C): 2001 by Yang Zhang, Linkage.
// ##########################################

#ifndef PREPROAPI_H_INCLUDED_C3D619E6
#define PREPROAPI_H_INCLUDED_C3D619E6

#include "config-all.h"

#include <iostream>
#include <vector>
#include <map>

USING_NAMESPACE(std);

#include "FieldInfo.h"

//##ModelId=3C29851C01A1
//##Documentation
//## 预处理可编程类
class PreProApi
{
 public:
  //##ModelId=3C29B3ED0078
  bool processCdr(multimap<MyString,FieldInfo *> &theFieldInfoMaps,const int datasorce);

 private:

  //##ModelId=3C29DF1E0140
  FieldInfo *getField(multimap<MyString,FieldInfo *> &theFieldInfoMaps,string fieldName);

  //##ModelId=3C3D803400C5
  bool proc_0(multimap<MyString,FieldInfo *> &theFieldInfoMaps);

  //##ModelId=3C3D80340152
  bool proc_1(multimap<MyString,FieldInfo *> &theFieldInfoMaps);

  //##ModelId=3C3D803401AC
  bool proc_2(multimap<MyString,FieldInfo *> &theFieldInfoMaps);

  //##ModelId=3C3D80340238
  bool proc_3(multimap<MyString,FieldInfo *> &theFieldInfoMaps);

  //##ModelId=3C3D8034029C
  bool proc_4(multimap<MyString,FieldInfo *> &theFieldInfoMaps);


};

//##ModelId=3C29DF1E0140
inline FieldInfo *PreProApi::getField(multimap<MyString,FieldInfo *> &theFieldInfoMaps,string fieldName)
{
    multimap<MyString,FieldInfo *>::iterator itr;
    itr = theFieldInfoMaps.find(fieldName.c_str());      
    if( (itr != theFieldInfoMaps.end() )
             && ((itr->second)->m_FieldName_.compare(fieldName) == 0)
       )
    {
        return itr->second;
    }

	return 0;
}



#endif /* PREPROAPI_H_INCLUDED_C3D619E6 */

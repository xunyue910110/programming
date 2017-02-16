// ##########################################
// Source file : PreProApi.cpp
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

#include "PreProApi.h"

//##ModelId=3C29B3ED0078
bool PreProApi::processCdr(multimap<MyString,FieldInfo *> &theFieldInfoMaps,const int datasorce)
{
  switch (datasorce)
  {
  case 0:
    return proc_0(theFieldInfoMaps);
  case 1:
    return proc_1(theFieldInfoMaps);
  case 2:
    return proc_2(theFieldInfoMaps);
  case 3:
    return proc_3(theFieldInfoMaps);
  case 4:
    return proc_4(theFieldInfoMaps);
  default:
    return true;
  }
}

//##ModelId=3C3D803400C5
bool PreProApi::proc_0(multimap<MyString,FieldInfo *> &theFieldInfoMaps)    // 0 通道处理
{
  FieldInfo *FieldInfoPoint;

  FieldInfoPoint=getField(theFieldInfoMaps,"Module_name");

  if(FieldInfoPoint!=0) {
    FieldInfoPoint->m_FieldDesValue="南京联创";
  }

  return true;
}

//##ModelId=3C3D80340152
bool PreProApi::proc_1(multimap<MyString,FieldInfo *> &theFieldInfoMaps)    // 1 通道处理
{
  return true;
}

//##ModelId=3C3D803401AC
bool PreProApi::proc_2(multimap<MyString,FieldInfo *> &theFieldInfoMaps)    // 2 通道处理
{
  return true;
}

//##ModelId=3C3D80340238
bool PreProApi::proc_3(multimap<MyString,FieldInfo *> &theFieldInfoMaps)    // 3 通道处理
{
  return true;
}

//##ModelId=3C3D8034029C
bool PreProApi::proc_4(multimap<MyString,FieldInfo *> &theFieldInfoMaps)    // 4 通道处理
{
  return true;
}



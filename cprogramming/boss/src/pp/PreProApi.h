// ##########################################
// Source file : PreProApi.h
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

#ifndef PREPROAPI_H_HEADER_INCLUDED_B7A60C5A
#define PREPROAPI_H_HEADER_INCLUDED_B7A60C5A

#include "config-all.h"
#include "FieldInfo.h"

#include <iostream>
#include <vector>
#include <map>

USING_NAMESPACE(std);

//##ModelId=48571A2F00AB
//##Documentation
//## 预处理可编程类
class PreProApi
{
  public:
    //##ModelId=48571A4C0280
    bool processCdr(const int datasorce);

    //##ModelId=48571A8902AF
    bool proc_0();

    //##ModelId=48571A9F00CB
    bool proc_1();

    //##ModelId=48571AC40290
    bool proc_2();

    //##ModelId=48571ACC007D
    bool proc_3();

    //##ModelId=48571AD10232
    bool proc_4();



};



#endif /* PREPROAPI_H_HEADER_INCLUDED_B7A60C5A */

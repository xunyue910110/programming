// ##########################################
// Source file : PreProApi.cpp
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

#include "PreProApi.h"

//##ModelId=48571A4C0280
bool PreProApi::processCdr(const int datasorce)
{
    switch (datasorce)
    {
    case 0:
        return proc_0();
    case 1:
        return proc_1();
    case 2:
        return proc_2();
    case 3:
        return proc_3();
    case 4:
        return proc_4();
    default:
        return true;
    }
}

//##ModelId=48571A8902AF
bool PreProApi::proc_0()
{
    return true;
}

//##ModelId=48571A9F00CB
bool PreProApi::proc_1()
{
    return true;
}

//##ModelId=48571AC40290
bool PreProApi::proc_2()
{
    return true;
}

//##ModelId=48571ACC007D
bool PreProApi::proc_3()
{
    return true;
}

//##ModelId=48571AD10232
bool PreProApi::proc_4()
{
    return true;
}


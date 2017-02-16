
// ##########################################
// Source file : CdrFlag.h
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

#ifndef CDRFLAG_H_HEADER_INCLUDED_B7A66B8C
#define CDRFLAG_H_HEADER_INCLUDED_B7A66B8C

#include "config-all.h"
#include <string>

USING_NAMESPACE(std);

//##ModelId=4855C44E038A
//##Documentation
//## 话单记录标志类,静态全局对象
class CdrFlag
{
  public:
    //##ModelId=4855C4750242
    ~CdrFlag(){}

    //##ModelId=4855C48703D8
    void clear();

    //##ModelId=4855C4990109
    static CdrFlag *getInstance();

    //##ModelId=4855C4B90128
    int m_errorCode;

    //##ModelId=4855C4C103A9
    bool m_isInvalid;

    //##ModelId=4855C4CA02DE
    bool m_isError;

    //##ModelId=4855C4D703D8
    //##Documentation
    //## 呼叫类型(两位字符)
    char m_callType[4];

    //##ModelId=4855C4E00109
    int m_cdrNo;

    //##ModelId=4855C4EF01E4
    string m_fieldName;

    //##ModelId=4855C4FA00EA
    bool m_errorFlag;
    
    int m_cdrFee; //总话费， add by lizhao 2008-10-07
    
    
    string m_FHT_errorcode;     //add by lizhao 2008-11-03
    
    //added by zaiww 20090819
	void resetCdrNo();

  private:
    //##ModelId=4855C50C0177
    static CdrFlag *m_cdrFlag;

    //##ModelId=4855C46F00CB
    CdrFlag();


};



#endif /* CDRFLAG_H_HEADER_INCLUDED_B7A66B8C */

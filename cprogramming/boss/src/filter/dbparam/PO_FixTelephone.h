/******************************************************************
Copyright    : NanJing Linkage System Integration Co.,Ltd 
File name    : PO_FixTelephone.h
Author       : 联创BOSS计费项目组
Version      : 2.0
Date         : 2002.05 
Description  : 
Others       : 
Function List: 
History      : 
******************************************************************/

#ifndef PO_FIXTELEPHONE_H
#define PO_FIXTELEPHONE_H

#include "dbparam/PO_Common.h"
#include "FieldsXX.h"

class PO_FixTelephone
{
  public:
    bool getProperty(PO_Property &property);    
    bool convert(const StringVector &argname);    
    
    PO_FixTelephone(){}    
    PO_FixTelephone(const PO_FixTelephone& right);    
    
    void operator=(const PO_FixTelephone& right);    
    
    friend int operator==(const PO_FixTelephone &left,
                          const PO_FixTelephone& right);    
    
    friend int operator<(const PO_FixTelephone &left,
                         const PO_FixTelephone& right);
    
    friend ostream& operator<<(ostream& os,const PO_FixTelephone& po);
    
    struct
    {
      T_MSISDN      m_otherParty;
      T_USER_TYPE   m_userType;
      T_DATE_TIME   m_beginTime;
      T_DATE_TIME   m_endTime;
      T_CALLED_TYPE m_calledType;
    }recordStruct;
      
  private:
    static PO_Property m_property;
};

#endif 

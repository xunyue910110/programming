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

#ifndef PO_PROVFIXTELEPHONE_H
#define PO_PROVFIXTELEPHONE_H

#include "dbparam/PO_Common.h"
#include "FieldsXX.h"

class PO_ProvFixTelephone
{
  public:
    bool getProperty(PO_Property &property);    
    bool convert(const StringVector &argname);    
    
    PO_ProvFixTelephone(){}    
    PO_ProvFixTelephone(const PO_ProvFixTelephone& right);    
    
    void operator=(const PO_ProvFixTelephone& right);    
    
    friend int operator==(const PO_ProvFixTelephone &left,
                          const PO_ProvFixTelephone& right);    
    
    friend int operator<(const PO_ProvFixTelephone &left,
                         const PO_ProvFixTelephone& right);
    
    friend ostream& operator<<(ostream& os,const PO_ProvFixTelephone& po);
    
    struct
    {
      T_MSISDN      m_otherParty;
      T_DATE_TIME   m_beginTime;
      T_DATE_TIME   m_endTime;
    }recordStruct;
      
  private:
    static PO_Property m_property;
};

#endif 

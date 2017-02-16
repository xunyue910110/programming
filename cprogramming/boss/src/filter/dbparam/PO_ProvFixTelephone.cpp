/******************************************************************
Copyright    : NanJing Linkage System Integration Co.,Ltd 
File name    : PO_FixTelephone.cpp
Author       : 联创BOSS计费项目组
Version      : 2.0
Date         : 2002.05 
Description  : 
Others       : 
Function List: 
History      : 
******************************************************************/

#include "PO_ProvFixTelephone.h"

bool PO_ProvFixTelephone::getProperty(PO_Property &property)
{
    property=m_property;
    return true;
}

bool PO_ProvFixTelephone::convert(const StringVector &argname)
{
    int i=0;
    string tmpstr;
 
    tmpstr=*(argname.begin()+i++);                                  
    tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));           
    tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
    recordStruct.m_otherParty=tmpstr;                                  
    
    tmpstr=*(argname.begin()+i++);                                 
    tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));           
    tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
    recordStruct.m_beginTime=tmpstr;                               

    tmpstr=*(argname.begin()+i++);                                 
    tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));           
    tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
    recordStruct.m_endTime=tmpstr;                 

    return true;
}

PO_ProvFixTelephone::PO_ProvFixTelephone(const PO_ProvFixTelephone& right)
{
    *this=right;
}

void PO_ProvFixTelephone::operator=(const PO_ProvFixTelephone& right)
{
    recordStruct=right.recordStruct;
}

int operator==(const PO_ProvFixTelephone &left,const PO_ProvFixTelephone& right)
{
    return (strcmp(right.recordStruct.m_otherParty.c_str(),left.recordStruct.m_otherParty.c_str()) == 0);
}

int operator<(const PO_ProvFixTelephone &left,const PO_ProvFixTelephone& right)
{
    return (strcmp(right.recordStruct.m_otherParty.c_str(),left.recordStruct.m_otherParty.c_str()) < 0);
}

ostream& operator<<(ostream& os,const PO_ProvFixTelephone& po)
{
     os<<po.recordStruct.m_otherParty<<" , "<<
         po.recordStruct.m_beginTime<<" , "<<
         po.recordStruct.m_endTime;
    return os;
}


PO_Property PO_ProvFixTelephone::m_property={FROM_DATABASE,BY_FLAG,
    "","PO_ProvFixTelephone",USERNAME_1,PASSWORD_1,SERVNAME_1,
    "select OTHER_PARTY,BEGIN_TIME,END_TIME FROM TD_PROV_FIX_TELEPHONE",
    3,"select count(*) from TD_PROV_FIX_TELEPHONE"};

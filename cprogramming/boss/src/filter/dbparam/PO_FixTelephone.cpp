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

#include "PO_FixTelephone.h"

bool PO_FixTelephone::getProperty(PO_Property &property)
{
    property=m_property;
    return true;
}

bool PO_FixTelephone::convert(const StringVector &argname)
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
    recordStruct.m_userType=tmpstr;
                                           
    tmpstr=*(argname.begin()+i++);                                 
    tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));           
    tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
    recordStruct.m_beginTime=tmpstr;                               

    tmpstr=*(argname.begin()+i++);                                 
    tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));           
    tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
    recordStruct.m_endTime=tmpstr;                                 
                                                                   
    tmpstr=*(argname.begin()+i++);                                 
    tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));           
    tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
    recordStruct.m_calledType=tmpstr;                              

    return true;
}

PO_FixTelephone::PO_FixTelephone(const PO_FixTelephone& right)
{
    *this=right;
}

void PO_FixTelephone::operator=(const PO_FixTelephone& right)
{
    recordStruct=right.recordStruct;
}

int operator==(const PO_FixTelephone &left,const PO_FixTelephone& right)
{
    //return (strncmp(right.recordStruct.m_otherParty.c_str(),left.recordStruct.m_otherParty.c_str(),left.recordStruct.m_otherParty.length())==0);
    return (strcmp(right.recordStruct.m_otherParty.c_str(),left.recordStruct.m_otherParty.c_str()) == 0);
}

int operator<(const PO_FixTelephone &left,const PO_FixTelephone& right)
{
    //return (strncmp(right.recordStruct.m_otherParty.c_str(),left.recordStruct.m_otherParty.c_str(),left.recordStruct.m_otherParty.length())<0);
    return (strcmp(right.recordStruct.m_otherParty.c_str(),left.recordStruct.m_otherParty.c_str()) < 0);
}

ostream& operator<<(ostream& os,const PO_FixTelephone& po)
{
     os<<po.recordStruct.m_otherParty<<" , "<<
         po.recordStruct.m_userType<<" , "<<
         po.recordStruct.m_beginTime<<" , "<<
         po.recordStruct.m_endTime  <<" , "<<
         po.recordStruct.m_calledType;
    return os;
}


PO_Property PO_FixTelephone::m_property={FROM_DATABASE,BY_FLAG,
    "","PO_FixTelephone",USERNAME_1,PASSWORD_1,SERVNAME_1,
    "select OTHER_PARTY,ASP||SERV_TYPE||HOME_TYPE,BEGIN_TIME,END_TIME,CALLED_TYPE FROM TD_FIX_TELEPHONE",
    5,"select count(*) from TD_FIX_TELEPHONE"};

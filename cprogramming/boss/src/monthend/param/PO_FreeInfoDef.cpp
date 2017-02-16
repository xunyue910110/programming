#include "PO_FreeInfoDef.h"


bool PO_FreeInfoDef::getProperty(PO_Property &property)
{
    property = m_property;
    return true;
}


bool PO_FreeInfoDef::convert(const StringVector &argname)
{
    int i=0;
    string tmpstr;

    tmpstr=*(argname.begin()+i++);
    tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
    tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
    m_record.m_selectSql=tmpstr;
    
    tmpstr=*(argname.begin()+i++);
    tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
    tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
    m_record.m_startMonth=atoi(tmpstr.c_str());
    
    tmpstr=*(argname.begin()+i++);
    tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
    tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
    m_record.m_endMonth=atoi(tmpstr.c_str()); 
            
    return true;
}


PO_FreeInfoDef::PO_FreeInfoDef(const PO_FreeInfoDef& right)
{
    *this = right;
}


void PO_FreeInfoDef::operator = (const PO_FreeInfoDef& right)
{
    m_record = right.m_record;
}


int operator == (const PO_FreeInfoDef &left,const PO_FreeInfoDef& right)
{
    return (left.m_record.m_selectSql == right.m_record.m_selectSql);
}


int operator < (const PO_FreeInfoDef &left,const PO_FreeInfoDef& right)
{
    return (left.m_record.m_selectSql < right.m_record.m_selectSql);
}


ostream& operator << (ostream& os,const PO_FreeInfoDef& po)
{
       os << po.m_record.m_selectSql <<" , "<<
            po.m_record.m_startMonth <<" , "<<
            po.m_record.m_endMonth;  

    return os;
}


PO_Property PO_FreeInfoDef::m_property={FROM_DATABASE,BY_FLAG,
    "","PO_FreeInfoDef","","","",
    "select SEL_SQL,START_CYCLE_ID,END_CYCLE_ID FROM TD_B_FREE_INFODEF",
    3,"SELECT count(*) FROM TD_B_FREE_INFODEF"};


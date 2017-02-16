#include "PO_SpRelaDiscntDetail.h"

bool PO_SpRelaDiscntDetail::getProperty(PO_Property &property)
{
    property = m_property;
    return true;
}

bool PO_SpRelaDiscntDetail::convert(const StringVector &argname)
{
    int i = 0;
    string tmpstr;

    tmpstr=*(argname.begin()+i++);
    tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
    tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
    m_record.m_discntId=atoi(tmpstr.c_str());

    tmpstr=*(argname.begin()+i++);
    tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
    tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
    m_record.m_priority=atoi(tmpstr.c_str());

    tmpstr=*(argname.begin()+i++);
    tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
    tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
    m_record.m_itemCode=atoi(tmpstr.c_str());

    tmpstr=*(argname.begin()+i++);
    tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
    tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
    m_record.m_spCode=tmpstr;

    tmpstr=*(argname.begin()+i++);
    tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
    tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
    m_record.m_spBizCode=tmpstr;

    return true;
}


PO_SpRelaDiscntDetail::PO_SpRelaDiscntDetail(const PO_SpRelaDiscntDetail& right)
{
    *this = right;
}


void PO_SpRelaDiscntDetail::operator=(const PO_SpRelaDiscntDetail& right)
{
    m_record = right.m_record;
}


int operator==(const PO_SpRelaDiscntDetail &left,const PO_SpRelaDiscntDetail& right)
{
    return (left.m_record.m_discntId == right.m_record.m_discntId);
}


int operator<(const PO_SpRelaDiscntDetail &left,const PO_SpRelaDiscntDetail& right)
{
    if (left.m_record.m_discntId == right.m_record.m_discntId)
    {
        return (left.m_record.m_priority < right.m_record.m_priority);
    }
    else
    {
        return (left.m_record.m_discntId < right.m_record.m_discntId);
    }
}


ostream& operator<<(ostream& os,const PO_SpRelaDiscntDetail& po)
{
    os  <<  po.m_record.m_discntId        <<" , "<<
            po.m_record.m_priority <<" , "<<
            po.m_record.m_itemCode     <<" , "<<
            po.m_record.m_spCode       <<" , "<<
            po.m_record.m_spBizCode         ;

    return os;
}


PO_Property PO_SpRelaDiscntDetail::m_property={FROM_DATABASE,BY_FLAG,
    "","PO_SpRelaDiscntDetail","","","",
    "SELECT FEEPOLICY_ID,PRIORITY,ITEM_CODE,   \
            SP_CODE,SP_BIZ_CODE     \
            FROM TD_A_SP_RELDISC_DETAIL",
    5,"SELECT count(*) FROM TD_A_SP_RELDISC_DETAIL"};

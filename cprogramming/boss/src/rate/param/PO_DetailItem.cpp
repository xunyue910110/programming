#include "PO_DetailItem.h"


bool PO_DetailItem::getProperty(PO_Property &property)
{
    property = m_property;
    return true;
}


bool PO_DetailItem::convert(const StringVector &argname)
{
    int i=0;
    string tmpstr;

    tmpstr=*(argname.begin()+i++);
    tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
    tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
    m_record.m_itemId=atoi(tmpstr.c_str());

    tmpstr=*(argname.begin()+i++);
    tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
    tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
    m_record.m_itemUseType=tmpstr[0];
    
    tmpstr=*(argname.begin()+i++);
    tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
    tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
    m_record.m_addUpElemType=tmpstr[0];
   
    tmpstr=*(argname.begin()+i++);
    tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
    tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
    m_record.m_itemClass=tmpstr[0];

    tmpstr=*(argname.begin()+i++);
    tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
    tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
    m_record.m_oweTag=tmpstr[0];
    
    tmpstr=*(argname.begin()+i++);
    tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
    tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
    m_record.m_lateFeeCalcTag=tmpstr[0];
    
    tmpstr=*(argname.begin()+i++);
    tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
    tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
    m_record.m_carryMode=tmpstr[0];        
    return true;
}


PO_DetailItem::PO_DetailItem(const PO_DetailItem& right)
{
    *this = right;
}


void PO_DetailItem::operator = (const PO_DetailItem& right)
{
    m_record = right.m_record;
}


int operator == (const PO_DetailItem &left,const PO_DetailItem& right)
{
    return (left.m_record.m_itemId == right.m_record.m_itemId);
}


int operator < (const PO_DetailItem &left,const PO_DetailItem& right)
{
    return (left.m_record.m_itemId < right.m_record.m_itemId);
}


ostream& operator << (ostream& os,const PO_DetailItem& po)
{
       os << po.m_record.m_itemId <<" , "<<
            po.m_record.m_itemUseType <<" , "<<
            po.m_record.m_addUpElemType <<" , "<<
            po.m_record.m_itemClass <<" , "<<
            po.m_record.m_oweTag <<" , "<<
            po.m_record.m_lateFeeCalcTag <<" , "<<
            po.m_record.m_carryMode;    
         
    return os;
}


PO_Property PO_DetailItem::m_property={FROM_DATABASE,BY_FLAG,
    "","PO_DetailItem","","","",
    "select ITEM_ID,ITEM_USE_TYPE,ADDUP_ELEM_TYPE,ITEM_CLASS,OWE_TAG,\
     LATEFEE_CALC_TAG,CARRY_MODE FROM TD_B_DETAILITEM",
    7,"SELECT count(*) FROM TD_B_DETAILITEM"};


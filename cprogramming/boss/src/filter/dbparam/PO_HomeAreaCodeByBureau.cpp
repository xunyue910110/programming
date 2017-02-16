#include "PO_HomeAreaCodeByBureau.h"

bool PO_HomeAreaCodeByBureau::getProperty(PO_Property& property)
{
    property = m_property;
    return true;
}


bool PO_HomeAreaCodeByBureau::convert(const StringVector& argname)
{
    int i = 0;
    string tmpstr;
    
    tmpstr = *(argname.begin() + i++);
    tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
    tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r") + 1, tmpstr.npos);
    recordStruct.m_bureau = tmpstr;
    
    tmpstr = *(argname.begin() + i++);
    tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
    tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r") + 1, tmpstr.npos);
    recordStruct.m_feeCode = tmpstr;

    tmpstr = *(argname.begin() + i++);
    tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
    tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r") + 1, tmpstr.npos);
    recordStruct.m_beginTime = tmpstr;
    
    tmpstr = *(argname.begin() + i++);
    tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
    tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r") + 1, tmpstr.npos);
    recordStruct.m_endTime = tmpstr;
    
    return true;
}

PO_HomeAreaCodeByBureau::PO_HomeAreaCodeByBureau(const PO_HomeAreaCodeByBureau& right)
{
    *this = right;
}

void PO_HomeAreaCodeByBureau::operator=(const PO_HomeAreaCodeByBureau& right)
{
    recordStruct = right.recordStruct;
}

int operator==(const PO_HomeAreaCodeByBureau& left, const PO_HomeAreaCodeByBureau& right)
{
    return ((left.recordStruct.m_bureau == right.recordStruct.m_bureau) 
		&& (left.recordStruct.m_feeCode == right.recordStruct.m_feeCode));
}

int operator<(const PO_HomeAreaCodeByBureau &left, const PO_HomeAreaCodeByBureau& right)
{
    if (left.recordStruct.m_bureau < right.recordStruct.m_bureau)
    {
        return true;
    }
    else if (left.recordStruct.m_bureau > right.recordStruct.m_bureau)
    {
        return false;
    }
	
    if (left.recordStruct.m_feeCode < right.recordStruct.m_feeCode)
    {
        return true;
    }
    else if (left.recordStruct.m_feeCode > right.recordStruct.m_feeCode)
    {
        return false;
    }
    return false;
}

ostream& operator<<(ostream& os, const PO_HomeAreaCodeByBureau& po)
{
    os << po.recordStruct.m_bureau << " , " 
	  << po.recordStruct.m_feeCode << " , " 
	  << po.recordStruct.m_beginTime << " , " 
	  << po.recordStruct.m_endTime;
    return os;
}


PO_Property PO_HomeAreaCodeByBureau::m_property={FROM_DATABASE,BY_FLAG,
    "","PO_HomeAreaCodeByBureau",USERNAME_1,PASSWORD_1,SERVNAME_1,
    "select BUREAU, REAL_AREACODE, BEGIN_TIME, END_TIME from TD_SPECIAL_DIRECTION",
    4,"select count(*) from TD_SPECIAL_DIRECTION"};

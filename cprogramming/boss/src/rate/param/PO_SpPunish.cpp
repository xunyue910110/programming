#include "PO_SpPunish.h"


bool PO_SpPunish::getProperty(PO_Property &property)
{
	property = m_property;
	return true;
}
    

bool PO_SpPunish::convert(const StringVector &argname)
{
	int i = 0;
	string tmpstr;

	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_spCode = tmpstr;
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_spBizCode = tmpstr;	
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_punishType = tmpstr;
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_beginTime = tmpstr;
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_endTime = tmpstr;
				
	return true;
}


PO_SpPunish::PO_SpPunish(const PO_SpPunish& right)
{
	*this = right;
}


void PO_SpPunish::operator=(const PO_SpPunish& right)
{
	m_record = right.m_record;
}


int operator==(const PO_SpPunish &left,const PO_SpPunish& right)
{
    return (left.m_record.m_spCode == right.m_record.m_spCode && 
    				left.m_record.m_spBizCode == right.m_record.m_spBizCode	);
}


int operator<(const PO_SpPunish &left,const PO_SpPunish& right)
{    
  	if(left.m_record.m_spCode == right.m_record.m_spCode)
   	{
    		return (left.m_record.m_spBizCode < right.m_record.m_spBizCode);
   	}
   	else
   	{
       	return (left.m_record.m_spCode < right.m_record.m_spCode);
    }
}


ostream& operator<<(ostream& os,const PO_SpPunish& po)
{
	os<<po.m_record.m_spCode          <<" , "<<			
	    po.m_record.m_spBizCode     <<" , "<<
		  po.m_record.m_punishType    <<" , "<<
		  po.m_record.m_beginTime     <<" , "<<
		  po.m_record.m_endTime;
	return os;
}


PO_Property PO_SpPunish::m_property={FROM_DATABASE,BY_FLAG,
    "","PO_SpPunish","","","",
    "SELECT SP_CODE,decode(IS_ALL_SP,'1','0',SP_BIZ_CODE),PUNISH_TYPE, \
    to_char(BEGIN_TIME,'yyyymmddhh24miss'),\
    to_char(END_TIME,'yyyymmddhh24miss') \
    FROM TD_A_SPPUNISH",
    5,"SELECT count(*) FROM TD_A_SPPUNISH"};


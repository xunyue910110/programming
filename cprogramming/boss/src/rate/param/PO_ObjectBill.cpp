#include "PO_ObjectBill.h"
#include "RateConstDef.h"


bool PO_ObjectBill::getProperty(PO_Property &property)
{
	property = m_property;
	return true;
}


bool PO_ObjectBill::convert(const StringVector &argname)
{
	int i = 0;
	string tmpstr;

	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_id = atoi(tmpstr.c_str());
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	if(tmpstr.size() < 4)
	{
		m_record.m_roleCode = atoi(tmpstr.c_str());
	}
	else
	{
		strncpy(m_record.m_roleCodes,tmpstr.c_str(),STR_ROLECODE_ENUMLEN);
		m_record.m_roleCodes[STR_ROLECODE_ENUMLEN] = '\0';
		m_record.m_roleCode =ROLECODE_ENUM;
		m_record.splitEnumValue((const char*)m_record.m_roleCodes, 
								m_record.m_roleCode,
	                            m_record.m_roleCodeVector, 
	                            m_record.m_roleCodeMap);
	}
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_attrType = tmpstr[0];
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_attrId = atoi(tmpstr.c_str());
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_attrSource = tmpstr[0];

	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_condId = atoi(tmpstr.c_str());
	if(m_record.m_condId != 0)
	{
		if(m_record.m_roleCode == ROLECODE_ENUM)
		{m_record.m_roleCode =ROLECODE_ENUMCONDID;}
		else
		{m_record.m_roleCode =ROLECODE_CONDID;}
	}
		
	m_record.m_itemType = '1';
	return true;
}


PO_ObjectBill::PO_ObjectBill(const PO_ObjectBill &right)
{
	*this = right;
}


void PO_ObjectBill::operator = (const PO_ObjectBill &right)
{
	m_record = right.m_record;
}


int operator == (const PO_ObjectBill &left,const PO_ObjectBill& right)
{
    return (left.m_record == right.m_record);
}


int operator < (const PO_ObjectBill &left,const PO_ObjectBill& right)
{
    return (left.m_record < right.m_record);
}


ostream& operator << (ostream& os,const PO_ObjectBill& po)
{
	//os<< po.m_record<<endl; 

	return os;
}


PO_Property PO_ObjectBill::m_property = {FROM_DATABASE,BY_FLAG,
    "","PO_ObjectBill","","","",
    "SELECT OBJECT_ID,ROLE_ID,ATTR_TYPE,ATTR_ID,VALUE_SRC,COND_ID "
    "FROM TD_B_OBJECT WHERE ATTR_TYPE = '0'",6,
    "SELECT count(*) FROM TD_B_OBJECT WHERE ATTR_TYPE = '0'"};

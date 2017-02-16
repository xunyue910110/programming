#include "PO_PriceParamImpl.h"


bool PO_PriceParamImpl::getProperty(PO_Property &property)
{
	property = m_property;
	return true;
}


bool PO_PriceParamImpl::convert(const StringVector &argname)
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
	m_record.m_priceParamId = atoi(tmpstr.c_str());

	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_implMethod = tmpstr[0];

	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_paramValueType = tmpstr[0];
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_feePolicyParamId = atoi(tmpstr.c_str());
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	strncpy(m_record.m_fixedValue,tmpstr.c_str(),20);
	m_record.m_fixedValue[20] = '\0';	
	
	m_record.m_next = 0;
			
	return true;
}


PO_PriceParamImpl::PO_PriceParamImpl(const PO_PriceParamImpl& right)
{
	*this = right;
}


void PO_PriceParamImpl::operator = (const PO_PriceParamImpl& right)
{
	m_record = right.m_record;
}


int operator == (const PO_PriceParamImpl &left,const PO_PriceParamImpl& right)
{
    return (left.m_record == right.m_record);
}


int operator < (const PO_PriceParamImpl &left,const PO_PriceParamImpl& right)
{
    return (left.m_record < right.m_record);
}


ostream& operator << (ostream& os,const PO_PriceParamImpl& po)
{
	//os<< po.m_record<<endl; 

	return os;
}


PO_Property PO_PriceParamImpl::m_property = {FROM_DATABASE,BY_FLAG,
    "","PO_PriceParamImpl","","","",
    "SELECT FEEPOLICY_COMP_ID,PRICE_PARAM_ID,IMPL_METHOD,\
    PARAM_VALUE_TYPE,FEEPOLICY_PARAM_ID,FIXED_VALUE FROM TD_B_PRICE_PARAMIMPL",
    6,"SELECT count(*) FROM TD_B_PRICE_PARAMIMPL"};


#include "PO_BaseTpGroup.h"

bool PO_BaseTpGroup::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}

bool PO_BaseTpGroup::convert(const StringVector &argname)
{
	int i=0;
	string tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_tpId=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_beginTime=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_endTime=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_bizType=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_homeAreaCode=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_callType=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_roamType=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_servType=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_carrierType=tmpstr;

    tmpstr=*(argname.begin()+i++);
    tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
    tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
    m_record.m_ruleId=tmpstr;	
	m_record.m_condRule=NULL;
	
	//add by xuf 2009-11-7 10:06:31
	tmpstr=*(argname.begin()+i++);
    tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
    tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
    m_record.m_effectBizTag=tmpstr;
  
	return true;
}

PO_BaseTpGroup::PO_BaseTpGroup(const PO_BaseTpGroup& right)
{
	*this=right;
}

void PO_BaseTpGroup::operator=(const PO_BaseTpGroup& right)
{
	m_record=right.m_record;
}

int operator==(const PO_BaseTpGroup &left,const PO_BaseTpGroup& right)
{
	return (((left.m_record.m_homeAreaCode == right.m_record.m_homeAreaCode) || (left.m_record.m_homeAreaCode == "*") || (right.m_record.m_homeAreaCode == "*")) &&
			((left.m_record.m_callType == right.m_record.m_callType) || (left.m_record.m_callType == "*") || (right.m_record.m_callType == "*")) &&
			((left.m_record.m_roamType == right.m_record.m_roamType) || (left.m_record.m_roamType == "*") || (right.m_record.m_roamType == "*")) &&
			((left.m_record.m_servType == right.m_record.m_servType) || (left.m_record.m_servType == "*") || (right.m_record.m_servType == "*")) &&
			((left.m_record.m_carrierType == right.m_record.m_carrierType) || (left.m_record.m_carrierType == "*") || (right.m_record.m_carrierType == "*")));
}

int operator<(const PO_BaseTpGroup &left,const PO_BaseTpGroup& right)
{
	if ((left.m_record.m_homeAreaCode != "*") && (right.m_record.m_homeAreaCode != "*"))
	{
		if (left.m_record.m_homeAreaCode < right.m_record.m_homeAreaCode)
		{
			return true;
		}
		if (left.m_record.m_homeAreaCode > right.m_record.m_homeAreaCode)
		{
			return false;
		}
	}
	if ((left.m_record.m_callType != "*") && (right.m_record.m_callType != "*"))
	{
		if (left.m_record.m_callType < right.m_record.m_callType)
		{
			return true;
		}
		if (left.m_record.m_callType > right.m_record.m_callType)
		{
			return false;
		}
	}
	if ((left.m_record.m_roamType != "*") && (right.m_record.m_roamType != "*"))
	{
		if (left.m_record.m_roamType < right.m_record.m_roamType)
		{
			return true;
		}
		if (left.m_record.m_roamType > right.m_record.m_roamType)
		{
			return false;
		}
	}
	if ((left.m_record.m_servType != "*") && (right.m_record.m_servType != "*"))
	{
		if (left.m_record.m_servType < right.m_record.m_servType)
		{
			return true;
		}
		if (left.m_record.m_servType > right.m_record.m_servType)
		{
			return false;
		}
	}
	if ((left.m_record.m_carrierType != "*") && (right.m_record.m_carrierType != "*"))
	{
		if (left.m_record.m_carrierType < right.m_record.m_carrierType)
		{
			return true;
		}
		if (left.m_record.m_carrierType > right.m_record.m_carrierType)
		{
			return false;
		}
	}
	return false;
}

ostream& operator<<(ostream& os,const PO_BaseTpGroup& po)
{
	os<<po.m_record.m_tpId<<" , "<<
			po.m_record.m_beginTime<<" , "<<
			po.m_record.m_endTime<<" , "<<
			po.m_record.m_bizType<<" , "<<
			po.m_record.m_homeAreaCode<<" , "<<
			po.m_record.m_callType<<" , "<<
			po.m_record.m_roamType<<" , "<<
			po.m_record.m_servType<<" , "<<
			po.m_record.m_carrierType<<" , "<<
			po.m_record.m_ruleId<<" , "<<
			po.m_record.m_effectBizTag; //add by xuf 2009-11-7 10:06:37
			
	return os;
}

PO_Property PO_BaseTpGroup::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_BaseTpGroup",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"select FEEPOLICY_ID,BEGIN_TIME,END_TIME,BIZ_TYPE,HOME_AREA_CODE,\
		 CALL_TYPE,ROAM_TYPE,SERVICE_TYPE,CARRIER_TYPE,rule_id,EFFECT_BIZ_TAG from TD_BASE_TARIFF_GROUP",
		11,"select count(*) from TD_BASE_TARIFF_GROUP"};


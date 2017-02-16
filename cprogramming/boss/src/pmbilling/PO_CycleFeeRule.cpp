#include "PO_CycleFeeRule.h"


bool PO_CycleFeeRule::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}

bool PO_CycleFeeRule::convert(const StringVector &argname)
{
	int i=0;
	string tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_itemCode=atoi(tmpstr.c_str());

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_eventTypeId=atoi(tmpstr.c_str());

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_orderNo=atoi(tmpstr.c_str());
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_sumMode=atoi(tmpstr.c_str());

	return true;
}

PO_CycleFeeRule::PO_CycleFeeRule(const PO_CycleFeeRule& right)
{
	*this=right;
}

void PO_CycleFeeRule::operator=(const PO_CycleFeeRule& right)
{
	m_record=right.m_record;
}

//实际上m_eventTypeId应该为主键
int operator==(const PO_CycleFeeRule &left,const PO_CycleFeeRule& right)
{
    return (left.m_record.m_eventTypeId == right.m_record.m_eventTypeId);
}

int operator<(const PO_CycleFeeRule &left,const PO_CycleFeeRule& right)
{  
    if( atoi( left.m_record.m_eventTypeId.c_str() ) == atoi( right.m_record.m_eventTypeId.c_str() ) )
    {
        return false;
    }
    else
    {
        return ( atoi( left.m_record.m_eventTypeId.c_str() ) < atoi( right.m_record.m_eventTypeId.c_str() ) );
    }
}

ostream& operator<<(ostream& os,const PO_CycleFeeRule& po)
{
	os<<po.m_record.m_itemCode	  <<" , "<<
		po.m_record.m_eventTypeId <<" , "<<
		po.m_record.m_orderNo     <<" , "<<
		po.m_record.m_sumMode; 

	return os;
}


PO_Property PO_CycleFeeRule::m_property={FROM_DATABASE,BY_FLAG,
    "","PO_CycleFeeRule","","","","SELECT ITEM_CODE, \
    EVENT_TYPE_ID, ORDER_NO, SUM_MODE FROM TD_B_CYCLEFEE_RULE",
    4,"SELECT count(*) FROM TD_B_CYCLEFEE_RULE"};

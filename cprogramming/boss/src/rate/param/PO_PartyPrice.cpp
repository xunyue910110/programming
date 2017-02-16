#include "PO_PartyPrice.h"

bool PO_PartyPrice::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}


bool PO_PartyPrice::convert(const StringVector &argname)
{
	int i=0;
	string tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_productCode=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_serviceType=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_partyCorpCode=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_accessNo=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_feePlanCode=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_feePlanState=tmpstr;

    tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_effDate=tmpstr;

    tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_billingModeCode=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_paramCode=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_paramValue=tmpstr;
	
	return true;
}

PO_PartyPrice::PO_PartyPrice(const PO_PartyPrice& right)
{
	*this=right;
}

void PO_PartyPrice::operator=(const PO_PartyPrice& right)
{
	recordStruct=right.recordStruct;
}

int operator==(const PO_PartyPrice &left,const PO_PartyPrice& right)
{
	return (left.recordStruct.m_productCode==right.recordStruct.m_productCode);
}

int operator<(const PO_PartyPrice &left,const PO_PartyPrice& right)
{
	return (left.recordStruct.m_productCode<right.recordStruct.m_productCode);
}

ostream& operator<<(ostream& os,const PO_PartyPrice& po)
{
	os<<po.recordStruct.m_productCode<<" , "<<
		po.recordStruct.m_serviceType<<" , "<<
		po.recordStruct.m_partyCorpCode<<" , "<<
		po.recordStruct.m_accessNo<<" , "<<
		po.recordStruct.m_feePlanCode<<" , "<<
		po.recordStruct.m_feePlanState<<" , "<<
		po.recordStruct.m_effDate<<" , "<<
		po.recordStruct.m_billingModeCode<<" , "<<
		po.recordStruct.m_paramCode<<" , "<<
		po.recordStruct.m_paramValue;
	return os;
}

PO_Property PO_PartyPrice::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_PartyPrice",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"select PRODUCTCODE,SERVICETYPE,PARTYCORPCODE,ACCESSNO,FEEPLANCODE,FEEPLANSTATE,EFFDATE,BILLINGMODECODE,PARACODE,PARAVALUE from TD_B_PARTY_PRICE",
		10,"select count(*) from td_b_party_price"};

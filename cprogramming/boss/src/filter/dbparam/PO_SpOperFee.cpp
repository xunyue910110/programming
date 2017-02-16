#include "PO_SpOperFee.h"

bool PO_SpOperFee::getProperty(PO_Property &property)
{
    property = m_property;
    return true;
}
  
bool PO_SpOperFee::convert(const StringVector &argname)
{
    int i=0;
	string tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.spCode=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.serviceCode=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.operCode=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.feeType=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.infoFee=atoi(tmpstr.c_str());

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.infoCheckType=tmpstr[0];

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.giftFee=atoi(tmpstr.c_str());

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.giftCheckType=tmpstr[0];

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_beginTime=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_endTime=tmpstr;
	
	return true;
}

PO_SpOperFee::PO_SpOperFee(const PO_SpOperFee& right)
{
    *this = right;
}

void PO_SpOperFee::operator=(const PO_SpOperFee& right)
{
    recordStruct = right.recordStruct;
}

int operator==(const PO_SpOperFee &left,const PO_SpOperFee& right)
{
/*
   return (left.recordStruct.spCode==right.recordStruct.spCode||
		left.recordStruct.spCode=="*"||right.recordStruct.spCode=="*")&&
		(left.recordStruct.operCode==right.recordStruct.operCode||
		left.recordStruct.operCode=="*"||right.recordStruct.operCode=="*");
*/
   return (left.recordStruct.spCode==right.recordStruct.spCode);
}

int operator<(const PO_SpOperFee &left,const PO_SpOperFee& right)
{
/*
	if(!(left.recordStruct.spCode =="*")&&!(right.recordStruct.spCode =="*"))
	{
		if(left.recordStruct.spCode<right.recordStruct.spCode)return true;
		if(left.recordStruct.spCode>right.recordStruct.spCode)return false;
	}
	if(!(left.recordStruct.operCode=="*")&&!(right.recordStruct.operCode=="*"))
	{
		if(left.recordStruct.operCode<right.recordStruct.operCode)return true;
		if(left.recordStruct.operCode>right.recordStruct.operCode)return false;
	}
*/
	return (left.recordStruct.spCode<right.recordStruct.spCode);
		
}

ostream& operator<<(ostream& os,const PO_SpOperFee& po)
{
    os<<po.recordStruct.spCode     <<" , "
      <<po.recordStruct.serviceCode<<" , "
      <<po.recordStruct.operCode   <<" , "
      <<po.recordStruct.feeType   <<" , "
      <<po.recordStruct.infoFee   <<" , "
      <<po.recordStruct.infoCheckType   <<" , "
      <<po.recordStruct.giftFee   <<" , "
      <<po.recordStruct.giftCheckType   <<" , "
      <<po.recordStruct.m_beginTime	<< " , "
      <<po.recordStruct.m_endTime << endl;
    return os;
}

PO_Property PO_SpOperFee::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_SpOperFee",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"select SP_CODE,SERVICE_CODE,OPER_CODE,FEE_TYPE,INFO_FEE,INFO_CHECK_TYPE,GIFT_FEE,GIFT_CHECK_TYPE,BEGIN_TIME,END_TIME from TD_SP_MON_OPER",
		10,"select count(*) from TD_SP_MON_OPER"};




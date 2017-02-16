
#include "PO_DetailBillItem.h"

bool PO_DetailBillItem::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}
  
bool PO_DetailBillItem::convert(const StringVector &argname)
{
	int i=0;
	string tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_billId=tmpstr;	

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_billType=atoi(tmpstr.c_str());	

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_priority=atoi(tmpstr.c_str());

 	tmpstr=*(argname.begin()+i++);
 	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
 	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
 	m_record.m_feeField=tmpstr;	
 	
 	tmpstr=*(argname.begin()+i++);
 	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
 	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
 	m_record.m_bizType=tmpstr;	
 	
 	for (int j=0;j < DTLBILL_FACTORS_COUNT; ++j) 
 	{
 		tmpstr=*(argname.begin()+i++);
 		tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
 		tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
 		m_record.m_factors[j]=tmpstr;
 	}
 	tmpstr=*(argname.begin()+i++);
 	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
 	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
 	m_record.m_ruleId=tmpstr;	
 	
 	tmpstr=*(argname.begin()+i++);
 	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
 	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
 	m_record.m_beginCycle=tmpstr;	
 	
	tmpstr=*(argname.begin()+i++);
 	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
 	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
 	m_record.m_endCycle=tmpstr;
 	
 	//Added By Sunam 2009/8/14 15:02:23 对于账务账单合账增加按feePolicyId以及忙闲时
 	tmpstr=*(argname.begin()+i++);
 	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
 	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
 	m_record.m_feePolicyId=atol(tmpstr.c_str());
 	
 	tmpstr=*(argname.begin()+i++);
 	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
 	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
 	m_record.m_beginTime=atol(tmpstr.c_str());
 	
 	tmpstr=*(argname.begin()+i++);
 	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
 	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
 	m_record.m_endTime=atol(tmpstr.c_str());
 	
 	m_record.m_condRule=NULL;
	return true;
}

PO_DetailBillItem::PO_DetailBillItem(const PO_DetailBillItem& right)
{
	*this=right;
}

void PO_DetailBillItem::operator=(const PO_DetailBillItem& right)
{
	m_record=right.m_record;
}


int operator==(const PO_DetailBillItem &left,const PO_DetailBillItem& right)
{
  if (left.m_record.m_billType != right.m_record.m_billType ||
      left.m_record.m_feeField != right.m_record.m_feeField ||
      left.m_record.m_bizType  != right.m_record.m_bizType  ) 
      return false;
     
	bool result = true;
	for (int i=0;i < DTLBILL_FACTORS_COUNT; ++i) 
	{
		result = (left.m_record.m_factors[i]    ==right.m_record.m_factors[i] 
					    || left.m_record.m_factors[i] =="*"
					    || right.m_record.m_factors[i]=="*");
		if(!result)
			return false;
	} 
  return true;
}

int operator<(const PO_DetailBillItem &left,const PO_DetailBillItem& right)
{
  if (left.m_record.m_billType < right.m_record.m_billType)
  {
  	return true;
  }

  if (left.m_record.m_billType > right.m_record.m_billType)
  {
  	return false;
  }

  if (left.m_record.m_feeField < right.m_record.m_feeField)
  {
  	return true;
  }
  if (left.m_record.m_feeField > right.m_record.m_feeField)
  {
  	return false;
  }
    
  if (left.m_record.m_bizType < right.m_record.m_bizType)
  {
  	return true;
  }
  if (left.m_record.m_bizType > right.m_record.m_bizType)
  {
  	return false;
  }    
	//按优先级排倒序
	if (left.m_record.m_priority < right.m_record.m_priority)
	{
		return false;
	}
	else if (left.m_record.m_priority >= right.m_record.m_priority)
	{
		return true;
	}
	//按时间排正序
	if (left.m_record.m_beginTime < right.m_record.m_beginTime)
	{
		return true;
	}
	else if (left.m_record.m_beginTime >= right.m_record.m_beginTime)
	{
		return false;
	}
	
  return true;
}


ostream& operator<<(ostream& os,const PO_DetailBillItem& po)
{
	os<<po.m_record.m_billId<<" , "<<
	  po.m_record.m_billType<<" , "<<
		po.m_record.m_priority<<" , "<<
		po.m_record.m_feeField<<" , "<<
		po.m_record.m_bizType<<" , "<<
		po.m_record.m_beginCycle <<" , "<<
		po.m_record.m_endCycle <<" , "<<
		po.m_record.m_ruleId ;
    
    for (int i=0;i < DTLBILL_FACTORS_COUNT; ++i) 
    {
		  os<<" , "<<po.m_record.m_factors[i];
    }

	return os;
}

//Added By Sunam 2009/8/14 15:02:23 对于账务账单合账增加按feePolicyId以及忙闲时
PO_Property PO_DetailBillItem::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_DetailBillItem",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"select item_id,billType,priority,event_elem_id,evenType,factor1, \
       factor2,factor3,factor4,factor5,factor6,factor7,factor8, \
       factor9,factor10,factor11,factor12,factor13,factor14,factor15, \
       factor16,factor17,factor18,factor19,factor20,ruleId,beginCycle,endCycle,feePolicyId,beginTime,endTime \
       from TD_B_SUMBILL",
		31,"select count(*) from TD_B_SUMBILL"};


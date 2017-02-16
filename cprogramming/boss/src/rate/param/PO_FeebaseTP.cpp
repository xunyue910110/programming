
#include "PO_FeebaseTP.h"

bool PO_FeebaseTP::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}
  
bool PO_FeebaseTP::convert(const StringVector &argname)
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
	m_record.m_priority=atoi(tmpstr.c_str());

  tmpstr=*(argname.begin()+i++);
  tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
  tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
  m_record.m_feeField=tmpstr;	

  tmpstr=*(argname.begin()+i++);
  tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
  tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
  m_record.m_bizType=tmpstr;	

  for (int j=0;j < BASETP_FACTORS_COUNT; ++j) 
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
  m_record.m_condRule=NULL;
	return true;
}

PO_FeebaseTP::PO_FeebaseTP(const PO_FeebaseTP& right)
{
	*this=right;
}

void PO_FeebaseTP::operator=(const PO_FeebaseTP& right)
{
	m_record=right.m_record;
}


int operator==(const PO_FeebaseTP &left,const PO_FeebaseTP& right)
{
  if (left.m_record.m_feeField != right.m_record.m_feeField ||
      left.m_record.m_bizType  != right.m_record.m_bizType  ) 
      return false;
     
	bool result = true;
	for (int i=0;i < BASETP_FACTORS_COUNT; ++i) 
	{
		result = (left.m_record.m_factors[i]==right.m_record.m_factors[i] 
					    || left.m_record.m_factors[i]=="*"
					    || right.m_record.m_factors[i]=="*");
		if(!result)
			return false;
	} 
  return true;
}

int operator<(const PO_FeebaseTP &left,const PO_FeebaseTP& right)
{
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
    return true;
}


ostream& operator<<(ostream& os,const PO_FeebaseTP& po)
{
	os<<po.m_record.m_tpId<<" , "<<
		po.m_record.m_priority<<" , "<<
		po.m_record.m_feeField<<" , "<<
		po.m_record.m_bizType <<" , "<<
		po.m_record.m_ruleId ;
    
    for (int i=0;i < BASETP_FACTORS_COUNT; ++i) 
    {
		  os<<" , "<<po.m_record.m_factors[i];
    }

	return os;
}

PO_Property PO_FeebaseTP::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_FeebaseTP",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"select TP_ID,PRIORITY,FEE_ID,biz_type, "
		" FACTOR1,FACTOR2,FACTOR3,FACTOR4,FACTOR5,FACTOR6,FACTOR7,FACTOR8,"
		" FACTOR9,FACTOR10,FACTOR11,FACTOR12, "
		" rule_id from TD_FEEBASE_ALL_TP ",
		17,"select count(*) from TD_FEEBASE_ALL_TP"};


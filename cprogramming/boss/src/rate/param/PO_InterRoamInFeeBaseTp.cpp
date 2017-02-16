#include "PO_InterRoamInFeeBaseTp.h"

bool PO_InterRoamInFeeBaseTp::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}


bool PO_InterRoamInFeeBaseTp::convert(const StringVector &argname)
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
  m_record.m_beginTime=tmpstr;	

 	tmpstr=*(argname.begin()+i++);
  tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
  tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
  m_record.m_endTime=tmpstr;	

 	tmpstr=*(argname.begin()+i++);
  tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
  tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
  m_record.m_aspGroup=tmpstr;	
  
  tmpstr=*(argname.begin()+i++);
  tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
  tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
  m_record.m_bizType=tmpstr;	
  
  tmpstr=*(argname.begin()+i++);
  tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
  tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
  m_record.m_sameHomeAb=tmpstr[0];	
  
  tmpstr=*(argname.begin()+i++);
  tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
  tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
  m_record.m_homeCodeB=tmpstr;	
  
  tmpstr=*(argname.begin()+i++);
  tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
  tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
  m_record.m_callType=tmpstr;	
  
  tmpstr=*(argname.begin()+i++);
  tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
  tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
  m_record.m_bAsp=tmpstr;	
  
  tmpstr=*(argname.begin()+i++);
  tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
  tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
  m_record.m_bServType=tmpstr;	

  tmpstr=*(argname.begin()+i++);
  tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
  tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
  m_record.m_carrierType=tmpstr;	
  
  tmpstr=*(argname.begin()+i++);
  tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
  tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
  m_record.m_longType1=tmpstr;	

  tmpstr=*(argname.begin()+i++);
  tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
  tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
  m_record.m_longGroup1=tmpstr;	
  
  tmpstr=*(argname.begin()+i++);
  tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
  tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
  m_record.m_longType2=tmpstr;	

  tmpstr=*(argname.begin()+i++);
  tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
  tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
  m_record.m_longGroup2=tmpstr;
       
	return true;
}

PO_InterRoamInFeeBaseTp::PO_InterRoamInFeeBaseTp(const PO_InterRoamInFeeBaseTp& right)
{
	*this=right;
}

void PO_InterRoamInFeeBaseTp::operator=(const PO_InterRoamInFeeBaseTp& right)
{
	m_record=right.m_record;
}

int operator==(const PO_InterRoamInFeeBaseTp &left,const PO_InterRoamInFeeBaseTp &right)
{
	if (left.m_record.m_feeField != right.m_record.m_feeField) 
	{
		return false;
	}
	
	return (((left.m_record.m_aspGroup == right.m_record.m_aspGroup) || 
			 (left.m_record.m_aspGroup == "*") || 
			 (right.m_record.m_aspGroup == "*")) &&
			(left.m_record.m_bizType  == right.m_record.m_bizType) &&
			((left.m_record.m_sameHomeAb == right.m_record.m_sameHomeAb) || 
			 (left.m_record.m_sameHomeAb == '*') || 
			 (right.m_record.m_sameHomeAb == '*')) &&
			((left.m_record.m_homeCodeB == right.m_record.m_homeCodeB) || 
			 (left.m_record.m_homeCodeB == "*") || 
			 (right.m_record.m_homeCodeB == "*")) &&
			((left.m_record.m_callType == right.m_record.m_callType) || 
			 (left.m_record.m_callType == "*") || 
			 (right.m_record.m_callType == "*")) &&
			((left.m_record.m_bAsp == right.m_record.m_bAsp) || 
			 (left.m_record.m_bAsp == "*") || 
			 (right.m_record.m_bAsp == "*")) &&
			((left.m_record.m_bServType == right.m_record.m_bServType) || 
			 (left.m_record.m_bServType == "*") || 
			 (right.m_record.m_bServType == "*")) &&
			((left.m_record.m_carrierType == right.m_record.m_carrierType) || 
			 (left.m_record.m_carrierType == "*") || 
			 (right.m_record.m_carrierType == "*")) &&
			((left.m_record.m_longType1 == right.m_record.m_longType1) || 
			 (left.m_record.m_longType1 == "*") || 
			 (right.m_record.m_longType1 == "*")) &&
			((left.m_record.m_longGroup1 == right.m_record.m_longGroup1) || 
			 (left.m_record.m_longGroup1 == "*") || 
			 (right.m_record.m_longGroup1 == "*")) &&
			((left.m_record.m_longType2 == right.m_record.m_longType2) || 
			 (left.m_record.m_longType2 == "*") || 
			 (right.m_record.m_longType2 == "*")) &&
			((left.m_record.m_longGroup2 == right.m_record.m_longGroup2) || 
			 (left.m_record.m_longGroup2 == "*") || 
			 (right.m_record.m_longGroup2 == "*")));
}

int operator<(const PO_InterRoamInFeeBaseTp &left,const PO_InterRoamInFeeBaseTp& right)
{
  if (left.m_record.m_feeField < right.m_record.m_feeField)
  {
  	return true;
  }
  else if (left.m_record.m_feeField > right.m_record.m_feeField)
  {
  	return false;
  }
  else if (left.m_record.m_feeField == right.m_record.m_feeField)
  {
  	//按优先级排倒序
  	if (left.m_record.m_priority < right.m_record.m_priority)
  	{
  		return false;
  	}
  	else if (left.m_record.m_priority >= right.m_record.m_priority)
  	{
  		return true;
  	}
  }
  return false;
}

ostream& operator<<(ostream& os,const PO_InterRoamInFeeBaseTp& po)
{
	os<<po.m_record.m_tpId<<" , "<<
		po.m_record.m_priority<<" , "<<
		po.m_record.m_feeField<<" , "<<
		po.m_record.m_beginTime<<" , "<<
		po.m_record.m_endTime<<" , "<<
		po.m_record.m_aspGroup<<" , "<<
		po.m_record.m_bizType<<" , "<<
		po.m_record.m_sameHomeAb<<" , "<<
		po.m_record.m_homeCodeB<<" , "<<
		po.m_record.m_callType<<" , "<<
		po.m_record.m_bAsp<<" , "<<
		po.m_record.m_bServType<<" , "<<
		po.m_record.m_carrierType<<" , "<<
		po.m_record.m_longType1<<" , "<<
		po.m_record.m_longGroup1<<" , "<<
		po.m_record.m_longType2<<" , "<<
		po.m_record.m_longGroup2;
			
	return os;
}

PO_Property PO_InterRoamInFeeBaseTp::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_InterRoamInFeeBaseTp",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"select TP_ID, PRIORITY, FEE_FIELD, START_TIME, END_TIME, ASP_GROUP,\
		 BIZ_TYPE, SAME_HOME_AB, HOME_CODE_B, CALL_TYPE, B_ASP, B_SERV_TYPE,\
		 CARRIER_TYPE, LONG_TYPE1, LONG_GROUP1, LONG_TYPE2, LONG_GROUP2 \
		 from TD_INTER_ROAMIN_BASETP",
		17, "select count(*) from TD_INTER_ROAMIN_BASETP"};

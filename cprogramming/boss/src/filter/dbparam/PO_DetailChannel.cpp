#include "PO_DetailChannel.h"

bool PO_DetailChannel::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}

bool PO_DetailChannel::convert(const StringVector &argname)
{
  int i=0;
  string tmpstr;

  tmpstr=*(argname.begin()+i++);
  tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
  tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
  recordStruct.m_channel=tmpstr;

  tmpstr=*(argname.begin()+i++);
  tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
  tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
  recordStruct.m_pri=tmpstr;

  int j;
  for (j=0;j < FACTORS_COUNT; ++j) {
     tmpstr=*(argname.begin()+i++);
     tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
     tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
     recordStruct.m_factors[j]=tmpstr;
  }

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


PO_DetailChannel::PO_DetailChannel(const PO_DetailChannel& right)
{
	*this=right;
}

void PO_DetailChannel::operator=(const PO_DetailChannel& right)
{
	recordStruct=right.recordStruct;
}

//searchMulti方式不使用，采用全表扫描
int operator==(const PO_DetailChannel &left,const PO_DetailChannel& right)
{
	int i; 
	bool result = true;
	for (i=0;i < FACTORS_COUNT; ++i) 
	{
		//支持XXX*的配置
		if (left.recordStruct.m_factors[i].c_str()[0] == '*')
		{
			continue;
		}
		else if (left.recordStruct.m_factors[i][left.recordStruct.m_factors[i].length() - 1] == '*')
		{
			if ((PatternMatcher::matches(left.recordStruct.m_factors[i].c_str(),(right.recordStruct.m_factors[i]).c_str())) ||
				(right.recordStruct.m_factors[i] == "*"))
			{
				continue;
			}
			else
			{
				return false;
			}
		}

		result = (left.recordStruct.m_factors[i]==right.recordStruct.m_factors[i] 
					|| left.recordStruct.m_factors[i]=="*"
					|| right.recordStruct.m_factors[i]=="*");
		if(!result)
			return false;
	} 

    return true;
}

int operator<(const PO_DetailChannel &left,const PO_DetailChannel& right)
{
 int i;
 if(left.recordStruct.m_pri == right.recordStruct.m_pri)
 {
    for (i=0;i < FACTORS_COUNT; ++i) 
    {
	    if(left.recordStruct.m_factors[i]!="*" && right.recordStruct.m_factors[i]!="*" 
	    && left.recordStruct.m_factors[i]!=right.recordStruct.m_factors[i])
	    {
	    	if(left.recordStruct.m_factors[i]>right.recordStruct.m_factors[i])
	    	{
	    		return false;
	    	}
	    }
    }
 }
 else
 {
 	 return (left.recordStruct.m_pri > right.recordStruct.m_pri);
 }

/*
    int i;
    for (i=0;i < FACTORS_COUNT; ++i) 
    {
	    if(left.recordStruct.m_factors[i]!="*" && right.recordStruct.m_factors[i]!="*" 
	    && left.recordStruct.m_factors[i]!=left.recordStruct.m_factors[i])
	    {
	    	if(left.recordStruct.m_factors[i]>right.recordStruct.m_factors[i])
	    	{
	    		return false;
	    	}
	    }
    }
*/
	return true;
}


ostream& operator<<(ostream& os,const PO_DetailChannel &po)
{
  int i;
	os<<po.recordStruct.m_channel<<" , "<<
		po.recordStruct.m_pri<<" , ";
    
    for (i=0;i < FACTORS_COUNT; ++i) {
		os<<po.recordStruct.m_factors [i] <<" , ";
    }

	os<<po.recordStruct.m_beginTime<<" , "<<
		po.recordStruct.m_endTime;
	return os;
}


PO_Property PO_DetailChannel::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_DetailChannel",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"select FILTER_CHANNEL,PRI_KEY,"
		" FACTOR1,FACTOR2,FACTOR3,FACTOR4,FACTOR5,FACTOR6,FACTOR7,FACTOR8,FACTOR9,FACTOR10,"
		" FACTOR11,FACTOR12,FACTOR13,FACTOR14,FACTOR15,FACTOR16,FACTOR17,FACTOR18,FACTOR19,FACTOR20,"
		" BEGIN_TIME,END_TIME from TD_FILTER ",
		24,"select count(*) from TD_FILTER"};


///继承类
bool PO_DetailChannel_GSM::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}

bool PO_DetailChannel_SM::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}

bool PO_DetailChannel_GPRS::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}

bool PO_DetailChannel_MISC::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}

PO_Property PO_DetailChannel_GSM::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_DetailChannel_GSM",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"select FILTER_CHANNEL,PRI_KEY,"
		" FACTOR1,FACTOR2,FACTOR3,FACTOR4,FACTOR5,FACTOR6,FACTOR7,FACTOR8,FACTOR9,FACTOR10,"
		" FACTOR11,FACTOR12,FACTOR13,FACTOR14,FACTOR15,FACTOR16,FACTOR17,FACTOR18,FACTOR19,FACTOR20,"
		" BEGIN_TIME,END_TIME from TD_FILTER where FACTOR1 = '1' or FACTOR1 = '*' ",
		24,"select count(*) from TD_FILTER"};

PO_Property PO_DetailChannel_SM::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_DetailChannel_SM",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"select FILTER_CHANNEL,PRI_KEY,"
		" FACTOR1,FACTOR2,FACTOR3,FACTOR4,FACTOR5,FACTOR6,FACTOR7,FACTOR8,FACTOR9,FACTOR10,"
		" FACTOR11,FACTOR12,FACTOR13,FACTOR14,FACTOR15,FACTOR16,FACTOR17,FACTOR18,FACTOR19,FACTOR20,"
		" BEGIN_TIME,END_TIME from TD_FILTER where FACTOR1 = '2' or FACTOR1 = '*' ",
		24,"select count(*) from TD_FILTER"};

PO_Property PO_DetailChannel_GPRS::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_DetailChannel_GPRS",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"select FILTER_CHANNEL,PRI_KEY,"
		" FACTOR1,FACTOR2,FACTOR3,FACTOR4,FACTOR5,FACTOR6,FACTOR7,FACTOR8,FACTOR9,FACTOR10,"
		" FACTOR11,FACTOR12,FACTOR13,FACTOR14,FACTOR15,FACTOR16,FACTOR17,FACTOR18,FACTOR19,FACTOR20,"
		" BEGIN_TIME,END_TIME from TD_FILTER where FACTOR1 = '3' or FACTOR1 = '*' ",
		24,"select count(*) from TD_FILTER"};

PO_Property PO_DetailChannel_MISC::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_DetailChannel_MISC",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"select FILTER_CHANNEL,PRI_KEY,"
		" FACTOR1,FACTOR2,FACTOR3,FACTOR4,FACTOR5,FACTOR6,FACTOR7,FACTOR8,FACTOR9,FACTOR10,"
		" FACTOR11,FACTOR12,FACTOR13,FACTOR14,FACTOR15,FACTOR16,FACTOR17,FACTOR18,FACTOR19,FACTOR20,"
		" BEGIN_TIME,END_TIME from TD_FILTER where FACTOR1 = '4' or FACTOR1 = '*' ",
		24,"select count(*) from TD_FILTER"};

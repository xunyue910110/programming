#include "PO_GprsSgsnAddr.h"

//说明：decAddr为点分10机制IP地址 
void PO_GprsSgsnAddr::decTohexAddr(const string &decAddr,string &hexAddr)
{
	char tmp[3+1];
	char desStr[16+1];
	int  j=0,k=0;
	for(int i=0;;i++)
	{
		if(i==decAddr.length())
		{
			tmp[j]=0;
			sprintf(desStr+k,"%02X",atoi(tmp));
			k+=2;
			break;
		}else	if(decAddr[i]=='.')
		{
			tmp[j]=0;
			sprintf(desStr+k,"%02X",atoi(tmp));
			k+=2;
			j=0;
		}else
		{
			tmp[j]=decAddr[i];
			j++;
		}
	}
	desStr[k]=0;
	hexAddr = desStr;
}

bool PO_GprsSgsnAddr::getProperty(PO_Property &property)
{
    property = m_property;
    return true;
}


bool PO_GprsSgsnAddr::convert(const StringVector &argname)
{
	int    i = 0;
	string tmpstr;
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_beginSgsnOrg = tmpstr;
	decTohexAddr(m_record.m_beginSgsnOrg,m_record.m_beginSgsn);
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
  m_record.m_endSgsnOrg = tmpstr;
	decTohexAddr(m_record.m_endSgsnOrg,m_record.m_endSgsn);

	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
  m_record.m_areaCode = tmpstr;

	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
  m_record.m_provCode = tmpstr;

	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_homeType = tmpstr[0];

	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_beginTime = tmpstr;
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_endTime = tmpstr;

	return true;
}

PO_GprsSgsnAddr::PO_GprsSgsnAddr(const PO_GprsSgsnAddr& right)
{
    *this = right;
}

void PO_GprsSgsnAddr::operator=(const PO_GprsSgsnAddr& right)
{
    m_record = right.m_record;
}

int operator==(const PO_GprsSgsnAddr &left,const PO_GprsSgsnAddr& right)
{
	long  b1,b2,e1,e2;
	b1 = strtol(left.m_record.m_beginSgsn.c_str(),NULL,16);
	b2 = strtol(right.m_record.m_beginSgsn.c_str(),NULL,16);
	e1 = strtol(left.m_record.m_endSgsn.c_str(),NULL,16);
	e2 = strtol(right.m_record.m_endSgsn.c_str(),NULL,16);

	return (b1>=b2 && b1<=e2 || b1<=b2 && e1>=b2);
}

int operator<(const PO_GprsSgsnAddr &left,const PO_GprsSgsnAddr& right)
{
	long  b1,b2,e1,e2;
	b1 = strtol(left.m_record.m_beginSgsn.c_str(),NULL,16);
	b2 = strtol(right.m_record.m_beginSgsn.c_str(),NULL,16);
	e1 = strtol(left.m_record.m_endSgsn.c_str(),NULL,16);
	e2 = strtol(right.m_record.m_endSgsn.c_str(),NULL,16);
	
  if(b1<b2 && e1<e2) return true;

  if(b1>b2 && e1>e2) return false;

	return false;
}

ostream& operator<<(ostream& os,const PO_GprsSgsnAddr& po)
{
    os<<po.m_record.m_beginSgsn <<" , "
      <<po.m_record.m_endSgsn   <<" , "
      <<po.m_record.m_beginSgsnOrg <<" , "
      <<po.m_record.m_endSgsnOrg   <<" , "
      <<po.m_record.m_areaCode  <<" , "
      <<po.m_record.m_provCode  <<" , "
      <<po.m_record.m_homeType  <<" , "
      <<po.m_record.m_beginTime <<" , "
      <<po.m_record.m_endTime;
    return os;
}


PO_Property PO_GprsSgsnAddr::m_property={FROM_DATABASE,BY_FLAG,
    "","PO_GprsSgsnAddr",USERNAME_1,PASSWORD_1,SERVNAME_1,
    "select begin_sgsn,end_sgsn,area_code,prov_code,home_type,begin_time,end_time from TD_GPRS_SGSN_SEGMENT",
    7,"select count(*) from TD_GPRS_SGSN_SEGMENT"};

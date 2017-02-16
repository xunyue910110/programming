#include "PO_GprsApn.h"

bool PO_GprsApn::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}


bool PO_GprsApn::convert(const StringVector &argname)
{
	int i=0;
	string tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.provCode=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.apnni=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.matchType=tmpstr.c_str()[0];

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.beginTime=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.endTime=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.apnGroup=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	if(tmpstr.length()>0)
		recordStruct.servRange=tmpstr.c_str()[0];
	else
		recordStruct.servRange=0;
		
	return true;
}

PO_GprsApn::PO_GprsApn(const PO_GprsApn& right)
{
	*this=right;
}

void PO_GprsApn::operator=(const PO_GprsApn& right)
{
	recordStruct=right.recordStruct;
}

int operator==(const PO_GprsApn &left,const PO_GprsApn& right)
{
	return (strcasecmp(left.recordStruct.apnni.c_str(),right.recordStruct.apnni.c_str())==0);
}

int operator<(const PO_GprsApn &left,const PO_GprsApn& right)
{
	return (strcasecmp(left.recordStruct.apnni.c_str(),right.recordStruct.apnni.c_str())<0);
}

ostream& operator<<(ostream& os,const PO_GprsApn& po)
{
	os<<po.recordStruct.provCode<<" , "<<
		po.recordStruct.apnni     <<" , "<<
		po.recordStruct.matchType<<" , "<<
		po.recordStruct.beginTime<<" , "<<
		po.recordStruct.endTime  <<"  , "<<
		po.recordStruct.apnGroup <<"  , "<<
		po.recordStruct.servRange;
	return os;
}


PO_Property PO_GprsApn::m_property={FROM_DATABASE,BY_FLAG,"",
		"PO_GprsApn",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"SELECT PROV_CODE,UPPER(APNNI),MATCH_TYPE,BEGIN_TIME,END_TIME,apnni_group,service_range FROM TD_GPRS_APN",
		7,"SELECT COUNT(*) FROM TD_GPRS_APN"};

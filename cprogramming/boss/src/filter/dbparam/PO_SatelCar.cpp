#include "PO_SatelCar.h"

bool PO_SatelCar::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}


bool PO_SatelCar::convert(const StringVector &argname)
{
	int i=0;
	string tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.province_code=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.msc=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.lac=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.cellId=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.beginTime=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.endTime=tmpstr;
	
	return true;
}

PO_SatelCar::PO_SatelCar(const PO_SatelCar& right)
{
	*this=right;
}

void PO_SatelCar::operator=(const PO_SatelCar& right)
{
	recordStruct=right.recordStruct;
}

int operator==(const PO_SatelCar &left,const PO_SatelCar& right)
{
	return ((left.recordStruct.msc==right.recordStruct.msc) &&
	    (left.recordStruct.lac==right.recordStruct.lac)&&
			(left.recordStruct.cellId==right.recordStruct.cellId));
}

int operator<(const PO_SatelCar &left,const PO_SatelCar& right)
{
  if (left.recordStruct.msc < right.recordStruct.msc)
	{
		return true;
	}
	else if (left.recordStruct.msc > right.recordStruct.msc)
	{
		return false;
	}
	if (left.recordStruct.lac < right.recordStruct.lac)
	{
		return true;
	}
	else if (left.recordStruct.lac > right.recordStruct.lac)
	{
		return false;
	}
	if (left.recordStruct.cellId < right.recordStruct.cellId)
	{
		return true;
	}
	return false;
}

ostream& operator<<(ostream& os,const PO_SatelCar& po)
{
	os<<po.recordStruct.province_code<<" , "<<
		po.recordStruct.msc<<" , "<<
		po.recordStruct.lac<<" , "<<
		po.recordStruct.cellId<<" , "<<
		po.recordStruct.beginTime<<" , "<<
		po.recordStruct.endTime;
		
	return os;
}


PO_Property PO_SatelCar::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_SatelCar",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"select PROVICE_CODE, MSC, LAC, CELL_ID, BEGIN_TIME,END_TIME from TD_SATELCAR ",
		6,"select count(*) from TD_SATELCAR"};



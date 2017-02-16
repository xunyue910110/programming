#include "Mdb_TestCm.h"
#include <string>

bool Mdb_TestCm::getProperty(Mdb_Property &property) const
{
	property=m_property;
	return true;
}

bool Mdb_TestCm::convert(Session* session)
{
	int length;
 	session->getColumnValue(0,&recordStruct.m_userId,length);
 	session->getColumnValue(1,&recordStruct.m_billId,length);
 	session->getColumnValue(2,&recordStruct.m_value,length);
 	return true;
}

void Mdb_TestCm::setKeyParams(InputParams *pInputParamsseIndex) const
{
	pInputParamsseIndex->setValue(VAR_TYPE_INT,(void *)&recordStruct.m_userId);
}
    
void Mdb_TestCm::setOtherParams(InputParams *pInputParamsseOther) const
{
	pInputParamsseOther->setValue(VAR_TYPE_INT,(void *)&recordStruct.m_billId);
}

Mdb_TestCm::Mdb_TestCm(const Mdb_TestCm& right)
{
	*this=right;
}

void Mdb_TestCm::operator=(const Mdb_TestCm& right)
{
	recordStruct=right.recordStruct;
}

int operator==(const Mdb_TestCm &left,const Mdb_TestCm& right)
{
	return 1;
}

int operator<(const Mdb_TestCm &left,const Mdb_TestCm& right)
{
	return 1;
}

ostream& operator<<(ostream& os,const Mdb_TestCm &po)
{
	os << po.recordStruct.m_userId << " , "
	   << po.recordStruct.m_billId << " , "
	   << po.recordStruct.m_value ;
	return os;
}

Mdb_Property Mdb_TestCm::m_property={"TEST_CM","IDX_TEST_CM",1,"billid == :0",1};

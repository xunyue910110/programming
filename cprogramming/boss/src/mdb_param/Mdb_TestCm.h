#ifndef MDB_TESTCM_H_HEADER_INCLUDED
#define MDB_TESTCM_H_HEADER_INCLUDED

#include "Mdb_Common.h"
#include <string>

class Mdb_TestCm
{
  public:
    bool getProperty(Mdb_Property &property) const;

    bool convert(Session* session);
    
    void setKeyParams(InputParams *pInputParamsseIndex) const;
    
    void setOtherParams(InputParams *pInputParamsseOther) const;

    Mdb_TestCm(){}

    Mdb_TestCm(const Mdb_TestCm& right);

    void operator=(const Mdb_TestCm& right);

    friend int operator==(const Mdb_TestCm &left,
    		const Mdb_TestCm &right);

    friend int operator<(const Mdb_TestCm &left,
    		const Mdb_TestCm& right);

    friend ostream& operator<<(ostream& os,const Mdb_TestCm& po);

    struct
    {
    	int  m_userId;
    	int  m_billId;
    	long m_value;
    }recordStruct;

  private:
	static Mdb_Property m_property;
};

#endif /* MDB_TEST_CM_H_HEADER_INCLUDED*/

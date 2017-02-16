#ifndef MDB_ACCNTINFO_H_INCLUDED_20081014_
#define MDB_ACCNTINFO_H_INCLUDED_20081014_

#include "Mdb_Common.h"
#include "FieldsXX.h"
#include "IdInfo.h"
//#include "RateConstDef.h"

class Mdb_AccntInfo
{
  public:
    bool getProperty(Mdb_Property &property) const;

    bool convert(Session* session);
    
    void setKeyParams(InputParams *pInputParamsseIndex) const;
    
    void setOtherParams(InputParams *pInputParamsseOther) const;

    Mdb_AccntInfo(){}

    Mdb_AccntInfo(const Mdb_AccntInfo& right);

    void operator=(const Mdb_AccntInfo& right);
    
    friend ostream& operator<<(ostream& os,const Mdb_AccntInfo& po);
    IdInfo m_record;

  private:
	  static Mdb_Property m_property;
};

#endif //MDB_ACCNTINFO_H_INCLUDED_20081014_

#ifndef Mdb_ReUserInfo_All_H_INCLUDED
#define Mdb_ReUserInfo_All_H_INCLUDED

#include "Mdb_Common.h"
#include "FieldsXX.h"
#include "BillingCommon.h"

class Mdb_ReUserInfo_All
{
  public:
    bool getProperty(Mdb_Property &property) const;

    bool convert(Session* session);
    
    void setKeyParams(InputParams *pInputParamsseIndex) const;
    
    void setOtherParams(InputParams *pInputParamsseOther) const;

    Mdb_ReUserInfo_All(){}

    Mdb_ReUserInfo_All(const Mdb_ReUserInfo_All& right);

    void operator=(const Mdb_ReUserInfo_All& right);
    
    friend ostream& operator<<(ostream& os,const Mdb_ReUserInfo_All& po);
    
    ReAccountEventInfo m_record;

  private:
	  static Mdb_Property m_property;
};

#endif //Mdb_ReUserInfo_All_H_INCLUDED

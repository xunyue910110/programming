#ifndef Mdb_UserServ_H
#define Mdb_UserServ_H

#include "Mdb_Common.h"
#include "FieldsXX.h"
#include "BillingCommon.h"


class Mdb_UserServ
{
  public:
    bool getProperty(Mdb_Property &property) const;

    bool convert(Session* session);
    
    void setKeyParams(InputParams *pInputParamsseIndex) const;
    
    void setOtherParams(InputParams *pInputParamsseOther) const;

    Mdb_UserServ(){}

    Mdb_UserServ(const Mdb_UserServ& right);

    void operator=(const Mdb_UserServ& right);
    
    friend ostream& operator<<(ostream& os,const Mdb_UserServ& po);
    
    UserServ m_record;

  private:
      static Mdb_Property m_property;
};

#endif //Mdb_UserServ_H


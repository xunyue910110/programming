#ifndef Mdb_UserServParam_H
#define Mdb_UserServParam_H

#include "Mdb_Common.h"
#include "FieldsXX.h"
#include "BillingCommon.h"


class Mdb_UserServParam
{
  public:
    bool getProperty(Mdb_Property &property) const;

    bool convert(Session* session);
    
    void setKeyParams(InputParams *pInputParamsseIndex) const;
    
    void setOtherParams(InputParams *pInputParamsseOther) const;

    Mdb_UserServParam(){}

    Mdb_UserServParam(const Mdb_UserServParam& right);

    void operator=(const Mdb_UserServParam& right);
    
    //Added by Malone 2009-11-28 15:46 start
    friend int operator<(const Mdb_UserServParam &left,
  		                   const Mdb_UserServParam& right);
    //Added by Malone 2009-11-28 15:46 end
    
    friend ostream& operator<<(ostream& os,const Mdb_UserServParam& po);
    
    UserServParam m_record;

  private:
      static Mdb_Property m_property;
};

#endif //Mdb_UserServParam_H


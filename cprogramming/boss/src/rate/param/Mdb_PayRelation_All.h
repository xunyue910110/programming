#ifndef Mdb_PayRelation_All_H_INCLUDED
#define Mdb_PayRelation_All_H_INCLUDED

#include "Mdb_Common.h"
#include "FieldsXX.h"


class Mdb_PayRelation_All
{
  public:
    bool getProperty(Mdb_Property &property) const;

    bool convert(Session* session);
    
    void setKeyParams(InputParams *pInputParamsseIndex) const;
    
    void setOtherParams(InputParams *pInputParamsseOther) const;

    Mdb_PayRelation_All(){}

    Mdb_PayRelation_All(const Mdb_PayRelation_All& right);

    void operator=(const Mdb_PayRelation_All& right);
    
    friend ostream& operator<<(ostream& os,const Mdb_PayRelation_All& po);
    struct 
    {
    	T_SERVER_ID  m_userId;        //用户ID
    	T_SERVER_ID  m_acctId;        //帐户ID
    }m_record;

  private:
	  static Mdb_Property m_property;
};

#endif //Mdb_PayRelation_All_H_INCLUDED

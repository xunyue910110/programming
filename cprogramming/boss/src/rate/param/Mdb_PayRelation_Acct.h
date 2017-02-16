#ifndef Mdb_PayRelation_Acct_H_INCLUDED
#define Mdb_PayRelation_Acct_H_INCLUDED

#include "Mdb_Common.h"
#include "FieldsXX.h"


class Mdb_PayRelation_Acct
{
  public:
    bool getProperty(Mdb_Property &property) const;

    bool convert(Session* session);
    
    void setKeyParams(InputParams *pInputParamsseIndex) const;
    
    void setOtherParams(InputParams *pInputParamsseOther) const;

    Mdb_PayRelation_Acct(){}

    Mdb_PayRelation_Acct(const Mdb_PayRelation_Acct& right);

    void operator=(const Mdb_PayRelation_Acct& right);
    
    friend ostream& operator<<(ostream& os,const Mdb_PayRelation_Acct& po);
    struct 
    {
    	T_SERVER_ID  m_userId;        //用户ID
    	T_SERVER_ID  m_acctId;        //帐户ID
    	int          m_discntPriority;//用户优惠优先级
    	char         m_actTag;        //作用标记
    	int          m_startCycId;    //生效帐期
    	int          m_endCycId;      //失效帐期
    }m_record;

  private:
	  static Mdb_Property m_property;
};

#endif //Mdb_PayRelation_Acct_H_INCLUDED

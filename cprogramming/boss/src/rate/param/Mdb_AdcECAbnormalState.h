#ifndef Mdb_AdcECAbnormalState_H
#define Mdb_AdcECAbnormalState_H

#include "Mdb_Common.h"
#include "FieldsXX.h"

class Mdb_AdcECAbnormalState
{
  public:
    bool getProperty(Mdb_Property &property) const;

    bool convert(Session* session);
    
    void setKeyParams(InputParams *pInputParamsseIndex) const;
    
    void setOtherParams(InputParams *pInputParamsseOther) const;

    Mdb_AdcECAbnormalState(){}

    Mdb_AdcECAbnormalState(const Mdb_AdcECAbnormalState& right);

    void operator=(const Mdb_AdcECAbnormalState& right);
    
    friend ostream& operator<<(ostream& os,const Mdb_AdcECAbnormalState& po);
   struct 
  		{
  			char            m_userState; 		    
        T_SERVER_ID		  m_ecUserId;
        String<18>     	m_bizCode;			
  	  } m_record;

  private:
	static Mdb_Property m_property;
};

#endif

#ifndef Mdb_AdcECuserNum_H
#define Mdb_AdcECuserNum_H

#include "Mdb_Common.h"
#include "FieldsXX.h"

class Mdb_AdcECuserNum
{
  public:
    bool getProperty(Mdb_Property &property) const;

    bool convert(Session* session);
    
    void setKeyParams(InputParams *pInputParamsseIndex) const;
    
    void setOtherParams(InputParams *pInputParamsseOther) const;

    Mdb_AdcECuserNum(){}

    Mdb_AdcECuserNum(const Mdb_AdcECuserNum& right);

    void operator=(const Mdb_AdcECuserNum& right);
    
    friend ostream& operator<<(ostream& os,const Mdb_AdcECuserNum& po);
  		struct 
  		{
  			int            m_userNum; 		    
        T_SERVER_ID         m_ecUserId;
        String<18>     m_bizCode;			
  	  } m_record;

  private:
	static Mdb_Property m_property;
};
#endif

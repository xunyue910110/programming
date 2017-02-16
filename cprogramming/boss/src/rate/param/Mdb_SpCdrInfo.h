#ifndef Mdb_SpCdrInfo_H
#define Mdb_SpCdrInfo_H


#include "Mdb_Common.h"
#include "FieldsXX.h"

class Mdb_SpCdrInfo
{
  public:
    bool getProperty(Mdb_Property &property) const;

    bool convert(Session* session);
    
    void setKeyParams(InputParams *pInputParamsseIndex) const;
    
    void setOtherParams(InputParams *pInputParamsseOther) const;

    Mdb_SpCdrInfo(){}

    Mdb_SpCdrInfo(const Mdb_SpCdrInfo& right);

    void operator=(const Mdb_SpCdrInfo& right);
    
    friend ostream& operator<<(ostream& os,const Mdb_SpCdrInfo& po);
  		struct
  		{
  			String<20>		m_serialNumber;
				String<18>		m_spCode;
			  String<18>		m_spBizCode;
			  int						m_cycId;
  		  int						m_cdrInfo;
  	  } m_record;
  private:
	static Mdb_Property m_property;
};
#endif
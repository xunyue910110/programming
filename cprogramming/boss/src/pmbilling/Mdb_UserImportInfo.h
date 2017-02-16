#ifndef MDB_USERIMPORTINFO_H_INCLUDED
#define MDB_USERIMPORTINFO_H_INCLUDED

#include "Mdb_Common.h"
#include "FieldsXX.h"

class Mdb_UserImportInfo
{
  public:
    bool getProperty(Mdb_Property &property) const;

    bool convert(Session* session);
    
    void setKeyParams(InputParams *pInputParamsseIndex) const;
    
    void setOtherParams(InputParams *pInputParamsseOther) const;

    Mdb_UserImportInfo(){}

    Mdb_UserImportInfo(const Mdb_UserImportInfo& right);

    void operator=(const Mdb_UserImportInfo& right);
    
    friend ostream& operator<<(ostream& os,const Mdb_UserImportInfo& po);
    
    bool friend operator > (const Mdb_UserImportInfo& left ,const Mdb_UserImportInfo& right);
    
    struct UserImportInfo
    {
    	friend bool operator > (const UserImportInfo &r_left,const UserImportInfo &r_right)
    	{
      	if (r_left.m_userId == r_right.m_userId)
      	{
        	return (r_left.m_startDate > r_right.m_startDate);
      	}
      	else
      	{
        	return (r_left.m_userId > r_right.m_userId);
      	}        
    	} 
    	T_SERVER_ID    m_userId;
    	T_PRODUCT_ID   m_productId;
    	T_BRAND_CODE   m_brandCode;
    	T_MSISDN       m_msisdn;
    	T_NET_TYPE_CODE m_netTypeCode;
    	T_DATE_TIME    m_startDate;
    	T_DATE_TIME    m_endDate;
    }m_record;

  private:
	static Mdb_Property m_property;
};

#endif //MDB_USERIMPORTINFO_H_INCLUDED

#ifndef _MDB_USERIMINFOBYIMSI_H_
#define _MDB_USERIMINFOBYIMSI_H_

#include "Mdb_Common.h"
#include "FieldsXX.h"

class Mdb_UserImInfobyImsi
{
  public:
    bool getProperty(Mdb_Property &property) const;

    bool convert(Session* session);
    
    void setKeyParams(InputParams *pInputParamsseIndex) const;
    
    void setOtherParams(InputParams *pInputParamsseOther) const;

    Mdb_UserImInfobyImsi(){}

    Mdb_UserImInfobyImsi(const Mdb_UserImInfobyImsi& right);

    void operator=(const Mdb_UserImInfobyImsi& right);
    
    friend ostream& operator<<(ostream& os,const Mdb_UserImInfobyImsi& po);

    friend bool operator < (const Mdb_UserImInfobyImsi& left,const Mdb_UserImInfobyImsi& right);

    struct
    {
	  	long           m_userId;       //用户标识
	  	T_SERV_TYPE    m_net_type_code; //网别编码 01/GSM 02/CDMA 03/固定电话 04/宽带
	  	int            m_productId;    //产品标识
	  	T_BRAND_CODE   m_brand_code;   //品牌编码
	  	T_MSISDN       m_logic_phone;  //01 02手机号码 03固话号码 04宽带号码...
	  	T_MSISDN       m_phyical_phone;//01 IMSI 02 MIN 03固话物理号码 04宽带帐号...
	  	T_DATE_TIME    m_beginTime;    //生效时间
	  	T_DATE_TIME    m_endTime;      //结束时间
    }m_record;

  private:
	static Mdb_Property m_property;
};

#endif //_MDB_USERIMINFOBYIMSI_H_
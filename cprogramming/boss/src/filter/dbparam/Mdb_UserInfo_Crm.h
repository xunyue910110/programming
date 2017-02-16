#ifndef _MDB_USERINFO_CRM_H_200800815_
#define _MDB_USERINFO_CRM_H_200800815_

#include "Mdb_Common.h"
#include "FieldsXX.h"

class Mdb_UserInfo_Crm
{
  public:
    bool getProperty(Mdb_Property &property) const;

    bool convert(Session* session);

    void setKeyParams(InputParams *pInputParamsseIndex) const;

    void setOtherParams(InputParams *pInputParamsseOther) const;

    Mdb_UserInfo_Crm(){}

    Mdb_UserInfo_Crm(const Mdb_UserInfo_Crm& right);

    void operator=(const Mdb_UserInfo_Crm& right);

    friend ostream& operator<<(ostream& os,const Mdb_UserInfo_Crm& po);

    struct
    {
	  	long           m_userId;       //用户标识
	  	T_SERV_TYPE    m_net_type_code;//服务类型 01/GSM 02/CDMA 03/固定电话 04/宽带
	    T_EPARCHY_CODE m_eparchy_code; //地市编码 用于分省内通道
	    T_OFFICE_CODE  m_office_code;  //市县编码 用户资料中获取
	    long           m_custId;       //客户标识
	    T_BRAND_CODE   m_brand_code;   //品牌编码
	    T_FLAG         m_open_mode;    //开户方式：0-正常，1-预开未返单，2-预开已返单，3-过户新增，4-当日返单并过户
	    T_FLAG         m_acct_tag;     //出帐标志：0-正常处理，1-定时激活，2-待激活用户，Z-不出帐
	  	int            m_productId;    //产品标识
	  	long           m_inTime;       //建档时间
	    long           m_openTime;     //开户时间
	    T_FLAG         m_prepay_tag;   //预付费标志：0-后付费，1-预付费
	    T_SERVICE_CODE m_user_type_code;  //用户类型 0-普通用户
    }m_record;

  private:
	static Mdb_Property m_property;
};

#endif //_MDB_USERINFO_CRM_H_200800815_
#ifndef _LOCALIZEPARAMMANAGER_019_H_
#define _LOCALIZEPARAMMANAGER_019_H_

#include "PMHelperall.h"
#include "CdrProperty.h"

#include "Mdb_Common.h"
#include "FieldsXX.h"

//////////////////////////////////////////////////////////////////////////
class PO_SpServCodeSpname
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_SpServCodeSpname(){}

    PO_SpServCodeSpname(const PO_SpServCodeSpname& right);

    void operator=(const PO_SpServCodeSpname& right);

    friend int operator==(const PO_SpServCodeSpname &left,
    		const PO_SpServCodeSpname& right);

    friend int operator<(const PO_SpServCodeSpname &left,
    		const PO_SpServCodeSpname& right);

	friend ostream& operator<<(ostream& os,const PO_SpServCodeSpname& po);

    SpProperty  recordStruct;

  private:
	static PO_Property m_property;
};


//////////////////////////////////////////////////////////////////////////
class PO_ServiceCodeSpname
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_ServiceCodeSpname(){}

    PO_ServiceCodeSpname(const PO_ServiceCodeSpname& right);

    void operator=(const PO_ServiceCodeSpname& right);

    friend int operator==(const PO_ServiceCodeSpname &left,
    		const PO_ServiceCodeSpname& right);

    friend int operator<(const PO_ServiceCodeSpname &left,
    		const PO_ServiceCodeSpname& right);

	friend ostream& operator<<(ostream& os,const PO_ServiceCodeSpname& po);

    SpProperty  recordStruct;

  private:
	static PO_Property m_property;
};

//////////////////////////////////////////////////////////////////////////
class PO_TrunkUser
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_TrunkUser(){}

    PO_TrunkUser(const PO_TrunkUser& right);

    void operator=(const PO_TrunkUser& right);

    friend int operator==(const PO_TrunkUser &left,
    		const PO_TrunkUser& right);

    friend int operator<(const PO_TrunkUser &left,
    		const PO_TrunkUser& right);

	friend ostream& operator<<(ostream& os,const PO_TrunkUser& po);
    struct
    {
    	T_ATTRI_VAL begin_phone;
    	T_ATTRI_VAL end_phone;
    	T_ATTRI_VAL usertype;
    	T_ATTRI_VAL area_code;
	    T_DATE_TIME m_beginDate;
	    T_DATE_TIME m_endDate;
    }recordStruct;
  private:
	static PO_Property m_property;
};


//////////////////////////////////////////////////////////////////////////
class PO_BpsPstnNumseg
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_BpsPstnNumseg(){}

    PO_BpsPstnNumseg(const PO_BpsPstnNumseg& right);

    void operator=(const PO_BpsPstnNumseg& right);

    friend int operator==(const PO_BpsPstnNumseg &left,
    		const PO_BpsPstnNumseg& right);

    friend int operator<(const PO_BpsPstnNumseg &left,
    		const PO_BpsPstnNumseg& right);

	friend ostream& operator<<(ostream& os,const PO_BpsPstnNumseg& po);

    struct
    {
    	T_ATTRI_VAL   m_areacode;
    	T_ATTRI_VAL   m_numberhead;
    	T_ATTRI_VAL   m_businessareacode;
    	T_ATTRI_VAL   m_operatorid;
    	T_ATTRI_VAL   m_nettype;
    }recordStruct;

  private:
	static PO_Property m_property;
};

//////////////////////////////////////////////////////////////////////////
class PO_BpsRelation
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_BpsRelation(){}

    PO_BpsRelation(const PO_BpsRelation& right);

    void operator=(const PO_BpsRelation& right);

    friend int operator==(const PO_BpsRelation &left,
    		const PO_BpsRelation& right);

    friend int operator<(const PO_BpsRelation &left,
    		const PO_BpsRelation& right);

	friend ostream& operator<<(ostream& os,const PO_BpsRelation& po);

    struct
    {
    	T_ATTRI_VAL   m_areacode;
    	T_ATTRI_VAL   m_businessareacode1;
    	T_ATTRI_VAL   m_businessareacode2;
    	T_ATTRI_VAL   m_connecttype;
    }recordStruct;

  private:
	static PO_Property m_property;
};

//////////////////////////////////////////////////////////////////////////
class PO_SpBillRule
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_SpBillRule(){}

    PO_SpBillRule(const PO_SpBillRule& right);

    void operator=(const PO_SpBillRule& right);

    friend int operator==(const PO_SpBillRule &left,
    		const PO_SpBillRule& right);

    friend int operator<(const PO_SpBillRule &left,
    		const PO_SpBillRule& right);

	friend ostream& operator<<(ostream& os,const PO_SpBillRule& po);

    struct
    {
    	T_ATTRI_VAL   m_source;
    	T_ATTRI_VAL   m_spcode;
    	T_ATTRI_VAL   m_sp_srvtype;
    	T_ATTRI_VAL   m_billflag;
    	T_DATE_TIME   beginTime;
    	T_DATE_TIME   endTime;
    }recordStruct;

  private:
	static PO_Property m_property;
};

//////////////////////////////////////////////////////////////////////////
class PO_LocalSpOperFee
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_LocalSpOperFee(){}

    PO_LocalSpOperFee(const PO_LocalSpOperFee& right);

    void operator=(const PO_LocalSpOperFee& right);
    
    friend int operator==(const PO_LocalSpOperFee &left,const PO_LocalSpOperFee &right);
    
    friend int operator<(const PO_LocalSpOperFee &left,const PO_LocalSpOperFee& right);
    
    friend ostream& operator<<(ostream& os,const PO_LocalSpOperFee& po);
      
  struct
  {
  	T_ATTRI_VAL    spCode;
  	T_ATTRI_VAL    operCode;
  	T_ATTRI_VAL    serviceCode;
  	T_ATTRI_VAL    feeType;
  	int            infoFee;
  	char           infoCheckType;
  	int            giftFee;
  	char           giftCheckType;
    T_DATE_TIME    m_beginTime;
    T_DATE_TIME    m_endTime;
    char           sp_serv_area; //吉林放到此处区分本地和全网SP业务
  }recordStruct;

  private:
    static PO_Property m_property;
};
class Mdb_UserMemberIvpn
{
  public:
    bool getProperty(Mdb_Property &property) const;

    bool convert(Session* session);

    void setKeyParams(InputParams *pInputParamsseIndex) const;

    void setOtherParams(InputParams *pInputParamsseOther) const;

    Mdb_UserMemberIvpn(){}

    Mdb_UserMemberIvpn(const Mdb_UserMemberIvpn& right);

    void operator=(const Mdb_UserMemberIvpn& right);

    friend int operator<(const Mdb_UserMemberIvpn &left,const Mdb_UserMemberIvpn& right);

    friend ostream& operator<<(ostream& os,const Mdb_UserMemberIvpn& po);

    struct
    {
      long            m_userId;         // 三户标识         user_id
      T_MEMBER_TYPE   m_memberType;     // 成员角色         member_role_code //前2位40代表IVPN用户
      T_MEMBER_TYPE   m_memberIdType;   // 成员角色标识类型 member_role_type
      long            m_memberId;       // 成员标识         member_role_id
      T_OTHER_PARTY   m_memberNumber;   // 成员号码         member_role_number
    	T_DATE_TIME     m_beginTime;      // 起始时间       start_date
    	T_DATE_TIME     m_endTime;        // 终止时间       end_date
    	//T_MEMBER_TYPE   m_lastMemberType; // 最底层角色类型

    }m_record;

  private:
	static Mdb_Property m_property;
};
class Mdb_UserInfoSN
{
  public:
    bool getProperty(Mdb_Property &property) const;

    bool convert(Session* session);

    void setKeyParams(InputParams *pInputParamsseIndex) const;

    void setOtherParams(InputParams *pInputParamsseOther) const;

    Mdb_UserInfoSN(){}

    Mdb_UserInfoSN(const Mdb_UserInfoSN& right);

    void operator=(const Mdb_UserInfoSN& right);

    friend ostream& operator<<(ostream& os,const Mdb_UserInfoSN& po);

    struct
    {
	  	T_ATTRI_VAL    m_sn;       //用户号码
	  	T_SERV_TYPE    m_net_type_code;//服务类型 01/GSM 02/CDMA 03/固定电话 04/宽带
	  	T_FLAG         m_user_type_code;  //wujg add 20090531
	    T_EPARCHY_CODE m_eparchy_code; //地市编码 用于分省内通道
	    T_OFFICE_CODE  m_office_code;  //市县编码 用户资料中获取
	    long           m_custId;       //客户标识
	    T_BRAND_CODE   m_brand_code;   //品牌编码
	    T_FLAG         m_open_mode;    //开户方式：0-正常，1-预开未返单，2-预开已返单，3-过户新增，4-当日返单并过户
	    T_FLAG         m_acct_tag;     //出帐标志：0-正常处理，1-定时激活，2-待激活用户，Z-不出帐
	  	int            m_productId;    //产品标识
	  	long           m_inTime;       //建档时间
	    long           m_openTime;     //开户时间
    }m_record;

  private:
	static Mdb_Property m_property;
};

//用于获取话单类型 ,山西特定需求。added by shijianchun in 20090926
class PO_LocalCdrType
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_LocalCdrType(){}

    PO_LocalCdrType(const PO_LocalSpOperFee& right);

    void operator=(const PO_LocalCdrType& right);

    friend int operator==(const PO_LocalCdrType &left,const PO_LocalCdrType &right);
    
    friend int operator<(const PO_LocalCdrType &left,const PO_LocalCdrType& right);
    
    friend ostream& operator<<(ostream& os,const PO_LocalCdrType& po);

  struct
  {
  	int          nRecord_type;
  	T_PRI        m_priority; 
  	T_ATTRI_VAL  m_bizType;
    T_ATTRI_VAL  m_sourceType;
    T_ATTRI_VAL  m_msc;
    T_ATTRI_VAL  m_trunkGroupIn;
    T_ATTRI_VAL  m_callType;
    T_ATTRI_VAL  m_userAspB;
    T_ATTRI_VAL  m_UserServiceTypeB;
    T_ATTRI_VAL  m_paType;
    T_ATTRI_VAL  m_longType;
    T_ATTRI_VAL  m_longGroup;
    T_ATTRI_VAL  m_carrierType;
    T_ATTRI_VAL  m_homeCodeA;
    T_ATTRI_VAL  m_homeCodeB;
    T_ATTRI_VAL  m_ruleId;  
  }recordStruct;

  private:
    static PO_Property m_property;
};

#endif /* _LOCALIZEPARAMMANAGER_019_H_ */

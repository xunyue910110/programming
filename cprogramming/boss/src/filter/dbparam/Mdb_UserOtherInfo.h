#ifndef _MDB_USEROTHERINFO_H_200801029_
#define _MDB_USEROTHERINFO_H_200801029_

#include "Mdb_Common.h"
#include "FieldsXX.h"

const string D_SERV_TYPE_GSM          ("01");
const string D_SERV_TYPE_CDMA         ("02");
const string D_SERV_TYPE_FIX          ("03");
const string D_SERV_TYPE_BROADBAND    ("04");

class Mdb_UserOtherInfo
{
  public:
    bool getProperty(Mdb_Property &property) const;

    bool convert(Session* session);

    void setKeyParams(InputParams *pInputParamsseIndex) const;

    void setOtherParams(InputParams *pInputParamsseOther) const;

    Mdb_UserOtherInfo(){}

    Mdb_UserOtherInfo(const Mdb_UserOtherInfo& right);

    void operator=(const Mdb_UserOtherInfo& right);

    friend ostream& operator<<(ostream& os,const Mdb_UserOtherInfo& po);

   /*
   m_otherinfo_type
   0:预付费用户有效性，
     ATTR_VALUE1 '0'代表无效 '1'代表有效
   1:手机串号信息(湖南特殊需求,购机用户本机本号码通话优惠)
     ATTR_VALUE1:手机号码
     ATTR_VALUE2:手机串号
   2:信息服务状态信息（用户信息服务状态限定，考虑放入服务状态表）
     ATTR_VALUE1:服务类型 03-WAP 04-SMS 05-MMS 13-手机动画 99-全业务
     ATTR_VALUE2:许可标志 0-允许 1-禁止
   3:联通转接表信息（联通转网用户呼转优惠）
     ATTR_VALUE1:用户现移动号码
     ATTR_VALUE2:用户原联通号码
   4:特殊边界漫游用户(天津特殊需求:用户投诉边界漫游，填入该表做优惠)
     ATTR_VALUE1:用户号码
     ATTR_VALUE2:业务类型
   */
    struct
    {
	  	long           m_userId;        //用户标识
	  	T_USER_TYPE    m_otherinfo_type;//0:预付费用户 1:手机串号信息 2:用户信息服务许可状态 3:联通转接表信息 4:特殊边界漫游用户
	  	T_ATTRI_VAL    m_attr_value1;
	  	T_ATTRI_VAL    m_attr_value2;
	  	T_ATTRI_VAL    m_attr_value3;
	  	T_DATE_TIME    m_beginTime;    //生效时间
	  	T_DATE_TIME    m_endTime;      //结束时间
    }m_record;

  private:
	static Mdb_Property m_property;
};

#endif //_MDB_USEROTHERINFO_H_200801029_
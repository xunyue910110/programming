#ifndef _CDRPROC_H_200800723_
#define _CDRPROC_H_200800723_

#include "config-all.h"
//#include "base/TimeUtil.h"
#include "ProcessBase.h"
#include "OutChanInfo.h"

#include "UserProperty.h"

class CDRProc:public ProcessBase
{
  public:
    CDRProc();
    virtual ~CDRProc();

    virtual bool initialize();
		virtual int  reset();        //话单共有属性重置
    virtual int  execute();

  protected:
		virtual bool getChanByRule(); //根据正则获取通道
		virtual	bool getTrash();
    virtual bool getError();
		
		//是否为国内imsi段
		bool isNationalImsi(const T_ATTRI_VAL &r_imsi);
		//是否为运营商手机号码
		bool isSelfMobile(const T_ATTRI_VAL &phoneNubmer);
		bool isMobile(const T_ATTRI_VAL &phoneNubmer);
  
  protected:
		T_ATTRI_VAL      homeType;
		T_ASP            aspType;
		//T_TD_TYPE        tdType;     //td用户不再根据号段区分 取消
    T_ATTRI_VAL      groupId;
    T_ATTRI_VAL      areaCode;
    T_ATTRI_VAL      userType;
    T_ATTRI_VAL      officeCode;
    T_ATTRI_VAL      provinceCode;
		T_ATTRI_VAL      calledType;
		T_HOME_TYPE      sgsnhomeType;

  	UserServInfo m_userServInfo;  //计费方资料
  	UserServInfo m_otherServInfo; //对端资料
};


#endif //_CDRPROC_H_200800723_
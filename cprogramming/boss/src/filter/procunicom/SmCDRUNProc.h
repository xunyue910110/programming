#ifndef _SMCDRUNPROC_H_200800723_
#define _SMCDRUNPROC_H_200800723_

#include "config-all.h"
#include "ProcessBase.h"
#include "CDRProc.h"

#include "CdrProperty.h"

class SmCDRUNProc:public CDRProc
{
  public:
    virtual int execute();

  protected:
    bool InitCdrProperty(); //话单初始化

    bool getDisChannelProperty();

    bool getSmsMethod();    //获得短信处理方法
    
    bool getTrash();

    bool getSmServType();    //点对点短信话单，根据号码获得该号码其余属性
    bool getSpServType();    //梦网话单，根据SP_CODE和业务代码获得话单属性
    bool getUssdServType();  //USSD话单，根据SP_CODE或者SERVICE_CODE获得话单属性
    bool getHyOperCode();    //行业网关话单，根据业务代码获得话单的计费属性
    bool getPpsSmServType(); //PPS短信合并处理逻辑，比较复杂，因此新增处理逻辑
    bool getFixSmServType(); //固网点对点短信话单 add by shenglj 2009-04-02 10:49:01
    bool getFixSpServType(); //固网网关短信话单 add by shenglj 2009-04-02 10:49:01

    bool checkMsisdn(const T_ATTRI_VAL &r_msisdn); //检查计费方
    bool setMsisdn();       //设定计费方
    bool setSpMsisdn();     //设定SP话单计费方
    bool checkFixPhone(const T_ATTRI_VAL &r_phone); //检查固网计费方

   //点对点短信
    bool ParseLinkage();   //处理互通话单
    bool ParseNumber();    //处理点对点话单
    bool ParsePpsNumber(); //处理PPS点对点短信
    //根据对方号码获得业务分类，业务类型,归属类型,运营商,应用类型,统计类
    bool setSmServType(const T_ATTRI_VAL &r_msisdn, SmProperty &r_smProperty);
    //设定点对点短信话单属性
    bool setSmCdrProperty(const T_CALL_TYPE &r_calltype,
            const SmProperty &r_msisdnProperty,
            const SmProperty &r_otherProperty);
    
    //梦网短信
    bool setSpServType(); //求SP属性

  protected:
    SmProperty  m_origProperty;
    SmProperty  m_destProperty;
    SpProperty  m_spProperty;
    int         m_smsMethod;

};

#endif //_SMCDRUNPROC_H_200800723_

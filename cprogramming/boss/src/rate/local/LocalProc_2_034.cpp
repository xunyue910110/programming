#include "LocalProc_2_034.h"

///////////////////////////////////////////////////////////////////////////
INSTALL_PROCEDURE_INSTANCE(RateCdrAttribInit);
INSTALL_PROCEDURE_INSTANCE(RateCdrAttribEnd);
INSTALL_PROCEDURE_INSTANCE(SetOriginFeeFlag);
INSTALL_PROCEDURE_INSTANCE(SpecialBillProc);
INSTALL_PROCEDURE_INSTANCE(LocalizeGsmFreeFlagProc);
INSTALL_PROCEDURE_INSTANCE(IsNoNeedTPSpecialProc);

//读取话单后的一些初始化处理
int RateCdrAttribInit::execute()
{
  DateTime t_beginDateTime;
  DateTime t_endDateTime;
 if(m_pcdr[ATTRI_BIZ_TYPE] == BIZ_TYPE_UNICOMSP)
 {
   DateTime t_beginDateTime;
   DateTime t_endDateTime;
   if(m_pcdr[ATTRI_SP_RATEUNIT] == SP_RATE_UNIT_TIME ||
      m_pcdr[ATTRI_SP_RATEUNIT] == SP_RATE_UNIT_CLTIME)
    { //时长
    }else if(m_pcdr[ATTRI_SP_RATEUNIT] == SP_RATE_UNIT_QRT ||
             m_pcdr[ATTRI_SP_RATEUNIT] == SP_RATE_UNIT_CLQRT)
    { //按次数,采用变相按时长方式实现
      m_pcdr[ATTRI_DURATION] = m_pcdr[ATTRI_SP_QTY];
    }else if(m_pcdr[ATTRI_SP_RATEUNIT] == SP_RATE_UNIT_DATA ||
             m_pcdr[ATTRI_SP_RATEUNIT] == SP_RATE_UNIT_CLDATA)
    { //流量
    }else 
    { //其他认为是按条的
      //if(m_pcdr[ATTRI_SP_RATEUNIT] == SP_RATE_UNIT_RECORD ||
      //   m_pcdr[ATTRI_SP_RATEUNIT] == SP_RATE_UNIT_CLRECORD)
      //按条,采用变相按时长方式实现
      m_pcdr[ATTRI_DURATION] = 1;
    }
    t_beginDateTime.set(m_pcdr[ATTRI_BEGIN_DATE].c_str(),m_pcdr[ATTRI_BEGIN_TIME].c_str());
    t_endDateTime = t_beginDateTime+m_pcdr[ATTRI_DURATION].value();
    m_pcdr[ATTRI_END_DATE] = t_endDateTime.getDateInteger();
    m_pcdr[ATTRI_END_TIME] = t_endDateTime.getTimeInteger();
 }
 
 if(m_pcdr[ATTRI_BIZ_TYPE] == BIZ_TYPE_GSM)
 {
    //含在语音里的国际短信
    if(m_pcdr[ATTRI_CALL_TYPE] == CALL_TYPE_SMMO ||
       m_pcdr[ATTRI_CALL_TYPE] == CALL_TYPE_SMMT)
    {
      m_pcdr[ATTRI_DURATION] = 1;
      t_beginDateTime.set(m_pcdr[ATTRI_BEGIN_DATE].c_str(),m_pcdr[ATTRI_BEGIN_TIME].c_str());
      t_endDateTime = t_beginDateTime+m_pcdr[ATTRI_DURATION].value();
      m_pcdr[ATTRI_END_DATE] = t_endDateTime.getDateInteger();
      m_pcdr[ATTRI_END_TIME] = t_endDateTime.getTimeInteger();
      m_pcdr[ATTRI_FEEUNIT_1] = 1;
    }
  }
 return 0;
}
//话单批价处理结束后的一些特殊处理
int RateCdrAttribEnd::execute()
{
 long t_totalData=0;
 if(m_pcdr[ATTRI_BIZ_TYPE] == BIZ_TYPE_GPRS)
 {
   m_pcdr[ATTRI_GPRS_CHARGED_ITEM] = 'X';
   m_pcdr[ATTRI_GPRS_CHARGED_OPER] = m_pcdr[ATTRI_GPRS_SERVICETYPE];
   t_totalData = m_pcdr[ATTRI_GPRS_DATA_UP1].value()+m_pcdr[ATTRI_GPRS_DATA_DOWN1].value()+
                 m_pcdr[ATTRI_GPRS_DATA_UP2].value()+m_pcdr[ATTRI_GPRS_DATA_DOWN2].value();
   if(t_totalData % DEFAULT_DATA_UNIT >0)
   {
     t_totalData = t_totalData+DEFAULT_DATA_UNIT-t_totalData%DEFAULT_DATA_UNIT;
   }
   m_pcdr[ATTRI_GPRS_CHARGED_UNITS]= t_totalData;
 }
 return 0;
}

//判断是否需要参加套餐优惠,特殊处理
int IsNoNeedTPSpecialProc::execute()
{
  
  return 0;
}

//GSM一些免费标记设置(比如3秒免费等)
int LocalizeGsmFreeFlagProc::execute()
{
  //国内出访话单三秒由于long_type置为0,因此前面不能判断出来
  //这里进行判断  国内出访小于3秒(不含3秒) 原始费用为0
  if  ((m_pcdr[ATTRI_ROAM_TYPE] == ROAM_TYPE_NATIONAL_OUT) && 
       (m_pcdr[ATTRI_DURATION]  < DURATION_FREE)&& 
       (m_pcdr[ATTRI_FEE_FIELD1]==0) &&
       (m_pcdr[ATTRI_FEE_FIELD3]==0) &&
       (m_pcdr[ATTRI_FEE_FIELD5]==0))
  {
      m_pcdr[ATTRI_FREE_FEE] = FLAG_YES;
      return 1;
  }
  
  //联通17911 17913 200 国内长途7秒(不含7秒)免费
  if((m_pcdr[ATTRI_ROAM_TYPE] <= ROAM_TYPE_NATIONAL_OUT) && 
     (m_pcdr[ATTRI_DURATION]  < 7)&& 
     (m_pcdr[ATTRI_LONG_TYPE1] != LONG_TYPE_NONE)&&
     (m_pcdr[ATTRI_CARRIER_TYPE] == CARRIER_TYPE_IP_17911 ||
      m_pcdr[ATTRI_CARRIER_TYPE] == CARRIER_TYPE_IP_17913 ||
      m_pcdr[ATTRI_CARRIER_TYPE] == CARRIER_TYPE_IP_200) )
  {
      m_pcdr[ATTRI_FREE_FEE] = FLAG_YES;
      return 1;
  }

  return 0;
}

//设置那些费用项有原始费用
int SetOriginFeeFlag::execute()
{
  DateTime t_beginDateTime;
  DateTime t_endDateTime;
  if(m_pcdr[ATTRI_BIZ_TYPE] == BIZ_TYPE_GSM)
  {
    if((m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_GSM_CRO )||
       (m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_GSM_CIO )||
       (m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_GSM_GBO ))
    { //总部下发的国内国际出访话单
     if(m_pcdr[ATTRI_ROAM_TYPE] == ROAM_TYPE_NONE)
     { //省际边界漫游需要重新批价
     }else
     {
        m_pRateInfo->setOrgFeeFlag(ATTRI_DISCFEE_FIELD1,1);
        m_pcdr[ATTRI_FEEUNIT_1] = DEFAULT_CFEE_UNIT;
        if(!(m_pcdr[ATTRI_LONG_TYPE1] == LONG_TYPE_NONE))
        {
          m_pRateInfo->setOrgFeeFlag(ATTRI_DISCFEE_FIELD3,1);
        }
        //有第二段长途或有信息费
        if(!(m_pcdr[ATTRI_LONG_TYPE2] == LONG_TYPE_NONE) ||
            m_pcdr[ATTRI_ORGFEE_FIELD5].value()>0)
        {
          m_pRateInfo->setOrgFeeFlag(ATTRI_DISCFEE_FIELD5,1);
        }
      }
    }else if(m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_GSM_GZHEJ_L )
    {  //军网长话单有一次批价费用17、短话单无原始费用16
      m_pRateInfo->setOrgFeeFlag(ATTRI_DISCFEE_FIELD1,1);
      m_pcdr[ATTRI_FEEUNIT_1] = DEFAULT_CFEE_UNIT;
      if(!(m_pcdr[ATTRI_LONG_TYPE1] == LONG_TYPE_NONE))
      {
        m_pRateInfo->setOrgFeeFlag(ATTRI_DISCFEE_FIELD3,1);
      }
      //有第二段长途或有信息费
      if(!(m_pcdr[ATTRI_LONG_TYPE2] == LONG_TYPE_NONE) ||
          m_pcdr[ATTRI_ORGFEE_FIELD5].value()>0)
      {
        m_pRateInfo->setOrgFeeFlag(ATTRI_DISCFEE_FIELD5,1);
      }
    }else
    { //if(m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_GSM_IVPN )
      //和普通端局
      //无原始费用
    } 
    //置长途缺省单位时长
    if(m_pcdr[ATTRI_CARRIER_TYPE] == CARRIER_TYPE_IP_17911 ||
       m_pcdr[ATTRI_CARRIER_TYPE] == CARRIER_TYPE_IP_17913 ||
       m_pcdr[ATTRI_CARRIER_TYPE] == CARRIER_TYPE_IP_200 )
    {
      m_pcdr[ATTRI_FEEUNIT_3] = DEFAULT_IPFEE_UNIT;
      m_pcdr[ATTRI_FEEUNIT_5] = DEFAULT_IPFEE_UNIT;
    }else
    {
      m_pcdr[ATTRI_FEEUNIT_3] = DEFAULT_LFEE_UNIT;
      m_pcdr[ATTRI_FEEUNIT_5] = DEFAULT_LFEE_UNIT;
    }
  }else if(m_pcdr[ATTRI_BIZ_TYPE] == BIZ_TYPE_SMS)
  {
    //短信中心和互联网关话单无原始费用
    //行业应用短信，从梦网上行话单里分出，无原始费用
    if((m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_SMS_GSMSC) ||
       (m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_SMS_MINTER) ||
       (m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_SMS_GINTER))
    {
      //无原始费用
    }//梦网网关上行无原始费用，下行有原始费用
    else if(m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_SMS_PSMG ||
            m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_SMS_NSMG)
    {
      if ((m_pcdr[ATTRI_CALL_TYPE] == CALL_TYPE_SMO)||
          (m_pcdr[ATTRI_CALL_TYPE] == CALL_TYPE_SMOF) )
      {
        //无原始费用
      }else //下行有信息费
      {
        //m_pRateInfo->setOrgFeeFlag(ATTRI_DISCFEE_FIELD1,1);
        m_pRateInfo->setOrgFeeFlag(ATTRI_DISCFEE_FIELD2,1);
        m_pRateInfo->setOrgFeeFlag(ATTRI_DISCFEE_FIELD3,1);
        //m_pcdr[ATTRI_FEEUNIT_1] = 1;
        m_pcdr[ATTRI_FEEUNIT_2] = 1;
        m_pcdr[ATTRI_FEEUNIT_3] = 1;
      }
    }else
    { //SOURCE_TYPE_SMS_IINTER 总部下发短信
      m_pRateInfo->setOrgFeeFlag(ATTRI_DISCFEE_FIELD1,1);
      m_pRateInfo->setOrgFeeFlag(ATTRI_DISCFEE_FIELD2,1);
      m_pRateInfo->setOrgFeeFlag(ATTRI_DISCFEE_FIELD3,1);
      m_pcdr[ATTRI_FEEUNIT_1] = 1;
      m_pcdr[ATTRI_FEEUNIT_2] = 1;
      m_pcdr[ATTRI_FEEUNIT_3] = 1;
    }
  }else if(m_pcdr[ATTRI_BIZ_TYPE] == BIZ_TYPE_GPRS)
  {
    //联通所有GPRS均在计费系统批价
    //无原始费用
  }else if(m_pcdr[ATTRI_BIZ_TYPE] == BIZ_TYPE_UNICOMIP)
  {
    //无原始费用
  }else if(m_pcdr[ATTRI_BIZ_TYPE] == BIZ_TYPE_UNICOMSP)
  { //联通SP增值业务 
    //省内彩信平台48需要一次批价
    if( m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_SP_MMS )
    { //无原始费用需要一次批价
    }else
    {
      m_pRateInfo->setOrgFeeFlag(ATTRI_DISCFEE_FIELD1,1);
      m_pRateInfo->setOrgFeeFlag(ATTRI_DISCFEE_FIELD2,1);
    }
    if(m_pcdr[ATTRI_SP_RATEUNIT] == SP_RATE_UNIT_TIME ||
       m_pcdr[ATTRI_SP_RATEUNIT] == SP_RATE_UNIT_CLTIME)
    { //时长
      m_pcdr[ATTRI_FEEUNIT_1] = DEFAULT_CFEE_UNIT;
      m_pcdr[ATTRI_FEEUNIT_2] = DEFAULT_CFEE_UNIT;
    }else if(m_pcdr[ATTRI_SP_RATEUNIT] == SP_RATE_UNIT_QRT ||
             m_pcdr[ATTRI_SP_RATEUNIT] == SP_RATE_UNIT_CLQRT)
    { //按次数,采用变相按时长方式实现
      m_pcdr[ATTRI_DURATION] = m_pcdr[ATTRI_SP_QTY];
      m_pcdr[ATTRI_FEEUNIT_1] = 1;
      m_pcdr[ATTRI_FEEUNIT_2] = 1;
    }else if(m_pcdr[ATTRI_SP_RATEUNIT] == SP_RATE_UNIT_DATA ||
             m_pcdr[ATTRI_SP_RATEUNIT] == SP_RATE_UNIT_CLDATA)
    { //流量
      m_pcdr[ATTRI_FEEUNIT_1] = DEFAULT_DATA_UNIT;
      m_pcdr[ATTRI_FEEUNIT_2] = DEFAULT_DATA_UNIT;
    }else 
    { //其他认为是按条的
      //if(m_pcdr[ATTRI_SP_RATEUNIT] == SP_RATE_UNIT_RECORD ||
      //   m_pcdr[ATTRI_SP_RATEUNIT] == SP_RATE_UNIT_CLRECORD)
      //按条,采用变相按时长方式实现
      m_pcdr[ATTRI_DURATION]  = 1;
      m_pcdr[ATTRI_FEEUNIT_1] = 1;
    }
    t_beginDateTime.set(m_pcdr[ATTRI_BEGIN_DATE].c_str(),m_pcdr[ATTRI_BEGIN_TIME].c_str());
    t_endDateTime = t_beginDateTime+m_pcdr[ATTRI_DURATION].value();
    m_pcdr[ATTRI_END_DATE] = t_endDateTime.getDateInteger();
    m_pcdr[ATTRI_END_TIME] = t_endDateTime.getTimeInteger();
  }else if(m_pcdr[ATTRI_BIZ_TYPE] == BIZ_TYPE_SPMONTH ||
  	m_pcdr[ATTRI_BIZ_TYPE] == BIZ_TYPE_MONFEE)
  { //联通SP包月费业务
    m_pRateInfo->setOrgFeeFlag(ATTRI_DISCFEE_FIELD1,1);
    m_pcdr[ATTRI_FEEUNIT_1] = 1;
  }else if(m_pcdr[ATTRI_BIZ_TYPE] == BIZ_TYPE_FIXVOICE)
  { //固网语音类
    //主要确定哪些业务是有原始费用的 coding here later
  }else if(m_pcdr[ATTRI_BIZ_TYPE] == BIZ_TYPE_FIXSMS)
  { //固网短信类
    
  }else if(m_pcdr[ATTRI_BIZ_TYPE] == BIZ_TYPE_NET)
  { //按流量计费
    m_pRateInfo->setOrgFeeFlag(ATTRI_DISCFEE_FIELD1,1);
    m_pcdr[ATTRI_FEEUNIT_1] = DEFAULT_DATA_UNIT;
  }
  
  return 0;
}

//特殊合帐过程
int SpecialBillProc::execute()
{
  return 0;
}




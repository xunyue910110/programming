#include "LocalProc_2_022.h"

///////////////////////////////////////////////////////////////////////////
INSTALL_PROCEDURE_INSTANCE(RateCdrAttribInit);
INSTALL_PROCEDURE_INSTANCE(RateCdrAttribEnd);
INSTALL_PROCEDURE_INSTANCE(SetOriginFeeFlag);
INSTALL_PROCEDURE_INSTANCE(SpecialBillProc);
INSTALL_PROCEDURE_INSTANCE(LocalizeGsmFreeFlagProc);
INSTALL_PROCEDURE_INSTANCE(IsNoNeedTPSpecialProc);

extern RateGlobalInfo g_globalInfo;

//读取话单后的一些初始化处理
int RateCdrAttribInit::execute()
{
  DateTime t_beginDateTime;
  DateTime t_endDateTime;
  long t_totalData=0;
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
//话单批价处理结束后的一些特殊处理
int RateCdrAttribEnd::execute()
{
 /*long t_totalData=0;
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
 }*/
 return 0;
}

//判断是否需要参加套餐优惠,特殊处理
int IsNoNeedTPSpecialProc::execute()
{
  //Added By Sunam 2009/11/10 16:56:27 固网800业务不作用套餐
  if ((m_pcdr[ATTRI_BIZ_TYPE] == BIZ_TYPE_FIXVOICE) && (m_pcdr[ATTRI_USER_SERVICE_TYPE_A] == "80" ))
  {
  	return 1;
  }
  return 0;
}

//GSM一些免费标记设置(比如3秒免费等)
int LocalizeGsmFreeFlagProc::execute()
{
  //国内出访话单三秒由于long_type置为0,因此前面不能判断出来
  //这里进行判断  国内出访小于3秒(不含3秒) 原始费用为0
  /*
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
  */
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
    if( m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_GPRS_PPSRET )
    {
    	m_pRateInfo->setOrgFeeFlag(ATTRI_DISCFEE_FIELD1,1);
    }
  }else if(m_pcdr[ATTRI_BIZ_TYPE] == BIZ_TYPE_UNICOMIP)
  {
    //无原始费用
  }else if(m_pcdr[ATTRI_BIZ_TYPE] == BIZ_TYPE_UNICOMSP)
  { //联通SP增值业务 
    //省内彩信平台48需要一次批价
    if( m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_SP_MMS )
    { //无原始费用需要一次批价
    }else if( m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_SP_VAC )
    {

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
    //added by hany for 3G at 2009-5-16 01:07上午
    if( m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_SP_VAC )
    {
        if ((m_pcdr[ATTRI_T_TIMES].value()+m_pcdr[ATTRI_M_TIMES].value())!=0)
        {
            //一个产品只能按M或者T计费
            if (m_pcdr[ATTRI_T_TIMES].value()!=0)
            {
                m_pRateInfo->m_hasFeeFlag[2]=false;//如果有T,费用域2就不计费
                m_pcdr[ATTRI_DURATION]=m_pcdr[ATTRI_T_TIMES].value();
                DateTime t_beginDateTime;
                DateTime t_endDateTime;
                t_beginDateTime.set(m_pcdr[ATTRI_BEGIN_DATE].c_str(),m_pcdr[ATTRI_BEGIN_TIME].c_str());
                t_endDateTime = t_beginDateTime+m_pcdr[ATTRI_DURATION].value();
                m_pcdr[ATTRI_END_DATE] = t_endDateTime.getDateInteger();
                m_pcdr[ATTRI_END_TIME] = t_endDateTime.getTimeInteger();
            }
            else if (m_pcdr[ATTRI_M_TIMES].value()!=0)
            {
                m_pRateInfo->m_hasFeeFlag[3]=false;//如果有M,费用域3就不计费
                m_pcdr[ATTRI_DURATION]=m_pcdr[ATTRI_M_TIMES].value();
                DateTime t_beginDateTime;
                DateTime t_endDateTime;
                t_beginDateTime.set(m_pcdr[ATTRI_BEGIN_DATE].c_str(),m_pcdr[ATTRI_BEGIN_TIME].c_str());
                t_endDateTime = t_beginDateTime+m_pcdr[ATTRI_DURATION].value();
                m_pcdr[ATTRI_END_DATE] = t_endDateTime.getDateInteger();
                m_pcdr[ATTRI_END_TIME] = t_endDateTime.getTimeInteger();
            }

        }
        else
        {
            m_pRateInfo->m_hasFeeFlag[2]=false;
            m_pRateInfo->m_hasFeeFlag[3]=false;
        }         
    }
  }else if(m_pcdr[ATTRI_BIZ_TYPE] == BIZ_TYPE_SPMONTH ||
  	       m_pcdr[ATTRI_BIZ_TYPE] == BIZ_TYPE_MONFEE)
  { //联通SP包月费业务
    m_pRateInfo->setOrgFeeFlag(ATTRI_DISCFEE_FIELD1,1);
    m_pcdr[ATTRI_FEEUNIT_1] = 1;
  }else if(m_pcdr[ATTRI_BIZ_TYPE] == BIZ_TYPE_FIXVOICE)
  { //固网语音类//主要确定哪些业务是有原始费用的 coding here later
    //Added By Sunam 2009/10/26 15:05:23 共享时长话单有原始费用，否则无原始费用
    if (strncmp(m_pRateInfo->m_fileName.c_str(),"TTD",3) == 0)
    {
        m_pRateInfo->setOrgFeeFlag(ATTRI_DISCFEE_FIELD1,1);
        m_pcdr[ATTRI_FEEUNIT_1] = DEFAULT_CFEE_UNIT;
    }
    //added by hany 2009/11/5 2:44 800业务有原始费用
    else if(m_pcdr[ATTRI_USER_SERVICE_TYPE_A] == "80" )
    {
    	  m_pRateInfo->setOrgFeeFlag(ATTRI_DISCFEE_FIELD1,1);
    	  m_pRateInfo->setOrgFeeFlag(ATTRI_DISCFEE_FIELD3,1);
        m_pcdr[ATTRI_FEEUNIT_1] = DEFAULT_CFEE_UNIT;
        m_pcdr[ATTRI_FEEUNIT_3] = DEFAULT_LFEE_UNIT;
    }
    //Added By Sunam 2009/11/18 12:43:10 400业务有原始长途费
    else if(m_pcdr[ATTRI_USER_SERVICE_TYPE_A] == "40" )
    {
    	m_pRateInfo->setOrgFeeFlag(ATTRI_DISCFEE_FIELD3,1);
        m_pcdr[ATTRI_FEEUNIT_3] = DEFAULT_LFEE_UNIT;
    }
  }else if(m_pcdr[ATTRI_BIZ_TYPE] == BIZ_TYPE_FIXSMS)
  { //固网短信类
    if ((m_pcdr[ATTRI_SOURCE_TYPE] == "2A") && (m_pcdr[ATTRI_CALL_TYPE] == CALL_TYPE_SMT))
    {
        m_pRateInfo->setOrgFeeFlag(ATTRI_DISCFEE_FIELD2,1);
        m_pcdr[ATTRI_FEEUNIT_1] = 1;
    }
  }else if(m_pcdr[ATTRI_BIZ_TYPE] == BIZ_TYPE_NET)
  { 
    //Added By Sunam 2009/10/5 22:40:27 宽带无原始费用
    //m_pRateInfo->setOrgFeeFlag(ATTRI_DISCFEE_FIELD1,1);
    //m_pcdr[ATTRI_FEEUNIT_1] = DEFAULT_DATA_UNIT;
  }
  else if(m_pcdr[ATTRI_BIZ_TYPE] == BIZ_TYPE_FISC)
  {
      m_pRateInfo->setOrgFeeFlag(ATTRI_DISCFEE_FIELD1,1);
      m_pcdr[ATTRI_FEEUNIT_1] = 1;
  }
  
  return 0;
}

//特殊合帐过程
int SpecialBillProc::execute()
{
	//add by zhangxy3 2009.04.24
  if( m_pcdr[ATTRI_BIZ_TYPE] == BIZ_TYPE_GPRS &&
     !(m_pcdr[ATTRI_ROAM_TYPE] == ROAM_TYPE_INTERNATIONAL_OUT)&&
     m_pRateInfo->m_userId.value()>0 ) 
  { //计算GPRS该天是否有话单
    const int BILL_ID_GPRS_FLAG = 18001; //这个帐目项对应的账单值
    //0b xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
    //   31-----------------------------0
    //第i位(i:0...31)表示第i+1天的信息，如果该位为1,表示有GPRS话单,否则无GPRS话单
    UserBill  t_userbill;
    const UserBill *t_pUserBill= NULL;
    int       t_startDay = m_pcdr[ATTRI_BEGIN_DATE].value()%100;
    t_userbill.m_userId       = m_pRateInfo->m_userId;
    t_userbill.m_billId       = BILL_ID_GPRS_FLAG;
    t_userbill.m_value        = 0;
    t_userbill.m_discntValue  = 0;
    /*add by zhangxy3 2009-5-9 14:54:43*/
    static RateRunInfo info={0,0,"",0,0,0,""};
   if ( info.m_billCycle == 0 ) 
   {
     info.m_channelNo=g_globalInfo.m_pConfig->m_channelNo;
     g_globalInfo.m_billMgr->getRateRunInfo(info);
   }
   
   #ifdef _DEBUG_
     cout<<"批价通道帐期："<<info.m_billCycle<<endl;
     cout<<"GPRS话单帐期："<<m_pcdr[ATTRI_BEGIN_DATE].substr(0,6)<<endl;
   #endif
   
   //Added By Sunam 2009/11/6 11:09:53 3G用户流量累计
   if ((m_pcdr[ATTRI_USER_SERVICE_TYPE_A_YZ] == "3G00") || (m_pcdr[ATTRI_USER_SERVICE_TYPE_A_YZ] == "3G01"))
   {
       UserSumBill1 t_sumBill1;
       t_sumBill1.m_userId = m_pRateInfo->m_userId;
       t_sumBill1.m_value = ((m_pcdr[ATTRI_SP_TOTALDATA].value() + 1023) / 1024) * 1024;
       if (m_pcdr[ATTRI_FEE_FIELD1].value() > 0)
       {
           t_sumBill1.m_billId = 99998;
       }
       else
       {
           t_sumBill1.m_billId = 99999;
       }
       m_pRateInfo->updateBillIncre(m_pRateInfo->m_userSumBill1s_new,t_sumBill1);
   }
   
   if( m_pcdr[ATTRI_BEGIN_DATE].value()/100!=info.m_billCycle)
   {
   #ifdef _DEBUG_
       cout<<"GPRS话单帐期不对，不累计日保底"
           <<__FILE__<<__LINE__<<endl;
     #endif
     return 0;
   }

    if(m_pRateInfo->getBillItem(m_pRateInfo->m_userBills_new,t_userbill,t_pUserBill)==true)
    {
      t_userbill.m_value       = t_pUserBill->m_value;
    }
    //t_userbill.m_value      |= 2^(t_startDay-1);
    t_userbill.m_value      |= 1<<(t_startDay-1);
    t_userbill.m_discntValue = t_userbill.m_value;
    m_pRateInfo->updateBillTotal(m_pRateInfo->m_userBills_new,t_userbill);
  }
  return 0;
}



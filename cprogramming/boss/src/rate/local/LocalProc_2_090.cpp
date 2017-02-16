#include "LocalProc_2_090.h"

extern RateGlobalInfo g_globalInfo;

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
	// by chenm for 吉林:没有什么3秒免费的说法,全部正常收费 2009-5-4 19:00:34
	return 1;
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
    //update by  xiel at 2009/06/26  
    //L193 52业务类型有原始费用
    if( m_pcdr[ATTRI_SOURCE_TYPE] == "52" )
    {
 	    m_pRateInfo->setOrgFeeFlag(ATTRI_DISCFEE_FIELD1,1);
        m_pcdr[ATTRI_FEEUNIT_1] = DEFAULT_CFEE_UNIT;
    }
 
  }else if(m_pcdr[ATTRI_BIZ_TYPE] == BIZ_TYPE_UNICOMSP)
  { //联通SP增值业务 
    //省内彩信平台48需要一次批价
    if( m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_SP_MMS )
    { //无原始费用需要一次批价
    }
    else if( m_pcdr[ATTRI_SOURCE_TYPE] == "4D" )
    {
    }
    else
    {
      m_pRateInfo->setOrgFeeFlag(ATTRI_DISCFEE_FIELD1,1);
      m_pRateInfo->setOrgFeeFlag(ATTRI_DISCFEE_FIELD2,1);
    }

    if(m_pcdr[ATTRI_SP_RATEUNIT] == SP_RATE_UNIT_TIME ||
       m_pcdr[ATTRI_SP_RATEUNIT] == SP_RATE_UNIT_CLTIME)
    { //时长
      //时长为0 置缺省1   add by xiel   at 2009/05/20
      if (m_pcdr[ATTRI_DURATION]==0)
      	m_pcdr[ATTRI_DURATION]=1;
      m_pcdr[ATTRI_FEEUNIT_1] = DEFAULT_CFEE_UNIT;
      m_pcdr[ATTRI_FEEUNIT_2] = DEFAULT_CFEE_UNIT;
    }else if(m_pcdr[ATTRI_SP_RATEUNIT] == SP_RATE_UNIT_QRT ||
             m_pcdr[ATTRI_SP_RATEUNIT] == SP_RATE_UNIT_CLQRT)
    { //按次数,采用变相按时长方式实现
      m_pcdr[ATTRI_DURATION] = m_pcdr[ATTRI_SP_QTY];
      //次数为0 置缺省1   add by xiel   at 2009/05/20
      if (m_pcdr[ATTRI_DURATION]==0)
      	m_pcdr[ATTRI_DURATION]=1;
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

    //重置计费时长
    if( m_pcdr[ATTRI_SOURCE_TYPE] == "4D" )
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
  { //固网语音类
    //主要确定哪些业务是有原始费用的 coding here later
  }else if(m_pcdr[ATTRI_BIZ_TYPE] == BIZ_TYPE_FIXSMS)
  { //固网短信类
    
  }else if(m_pcdr[ATTRI_BIZ_TYPE] == BIZ_TYPE_NET)
  { //按流量计费
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
  if( m_pcdr[ATTRI_BIZ_TYPE] == BIZ_TYPE_NET &&
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

    static RateRunInfo info={0,0,"",0,0,0,""};
   if ( info.m_billCycle == 0 ) 
   {
     info.m_channelNo=g_globalInfo.m_pConfig->m_channelNo;
     g_globalInfo.m_billMgr->getRateRunInfo(info);
   }
   
   #ifdef _DEBUG_
     cout<<"批价通道帐期："<<info.m_billCycle<<endl;
     cout<<"宽带话单帐期："<<m_pcdr[ATTRI_BEGIN_DATE].substr(0,6)<<endl;
   #endif
   
   /*
   if( m_pcdr[ATTRI_BEGIN_DATE].value()/100!=info.m_billCycle)
   {
     #ifdef _DEBUG_
       cout<<"宽带话单帐期不对，不累计日保底"
           <<__FILE__<<__LINE__<<endl;
     #endif
     return 0;
   }
   */

    if(m_pRateInfo->getBillItem(m_pRateInfo->m_userBills_new,t_userbill,t_pUserBill)==true)
    {
      t_userbill.m_value       = t_pUserBill->m_value;
    }
    //t_userbill.m_value      |= 2^(t_startDay-1);
    t_userbill.m_value      |= 1<<(t_startDay-1);
    t_userbill.m_discntValue = t_userbill.m_value;
    m_pRateInfo->updateBillTotal(m_pRateInfo->m_userBills_new,t_userbill);
  }
  
  
  //累计宽带时长，按账期进行累计
  if( m_pcdr[ATTRI_BIZ_TYPE] == BIZ_TYPE_NET &&
     m_pRateInfo->m_userId.value()>0 ) 
  { 
    const int BILL_ID_NET_FLAG = 21000; //这个帐目项对应的账单值
    UserBill  t_userbill;
    const UserBill *t_pUserBill= NULL;
    t_userbill.m_userId       = m_pRateInfo->m_userId;
    t_userbill.m_billId       = BILL_ID_NET_FLAG;
    t_userbill.m_value        = 0;
    t_userbill.m_discntValue  = 0;
    
    
   static RateRunInfo info={0,0,"",0,0,0,""};
   if ( info.m_billCycle == 0 ) 
   {
     info.m_channelNo=g_globalInfo.m_pConfig->m_channelNo;
     g_globalInfo.m_billMgr->getRateRunInfo(info);
   }
   
   #ifdef _DEBUG_
     cout<<"批价通道帐期："<<info.m_billCycle<<endl;
     cout<<"宽带话单帐期："<<m_pcdr[ATTRI_BEGIN_DATE].substr(0,6)<<endl;
   #endif
   
    if(m_pRateInfo->getBillItem(m_pRateInfo->m_userBills_new,t_userbill,t_pUserBill)==true)
    {
      t_userbill.m_value       = t_pUserBill->m_value;
    }
    
    t_userbill.m_value     =t_userbill.m_value + (m_pcdr[ATTRI_DURATION].value()+59)/60*60;
   #ifdef _DEBUG_
     cout<<"t_userbill.m_value："<<t_userbill.m_value<<endl;
     cout<<"m_pcdr[ATTRI_DURATION].value()："<<m_pcdr[ATTRI_DURATION].value()<<endl;
   #endif
   
    t_userbill.m_discntValue = t_userbill.m_value;
    m_pRateInfo->updateBillTotal(m_pRateInfo->m_userBills_new,t_userbill);
  }
  
  /*
   if( m_pcdr[ATTRI_BIZ_TYPE] == BIZ_TYPE_FISC &&(m_pcdr[ATTRI_SP_CODE]=="1613" ||m_pcdr[ATTRI_SP_CODE]=="1801") &&
     m_pRateInfo->m_userId.value()>0 ) 
  { 
  	const int BILL_ID_NET1_FLAG = 81283; //这个帐目项对应的账单值
  	const int BILL_ID_NET2_FLAG = 81041; //这个帐目项对应的账单值

    UserBill  t_userbill_f;
    UserBill  t_userbill_n;
    
    const UserBill *t_pUserBill_f= NULL;
    const UserBill *t_pUserBill_n= NULL;
    
    t_userbill_f.m_userId       = m_pRateInfo->m_userId;
    t_userbill_f.m_billId       = BILL_ID_NET1_FLAG;
    t_userbill_f.m_value        = 0;
    t_userbill_f.m_discntValue  = 0;
    
    t_userbill_n.m_userId       = m_pRateInfo->m_userId;
    t_userbill_n.m_billId       = BILL_ID_NET2_FLAG;
    t_userbill_n.m_value        = 0;
    t_userbill_n.m_discntValue  = 0;
    
   static RateRunInfo info={0,0,"",0,0,0,""};
   if ( info.m_billCycle == 0 ) 
   {
     info.m_channelNo=g_globalInfo.m_pConfig->m_channelNo;
     g_globalInfo.m_billMgr->getRateRunInfo(info);
   }
   
   #ifdef _DEBUG_
     cout<<"批价通道帐期："<<info.m_billCycle<<endl;
     cout<<"宽带话单帐期："<<m_pcdr[ATTRI_BEGIN_DATE].substr(0,6)<<endl;
   #endif
   
   if(m_pcdr[ATTRI_SP_CODE]=="1613")
   {
    if(m_pRateInfo->getBillItem(m_pRateInfo->m_userBills_new,t_userbill_f,t_pUserBill_f)==true)
    {
      t_userbill_f.m_value       = t_pUserBill_f->m_value;
    }
#ifdef _DEBUG_
     cout<<"1-t_userbill_f.m_value："<<t_userbill_f.m_value<<endl;
#endif      
    t_userbill_f.m_value     =t_userbill_f.m_value + 1;
    t_userbill_f.m_discntValue = t_userbill_f.m_value;
    m_pRateInfo->updateBillTotal(m_pRateInfo->m_userBills_new,t_userbill_f);
#ifdef _DEBUG_
     cout<<"2-t_userbill_f.m_value："<<t_userbill_f.m_value<<endl;
#endif    
   }
   else
   {
    if(m_pRateInfo->getBillItem(m_pRateInfo->m_userBills_new,t_userbill_n,t_pUserBill_n)==true)
    {
      t_userbill_n.m_value       = t_pUserBill_n->m_value;
    }
    
    t_userbill_n.m_value     =t_userbill_n.m_value + 1;
    t_userbill_n.m_discntValue = t_userbill_n.m_value;
    m_pRateInfo->updateBillTotal(m_pRateInfo->m_userBills_new,t_userbill_n);
#ifdef _DEBUG_
     cout<<"t_userbill_n.m_value："<<t_userbill_n.m_value<<endl;
#endif    	
   }
  }
  */
  
  return 0;
}

#include "LocalizeAttribProc_019.h"

#include "FilterConfig.h"
#include "PMGlobalManager.h"

//////////////////////////////////////////////////////////////////////////
INSTALL_PROCESSBASE_INSTANCE(LocalizeAttribProc019);

int LocalizeAttribProc019::execute()
{
	return 0;
}

//////////////////////////////////////////////////////////////////////////
INSTALL_PROCESSBASE_INSTANCE(LocalizeAttribSet);

int LocalizeAttribSet::execute()
{
	//江苏联通语音 IP特殊处理 将智能网类别06转换成01类型
	if ((m_pcdr[ATTRI_BIZ_TYPE] == BIZ_TYPE_GSM || m_pcdr[ATTRI_BIZ_TYPE] == BIZ_TYPE_UNICOMIP) &&
		   m_pcdr[ATTRI_USER_SERVICE_TYPE_B] == SERVICE_TYPE_SSP)
	{
		m_pcdr[ATTRI_USER_SERVICE_TYPE_B] = SERVICE_TYPE_GSM;
	}
	if(m_pcdr[ATTRI_SUM_SERVTYPE].empty())
	{
		m_pcdr[ATTRI_SUM_SERVTYPE] = m_pcdr[ATTRI_USER_SERVICE_TYPE_B];
	}
	
	if (m_pcdr[ATTRI_BIZ_TYPE] == BIZ_TYPE_GSM && m_pcdr[ATTRI_USER_SERVICE_TYPE_A] == "03")
	{
		m_pcdr[ATTRI_DOUBLEMODE] = DOUBLEMODE_CTOG;
	}
	/*************** add by chenzm@lianchuang.com ******************
	 IVPN话单的部分属性 一次分拣和老系统求取出的不一致，尤其是长途类型。
	 需要从预处理传入，保留在预留字段，然后在本地化代码中覆盖相应字段，
	 启用<input_gsm>中62 ~ 66预留字段
	 对应关系如下：
	 预处理输入字段        覆盖分拣对应字段
	 ATTRI_GPRS_DATA_UP1   ATTRI_LONG_TYPE1     //长途类型
	 ATTRI_GPRS_DATA_UP2   ATTRI_HOME_CODE_A    //A归属
	 ATTRI_GPRS_DATA_DOWN1 ATTRI_VISIT_CODE_A   //A到访
	 ATTRI_GPRS_DATA_DOWN2 ATTRI_HOME_CODE_B    //B归属
	 ATTRI_GPRS_APNNI      ATTRI_VISIT_CODE_B   //B到访	
	****************  add end 2009-5-17 ****************************/
	if (m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_GSM_IVPN)
	{
		if (!m_pcdr[ATTRI_GPRS_DATA_UP2].empty() && m_pcdr[ATTRI_GPRS_DATA_UP2].length()>=3) 
		{
			if (m_pcdr[ATTRI_GPRS_DATA_UP2].in("0349","0350","0351","0352","0353","0354","0355","0356","0357","0358","0359"))
			{
				m_pcdr[ATTRI_HOME_CODE_A]=m_pcdr[ATTRI_GPRS_DATA_UP2].c_str();
			}
			else
			{
	             m_pcdr[ATTRI_ERROR_CODE]=BH_MSISDNP_ERROR;
	             m_pcdr[ATTRI_CDR_CHANNEL]=CHANNEL_ERROR;
	             return 1;				
			}
		}
		if (!m_pcdr[ATTRI_GPRS_DATA_DOWN1].empty() && m_pcdr[ATTRI_GPRS_DATA_UP2].length()>=3) 
			m_pcdr[ATTRI_VISIT_CODE_A]=m_pcdr[ATTRI_GPRS_DATA_DOWN1].c_str();
		
		if (!m_pcdr[ATTRI_GPRS_DATA_DOWN2].empty() && m_pcdr[ATTRI_GPRS_DATA_UP2].length()>=3) 
			m_pcdr[ATTRI_HOME_CODE_B]=m_pcdr[ATTRI_GPRS_DATA_DOWN2].c_str();
		
		if (!m_pcdr[ATTRI_GPRS_APNNI].empty() && m_pcdr[ATTRI_GPRS_DATA_UP2].length()>=3) 
			m_pcdr[ATTRI_VISIT_CODE_B]=m_pcdr[ATTRI_GPRS_APNNI].c_str();
		//边漫特殊处理
		if(m_pcdr[ATTRI_EDGE_FLAG] == EDGE_ROAM_PORVINNER)
		{
			m_pcdr[ATTRI_VISIT_CODE_A] = m_pcdr[ATTRI_HOME_CODE_A];
		}		
		if (m_pcdr[ATTRI_HOME_CODE_A] == m_pcdr[ATTRI_VISIT_CODE_A])
			m_pcdr[ATTRI_ROAM_TYPE] = ROAM_TYPE_NONE; //0
		else if (m_pcdr[ATTRI_VISIT_CODE_A].in("0349","0350","0351","0352","0353","0354","0355","0356","0357","0358","0359"))
			m_pcdr[ATTRI_ROAM_TYPE] = ROAM_TYPE_PRO_IN; //1
		else if (strncmp(m_pcdr[ATTRI_VISIT_CODE_A].c_str(),"00",2) == 0)
			m_pcdr[ATTRI_ROAM_TYPE] = ROAM_TYPE_INTERNATIONAL_OUT; //6
		else
			m_pcdr[ATTRI_ROAM_TYPE] = ROAM_TYPE_NATIONAL_OUT; //4
		
		if ( m_pcdr[ATTRI_GPRS_DATA_UP1] == "1")//预处理求出长途
		{
			if (m_pcdr[ATTRI_LONG_TYPE1] == "0")//分拣求取出非长途
			{
				//m_pcdr[ATTRI_LONG_TYPE1] = "1";//设置分拣长途属性为省内长途
				setLongType();//再次求取
				if (m_pcdr[ATTRI_LONG_TYPE1] == "0")
					m_pcdr[ATTRI_LONG_TYPE1] = "1";
				
			}
		}
		//边漫特殊处理
		if(m_pcdr[ATTRI_EDGE_FLAG] == EDGE_ROAM_PORVINNER)
		{
			if (m_pcdr[ATTRI_VISIT_CODE_A] == m_pcdr[ATTRI_HOME_CODE_B])
				m_pcdr[ATTRI_LONG_TYPE1] = "0";
		}		
	}
	if(m_pcdr[ATTRI_EDGE_FLAG] == EDGE_ROAM_PORVINNER)
	{
		m_pcdr[ATTRI_ROAM_TYPE] = ROAM_TYPE_NONE;
	}
    
    //add by chenzm@20090529
	//按老系统规则：无条件呼转和不可及呼转，不论是否漫游，都应视为非漫游,
	//否则呼转拆出来的被叫话单CALL_TYPE=20的话单会计费,需要修改本地化程序
	//涉及source_type=11 12 15的	
	if ((m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_GSM_MSC //11 GSM端局，关口局话单
		&& m_pcdr[ATTRI_HOME_CODE_A].in("0351","0349","0350","0352","0353","0354","0355","0356","0357","0358","0359"))//本省用户
    ||  m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_GSM_CRO   //12 GSM总部下发的国内漫游话单
  //||  m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_GSM_IVPN  //15 GSM语音IVPN话单(逻辑复杂，上面已经求解)
	) 
	{
		if (m_pcdr[ATTRI_FORWARD_CAUSE] == FORW_UNCONDITIONAL //1 无条件呼转
		||  m_pcdr[ATTRI_FORWARD_CAUSE] == FORW_UNREACHABLE)  //4 不可及呼转
		{
			if (m_pcdr[ATTRI_CALL_TYPE] == CALL_TYPE_FORW    //03 GSM呼转
			||  m_pcdr[ATTRI_CALL_TYPE] == CALL_TYPE_FORWMTC)//20 呼转被叫
			{
				if (m_pcdr[ATTRI_VISIT_CODE_A] != m_pcdr[ATTRI_HOME_CODE_A])
				{
					m_pcdr[ATTRI_VISIT_CODE_A] = m_pcdr[ATTRI_HOME_CODE_A];//到访地置为归属地
					m_pcdr[ATTRI_ROAM_TYPE]    = ROAM_TYPE_NONE;           //漫游类型设置为非漫游
					setLongType();//再次求取长途类型
					if (m_pcdr[ATTRI_VISIT_CODE_A] == m_pcdr[ATTRI_HOME_CODE_B])
						m_pcdr[ATTRI_LONG_TYPE1] = "0";
				}
			}
		}
	}
	//add end
	if (m_pcdr[ATTRI_SOURCE_TYPE] == "18" 
	&&  m_pcdr[ATTRI_LONG_TYPE1] == LONG_TYPE_NONE
	&& !m_pcdr[ATTRI_ORGFEE_FIELD3].empty() //ORG_LFEE
	&&  m_pcdr[ATTRI_ORGFEE_FIELD3].value() > 0
	)
	{
		//0 LONG_TYPE_NONE,1 LONG_TYPE_PROV,2 LONG_TYPE_NATIONAL,3 LONG_TYPE_INTERNATIONAL
		m_pcdr[ATTRI_LONG_TYPE1] = LONG_TYPE_PROV;
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////
INSTALL_PROCESSBASE_INSTANCE(CdrAttribInit);

int CdrAttribInit::execute()
{
	return 0;
}

//////////////////////////////////////////////////////////////////////////
INSTALL_PROCESSBASE_INSTANCE(BeforeOtherPartyStand);
//对端号码提前规整
int BeforeOtherPartyStand::execute()
{
   int rd;
   T_ATTRI_VAL r_homecode;
   T_ATTRI_VAL r_longgroup;
   T_ATTRI_VAL r_usertype;
   T_ATTRI_VAL r_msisdn_b;
   int transNumbLen = 0;
   int msisdnALen = 0;
   T_ATTRI_VAL tmpTranType;
   T_ATTRI_VAL tmpCarrierType;
   T_ATTRI_VAL tmpUserAspB;
   T_ATTRI_VAL tmpDialType;
   T_ATTRI_VAL tmpUserType;
   T_ATTRI_VAL tmpIsnoneedTp;
   T_ATTRI_VAL called_type;
   T_ATTRI_VAL groupId;
   T_ATTRI_VAL sum_servtype;
   int length;
   int error_code;

   //去除区号
   if (m_pcdr[ATTRI_MSISDN_B].length() <5)
   {
	    return 2;
   }
   //山西有主叫14位，002开头的号码，无法求取归属及运营商，做特殊处理，
   //add by chenzm@2009-5-17
   if (m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_GSM_MSC
   &&  m_pcdr[ATTRI_CALL_TYPE] == CALL_TYPE_MTC
   &&  m_pcdr[ATTRI_MSISDN_B].length() == 14 
   &&  strncmp(m_pcdr[ATTRI_MSISDN_B].c_str(), "002", 3) == 0 
   &&  isMobile(m_pcdr[ATTRI_MSISDN_B].c_str() + 3))
   {
   	   m_pcdr[ATTRI_MSISDN_B] = m_pcdr[ATTRI_MSISDN_B].c_str() + 3;
   	   m_pcdr[ATTRI_MSISDN_B_FOR_ROAM] = m_pcdr[ATTRI_MSISDN_B];
   	   return 2;
   }
   //add end

   //modify by shenglj 2009-09-25 14:37:00
   if (m_pcdr[ATTRI_SOURCE_TYPE] == "18") //商务总机去掉101313后求对端归属
   {
      if(strncmp(m_pcdr[ATTRI_MSISDN_B].c_str(),"101313",6) == 0)
      {
  		   m_pcdr[ATTRI_MSISDN_B_ORG] = m_pcdr[ATTRI_MSISDN_B_ORG].c_str()+6;
         return 2;
      }
   }

	//"00"+手机号码开始，则截去00 2008-12-26 15:20:01
	if ((strncmp(m_pcdr[ATTRI_MSISDN_B].c_str(), "00", 2) == 0) && (isMobile(m_pcdr[ATTRI_MSISDN_B].c_str() + 2)))
	{
		m_pcdr[ATTRI_MSISDN_B] = m_pcdr[ATTRI_MSISDN_B].c_str() + 2;
		m_pcdr[ATTRI_MSISDN_B_FOR_ROAM] = m_pcdr[ATTRI_MSISDN_B];
	}

   //手机号码截取前11位
   if(isMobile(m_pcdr[ATTRI_MSISDN_B]) && m_pcdr[ATTRI_MSISDN_B].length()>11)
   {
   	 m_pcdr[ATTRI_MSISDN_B] = m_pcdr[ATTRI_MSISDN_B].substr(0,11);
   	 m_pcdr[ATTRI_MSISDN_B_FOR_ROAM] = m_pcdr[ATTRI_MSISDN_B];
   	 return 2;
   }

	//固话区号补0
   r_msisdn_b = m_pcdr[ATTRI_MSISDN_B];

   if(m_globalManager->getSpecialPhone(r_msisdn_b,m_pcdr[ATTRI_HOME_CODE_A],m_pcdr[ATTRI_HOME_CODE_B],m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],
                          m_pcdr,tmpUserType,tmpIsnoneedTp,called_type,groupId,sum_servtype,length,error_code))
   {
      //特殊号码标识
      m_pcdr[ATTRI_SPE_PHONE_FLAG] = ATTRI_SPECIALPHONE_Y;
   }
   if(r_msisdn_b.length()>=10 && !isMobile(r_msisdn_b) && 
   	  !m_globalManager->getTransNumber(r_msisdn_b,transNumbLen,tmpCarrierType,tmpTranType,tmpUserAspB,tmpDialType) &&
      m_pcdr[ATTRI_SPE_PHONE_FLAG] == ATTRI_SPECIALPHONE_N)
   {
   	  //漫游出访话单不在此补0处理
   	  if(m_pcdr[ATTRI_SOURCE_TYPE].in(SOURCE_TYPE_GSM_CRO,SOURCE_TYPE_GSM_CIO,SOURCE_TYPE_GSM_GBO))
   	  	return 2;
   	  r_msisdn_b = r_msisdn_b.append(0,"0");
      rd = m_globalManager->getHomeAreaCodeByMsisdnN(r_msisdn_b,m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],
    	   	                                           r_homecode,r_longgroup,r_usertype);
      //if(rd >0 && m_globalManager->isProvInnerCode(r_homecode))
      if(rd >0)
      {
      	//主叫 对端号码1开头不规整 add by shenglj 2009-03-01 23:58:00
      	if(m_pcdr[ATTRI_CALL_TYPE] == CALL_TYPE_MOC && strncmp(m_pcdr[ATTRI_MSISDN_B].c_str(), "1", 1) == 0)
      	{
   	       return 2;
      	}
     		m_pcdr[ATTRI_MSISDN_B] = r_msisdn_b;
     		m_pcdr[ATTRI_MSISDN_B_FOR_ROAM] = m_pcdr[ATTRI_MSISDN_B];
   	    return 2;
      }
   }

	return 0;
}

///////////////////////////////////////////////////////////////////////////
INSTALL_PROCESSBASE_INSTANCE(AfterSetMocLongType);

int AfterSetMocLongType::execute()
{
	return 0;
}

///////////////////////////////////////////////////////////////////////////
INSTALL_PROCESSBASE_INSTANCE(AfterForwLongType);

int AfterForwLongType::execute()
{
	return 0;
}

///////////////////////////////////////////////////////////////////////////
INSTALL_PROCESSBASE_INSTANCE(BeforeGetVisitAreaCodeA);

int BeforeGetVisitAreaCodeA::execute()
{
	//add by chenzm@2009-4-15
	//山西智能网下的IVPN话单不能放到错单中
	if(m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_GSM_IVPN)
	{
		//如果到访地已经求出，直接返回
		if (m_pcdr[ATTRI_VISIT_CODE_A].length() > 1 && m_pcdr[ATTRI_VISIT_CODE_A][0] == '0')
			return 1;
	}
	//add end
		
  //PPS语音话单A到访，如果非区号，则重新求取
  if(m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_GSM_PPS)
  {
    T_ATTRI_VAL homeType;
    T_ATTRI_VAL long_group;
  	if(m_globalManager->getGroupIdByAreaCodeN(m_pcdr[ATTRI_VISIT_CODE_A],
           m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],
           homeType,long_group))
    {
    	 m_pcdr[ATTRI_ERROR_CODE]  = DEFAULT_ERROR;
       return 1;
    }
  }

   //IVPN 军网话单中有漫游话单，交换机号是全网 端局求取方式不适用
  if(m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_GSM_IVPN    ||
  	 m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_GSM_GZHEJ_S ||
  	 m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_GSM_GZHEJ_L)
  {
		 T_ATTRI_VAL sTmp1;
		 T_ATTRI_VAL sTmp2;
		 int rd;
     rd = m_globalManager->getHomeAreaCodeByM123(m_pcdr[ATTRI_MSC],m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],
                m_pcdr[ATTRI_VISIT_CODE_A],sTmp1,sTmp2);
     return 1;
   }

	return 0;
}

///////////////////////////////////////////////////////////////////////////
INSTALL_PROCESSBASE_INSTANCE(AfterGetVisitAreaCodeA);

int AfterGetVisitAreaCodeA::execute()
{
  if (m_pcdr[ATTRI_SOURCE_TYPE] == "18" 
  && m_pcdr[ATTRI_MSC].empty()
  && !m_pcdr[ATTRI_VISIT_CODE_A].empty()
  && !m_pcdr[ATTRI_HOME_CODE_A].empty() )
  {
  		m_pcdr[ATTRI_ERROR_CODE] = DEFAULT_ERROR;
  		return 1;
  }	
  //IVPN、江浙沪军网话单A到访求取失败，如果非区号，则重新求取
  if(m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_GSM_IVPN    ||
  	 m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_GSM_GZHEJ_S ||
  	 m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_GSM_GZHEJ_L)
  {
    T_ATTRI_VAL homeType;
    T_ATTRI_VAL long_group;
  	if(m_globalManager->getGroupIdByAreaCodeN(m_pcdr[ATTRI_VISIT_CODE_A],
           m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],
           homeType,long_group))
    {
    	 m_pcdr[ATTRI_ERROR_CODE]  = DEFAULT_ERROR;
       return 1;
    }
  }
	return 0;
}

///////////////////////////////////////////////////////////////////////////
INSTALL_PROCESSBASE_INSTANCE(BeforeGetVisitAreaCodeB);

int BeforeGetVisitAreaCodeB::execute()
{
	return 0;
}

///////////////////////////////////////////////////////////////////////////
INSTALL_PROCESSBASE_INSTANCE(AfterGetVisitAreaCodeB);

int AfterGetVisitAreaCodeB::execute()
{
	return 0;
}

///////////////////////////////////////////////////////////////////////////
INSTALL_PROCESSBASE_INSTANCE(AfterSetRoamType);

int AfterSetRoamType::execute()
{
	return 0;
}

///////////////////////////////////////////////////////////////////////////
INSTALL_PROCESSBASE_INSTANCE(SetOriginFeeFlag);

int SetOriginFeeFlag::execute()
{
  return 0;
}

///////////////////////////////////////////////////////////////////////////
INSTALL_PROCESSBASE_INSTANCE(CdrAttribEnd);

int CdrAttribEnd::execute()
{
	return 0;
}

///////////////////////////////////////////////////////////////////////////
INSTALL_PROCESSBASE_INSTANCE(LocalizeErrorCdrProc);

int LocalizeErrorCdrProc::execute()
{	
	return 0;
}

///////////////////////////////////////////////////////////////////////////
INSTALL_PROCESSBASE_INSTANCE(IsNoNeedTPSpecialProc);

int IsNoNeedTPSpecialProc::execute()
{
	return 0;
}

///////////////////////////////////////////////////////////////////////////
INSTALL_PROCESSBASE_INSTANCE(LocalEdgeRoamProc);

int LocalEdgeRoamProc::execute()
{
	//省内边漫 拨打AV=BH算漫游，AV!=BH 算本地 by shenglj 2009-01-13 21:51:01
  //if(m_pcdr[ATTRI_EDGE_FLAG] == EDGE_ROAM_PORVINNER || m_pcdr[ATTRI_EDGE_FLAG] == EDGE_ROAM_INTERPROV)
  if(m_pcdr[ATTRI_EDGE_FLAG] == EDGE_ROAM_PORVINNER)
	{
		m_pcdr[ATTRI_VISIT_CODE_A] = m_pcdr[ATTRI_HOME_CODE_A];
	    //if ((m_pcdr[ATTRI_CALL_TYPE] == CALL_TYPE_MOC || m_pcdr[ATTRI_CALL_TYPE] == CALL_TYPE_FORW) &&
	    //    isSameArea(m_pcdr[ATTRI_VISIT_CODE_A_ORG],m_pcdr[ATTRI_HOME_CODE_B]))
	    //{
	    //	//不重置
	    //}
	    //else
	    //{
		//	m_pcdr[ATTRI_VISIT_CODE_A] = m_pcdr[ATTRI_HOME_CODE_A];
	    //}
   
	    ////呼转暂时先不考虑，如果需要再打开
	    //if ((m_pcdr[ATTRI_CALL_TYPE] == CALL_TYPE_MOC /*|| m_pcdr[ATTRI_CALL_TYPE] == CALL_TYPE_FORW*/) 
	    //&& isSameArea(m_pcdr[ATTRI_VISIT_CODE_A_ORG],m_pcdr[ATTRI_HOME_CODE_B]))
	    //{
	    //	m_pcdr[ATTRI_VISIT_CODE_A] = m_pcdr[ATTRI_HOME_CODE_B];
	    //}
	    //else
	    //{
		//	m_pcdr[ATTRI_VISIT_CODE_A] = m_pcdr[ATTRI_HOME_CODE_A];
	    //}
	}
	//省际边漫算本地
	else if(m_pcdr[ATTRI_EDGE_FLAG] == EDGE_ROAM_INTERPROV)
	{
     m_pcdr[ATTRI_VISIT_CODE_A] = m_pcdr[ATTRI_HOME_CODE_A];
     //m_pcdr[ATTRI_CELLID_A]     = "99999";//影响小区优惠
	}
	//集团下发CRO文件边漫算本地
	else if(m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_GSM_CRO && m_pcdr[ATTRI_EDGE_FLAG] == EDGE_ROAM_LOCAL_INTERPROV)
	{
     m_pcdr[ATTRI_VISIT_CODE_A] = m_pcdr[ATTRI_HOME_CODE_A];
     //m_pcdr[ATTRI_CELLID_A]     = "99999";//影响小区优惠
	}
	return 0;
}

///////////////////////////////////////////////////////////////////////////
INSTALL_PROCESSBASE_INSTANCE(LocalizeIPCdrSetProc);

int LocalizeIPCdrSetProc::execute()
{
  T_ATTRI_VAL m_trunktype;
  T_ATTRI_VAL m_busitype;
  T_ATTRI_VAL m_usertype;
  T_ATTRI_VAL m_filterflag;
  T_ATTRI_VAL m_areacode;
	int t_iRet = 0;

  m_PO_TrunkUserContainer = (ParamContainer<PO_TrunkUser>*) PMHelperall::GetParamContainer("PO_TrunkUser");

  //判断话单种类
  otherpartyStanding();

  //4.根据交换机类别 出中继先过滤
	if(m_globalManager->getTypeByTrunk(m_pcdr[ATTRI_MSC],
		     m_pcdr[ATTRI_TRUNKGROUPOUT],
		     m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],"1",
		     m_trunktype,m_busitype,m_usertype,m_filterflag))
	{
		if(m_filterflag == "D") //RESEVER保留 DEL删除
		{
			m_pcdr[ATTRI_TRASH_FLAG]=ATTRI_TRASH_Y;
      return true;
		}
	}

   m_pcdr[ATTRI_BUSI_TYPE] = "0"; 
	//5. 根据交换机类别 入中继匹配
	m_pcdr[ATTRI_IP_SERV_TYPE_A] = SERVICE_TYPE_17911_TRUNK; //默认是17911专线话单
	if(m_globalManager->getTypeByTrunk(m_pcdr[ATTRI_MSC],
		     m_pcdr[ATTRI_TRUNKGROUPIN],
		     m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],"0",
		     m_trunktype,m_busitype,m_usertype,m_filterflag))
	{
		if(m_filterflag == "D") //RESEVER保留 DEL删除
		{
			m_pcdr[ATTRI_TRASH_FLAG]=ATTRI_TRASH_Y;
      return true;
		}
    m_pcdr[ATTRI_BUSI_TYPE] = m_busitype; //后续属性求取部分需带出来确定计费方
	}

  //专网用户表
	t_iRet = getTypeByTrunkUser(m_pcdr[ATTRI_MSISDN_A],
	        m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],
	        m_usertype,m_areacode);

	 if(m_pcdr[ATTRI_BUSI_TYPE] == "1")
	 {
	 	 if(m_pcdr[ATTRI_IPACCESS_TYPE] == "2")
	 	 	 m_pcdr[ATTRI_IP_SERV_TYPE_A] = SERVICE_TYPE_193_TRUNK;
	 	 else
	 	 	 m_pcdr[ATTRI_IP_SERV_TYPE_A] = SERVICE_TYPE_17911_TRUNK;
	 }
	 else if(t_iRet>0)
	 {
   	  if(m_usertype=="1")
   	  {
//cout<<"m_usertype:  "<<m_usertype<<__FILE__<<__LINE__<<endl;
   	  	 if(m_pcdr[ATTRI_IPACCESS_TYPE] == "2")
   	  	    m_pcdr[ATTRI_IP_SERV_TYPE_A] = SERVICE_TYPE_193_TRUNK;
   	  	 else if(m_pcdr[ATTRI_IPACCESS_TYPE] == "4")
   	  	    m_pcdr[ATTRI_IP_SERV_TYPE_A] = SERVICE_TYPE_17911_TRUNK;
   	  	 else
   	  	 	  m_pcdr[ATTRI_IP_SERV_TYPE_A] = SERVICE_TYPE_193_TRUNK;
   	  }
   	  else if(m_usertype=="3")
   	  {
//cout<<"m_usertype:  "<<m_usertype<<__FILE__<<__LINE__<<endl;
   	  	 if(m_pcdr[ATTRI_IPACCESS_TYPE] == "2")
   	  	    m_pcdr[ATTRI_IP_SERV_TYPE_A] = SERVICE_TYPE_193_TRUNK;
   	  	 else if(m_pcdr[ATTRI_IPACCESS_TYPE] == "4")
   	  	    m_pcdr[ATTRI_IP_SERV_TYPE_A] = SERVICE_TYPE_17911_TRUNK;
   	  	 else
   	  	 	  m_pcdr[ATTRI_IP_SERV_TYPE_A] = SERVICE_TYPE_17911_TRUNK;
   	  }
   	  else
   	  {
//cout<<"m_usertype:  "<<m_usertype<<__FILE__<<__LINE__<<endl;
   	  	 if(m_pcdr[ATTRI_IPACCESS_TYPE] == "2")
   	  	    m_pcdr[ATTRI_IP_SERV_TYPE_A] = SERVICE_TYPE_193_SCATTER;
   	  	 else if(m_pcdr[ATTRI_IPACCESS_TYPE] == "3")
   	  	    m_pcdr[ATTRI_IP_SERV_TYPE_A] = SERVICE_TYPE_17911_TRUNK;
   	  	 else if(m_pcdr[ATTRI_IPACCESS_TYPE] == "4")
   	  	 	  m_pcdr[ATTRI_IP_SERV_TYPE_A] = SERVICE_TYPE_17911_SCATTER;
   	  }
	 }
	 else
	 {
     	if((m_pcdr[ATTRI_MSISDN_A].length()>3) &&
     	    (!isMobile(m_pcdr[ATTRI_MSISDN_A])) &&
     	 (!isMobile(m_pcdr[ATTRI_MSISDN_A].c_str()+1)))
     	 {
//cout<<"m_usertype:  "<<m_usertype<<__FILE__<<__LINE__<<endl;
     	  	 if(m_pcdr[ATTRI_IPACCESS_TYPE] == "2")
     	  	    m_pcdr[ATTRI_IP_SERV_TYPE_A] = SERVICE_TYPE_193_SCATTER;
     	  	 else if(m_pcdr[ATTRI_IPACCESS_TYPE] == "3")
     	  	    m_pcdr[ATTRI_IP_SERV_TYPE_A] = SERVICE_TYPE_17911_TRUNK;
     	  	 else if(m_pcdr[ATTRI_IPACCESS_TYPE] == "4")
     	  	 	  m_pcdr[ATTRI_IP_SERV_TYPE_A] = SERVICE_TYPE_17911_SCATTER;
     	 }
	 }

	return 0;
}

bool LocalizeIPCdrSetProc::otherpartyStanding()
{
  //被叫用户号码为'193,111193,068' 类别为2
  //被叫用户号码为'17911,17912,17991,172,161'打头 类别为4
  //其他为类别3
  if(strncmp(m_pcdr[ATTRI_MSISDN_B_ORG].c_str(), "193", 3) == 0 ||
  	 strncmp(m_pcdr[ATTRI_MSISDN_B_ORG].c_str(), "111193", 6) == 0 ||
  	 strncmp(m_pcdr[ATTRI_MSISDN_B_ORG].c_str(), "068", 3) == 0)
  {
  	m_pcdr[ATTRI_IPACCESS_TYPE] = "2";
  }
  else if(strncmp(m_pcdr[ATTRI_MSISDN_B_ORG].c_str(), "17911", 5) == 0 ||
  	 strncmp(m_pcdr[ATTRI_MSISDN_B_ORG].c_str(), "17912", 5) == 0 ||
  	 strncmp(m_pcdr[ATTRI_MSISDN_B_ORG].c_str(), "17991", 5) == 0 ||
  	 strncmp(m_pcdr[ATTRI_MSISDN_B_ORG].c_str(), "172", 3) == 0 ||
  	 strncmp(m_pcdr[ATTRI_MSISDN_B_ORG].c_str(), "161", 3) == 0)
  {
  	m_pcdr[ATTRI_IPACCESS_TYPE] = "4";
  }
  else
  {
  	m_pcdr[ATTRI_IPACCESS_TYPE] = "3";
  }
  m_pcdr[ATTRI_MSISDN_B] = m_pcdr[ATTRI_MSISDN_B_ORG];
  return true;
}

//获取中继用户类型
bool LocalizeIPCdrSetProc::getTypeByTrunkUser(const T_ATTRI_VAL &r_phone,
        const T_DATE_TIME &r_beginTime,
        T_ATTRI_VAL &r_usertype,
        T_ATTRI_VAL &r_areacode)
{
    PO_TrunkUser po;
    vector<PO_TrunkUser*> rpo;
    vector<PO_TrunkUser*>::iterator itr;
    
    po.recordStruct.begin_phone = r_phone;
    po.recordStruct.end_phone = r_phone;
    if(m_PO_TrunkUserContainer->searchMulti(po,rpo))
    {
	    for(itr = rpo.begin(); itr != rpo.end(); itr++)
	    {
	 			if((*itr)->recordStruct.m_beginDate <= r_beginTime &&
					 (*itr)->recordStruct.m_endDate >= r_beginTime )
				{
					r_usertype = (*itr)->recordStruct.usertype;
					r_areacode = (*itr)->recordStruct.area_code;
					return true;
				}
		  }
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////
INSTALL_PROCESSBASE_INSTANCE(LocalizeIPPAAreaSetProc);

int LocalizeIPPAAreaSetProc::execute()
{
  T_ATTRI_VAL r_tvalue;
  char r_sctr[24+1];

  m_PO_BpsPstnNumsegContainer = (ParamContainer<PO_BpsPstnNumseg>*) PMHelperall::GetParamContainer("PO_BpsPstnNumseg");
  m_PO_BpsRelationContainer   = (ParamContainer<PO_BpsRelation>*) PMHelperall::GetParamContainer("PO_BpsRelation");

  //长途类型重置
    if (isSameArea(m_pcdr[ATTRI_VISIT_CODE_A],m_pcdr[ATTRI_HOME_CODE_B]) &&
    	  !isSameArea(m_pcdr[ATTRI_HOME_CODE_A],m_pcdr[ATTRI_HOME_CODE_B]))
    {
        if (m_pcdr[ATTRI_USER_HOME_TYPE_A] < AREA_TYPE_NATIONAL )
            m_pcdr[ATTRI_LONG_TYPE1] = LONG_TYPE_PROV; //省内长途
        else
            m_pcdr[ATTRI_LONG_TYPE1] = LONG_TYPE_NATIONAL; //国内长途
    }

	//特殊前缀类型
	if(!setSpecialIPAreaType())
	{
		//设置相应区间号并进行同区间判断
		setSameAreaType();
	}

/*
计费方号码规整：
1. 193散户话单   少量   32
   193注册用户（5001）：带0的区号+电话号码
2. 193专线话单          31
   193话音专线（5002）：
   以10170开头的号码--直接查定购；
   电信号码--区号+电话号码
   中继号码--区号+'-'+中继号码
3. 17911专线话单        33
   IP话音专线（5003）：
   以10170开头的号码--直接查定购；
   电信号码--区号＋电话号码；
   中继号码--区号+'-'+中继号码
*/

  if(m_pcdr[ATTRI_IP_SERV_TYPE_A] == SERVICE_TYPE_193_SCATTER)
  {
  	if(strncmp(m_pcdr[ATTRI_VISIT_CODE_A].c_str(), "0", 1) == 0)
  	{
  		m_pcdr[ATTRI_MSISDN_A] = m_pcdr[ATTRI_VISIT_CODE_A]+m_pcdr[ATTRI_MSISDN_A];
		  return 0;
  	}
  }
  else if(m_pcdr[ATTRI_IP_SERV_TYPE_A] == SERVICE_TYPE_193_TRUNK)
  {
	  if(strncmp(m_pcdr[ATTRI_BUSI_TYPE].c_str(), "1", 1) == 0) //1根据中继计费 其他按号码计费
	  {
		  memset(r_sctr,0,sizeof(r_sctr));
		  sprintf(r_sctr,"%s-%s",m_pcdr[ATTRI_VISIT_CODE_A].c_str(),m_pcdr[ATTRI_TRUNKGROUPIN].c_str());
		  m_pcdr[ATTRI_MSISDN_A] = r_sctr;
	
		 	//按中继计费归属肯定是交换机归属
		 	//m_pcdr[ATTRI_HOME_CODE_A]      = m_pcdr[ATTRI_VISIT_CODE_A];
			//m_pcdr[ATTRI_USER_ASP_A]       = m_Config->m_operatorType;
			//m_pcdr[ATTRI_USER_HOME_TYPE_A] = AREA_TYPE_LOCAL;
			return 0;
	  }
	  else if(strncmp(m_pcdr[ATTRI_MSISDN_A].c_str(), "10170", 5) == 0)
	  {
		  return 0;
	  }
	  else if(m_pcdr[ATTRI_USER_SERVICE_TYPE_A] == SERVICE_TYPE_FIX || m_pcdr[ATTRI_USER_SERVICE_TYPE_A] == SERVICE_TYPE_HBS)
	  {
		  memset(r_sctr,0,sizeof(r_sctr));
		  sprintf(r_sctr,"%s%s",m_pcdr[ATTRI_VISIT_CODE_A].c_str(),m_pcdr[ATTRI_MSISDN_A].c_str());
		  m_pcdr[ATTRI_MSISDN_A] = r_sctr;
			return 0;
	  }
	  //主叫号码为手机号码，trash处理
/*
	  else if(isMobile(m_pcdr[ATTRI_MSISDN_A]))
	  {
	  	m_pcdr[ATTRI_TRASH_FLAG]=ATTRI_TRASH_Y;
			return 0;
	  }
*/
  }
  else if(m_pcdr[ATTRI_IP_SERV_TYPE_A] == SERVICE_TYPE_17911_TRUNK)
  {
	  if(strncmp(m_pcdr[ATTRI_BUSI_TYPE].c_str(), "1", 1) == 0) //1根据中继计费 其他按号码计费
	  {
		  memset(r_sctr,0,sizeof(r_sctr));
		  sprintf(r_sctr,"%s-%s",m_pcdr[ATTRI_VISIT_CODE_A].c_str(),m_pcdr[ATTRI_TRUNKGROUPIN].c_str());
		  m_pcdr[ATTRI_MSISDN_A] = r_sctr;
	
		 	//按中继计费归属肯定是交换机归属
		 	m_pcdr[ATTRI_HOME_CODE_A]      = m_pcdr[ATTRI_VISIT_CODE_A];
			m_pcdr[ATTRI_USER_ASP_A]       = m_Config->m_operatorType;
			m_pcdr[ATTRI_USER_HOME_TYPE_A] = AREA_TYPE_LOCAL;
			return 0;
	  }
	  else if(strncmp(m_pcdr[ATTRI_MSISDN_A].c_str(), "10170", 5) == 0)
	  {
		  return 0;
	  }
	  else if(m_pcdr[ATTRI_USER_SERVICE_TYPE_A] == SERVICE_TYPE_FIX || m_pcdr[ATTRI_USER_SERVICE_TYPE_A] == SERVICE_TYPE_HBS)
	  {
		  memset(r_sctr,0,sizeof(r_sctr));
		  sprintf(r_sctr,"%s%s",m_pcdr[ATTRI_VISIT_CODE_A].c_str(),m_pcdr[ATTRI_MSISDN_A].c_str());
		  m_pcdr[ATTRI_MSISDN_A] = r_sctr;
			return 0;
	  }
	  //主叫号码为手机号码，trash处理
/*
	  else if(isMobile(m_pcdr[ATTRI_MSISDN_A]))
	  {
	  	m_pcdr[ATTRI_TRASH_FLAG]=ATTRI_TRASH_Y;
			return 0;
	  }
*/
  }

	 //用保留的原始A号码号码替换规整后号码 走不到
	 //m_pcdr[ATTRI_MSISDN_A] = m_pcdr[ATTRI_MSISDN_A_ORG];


	return 0;
}


bool LocalizeIPPAAreaSetProc::setSpecialIPAreaType()
{
	T_ATTRI_VAL r_tmp = m_pcdr[ATTRI_VISIT_CODE_A] + "1001";
	T_ATTRI_VAL r_servtype_b = "00";


  if(strncmp(m_pcdr[ATTRI_MSISDN_B_ORG].c_str(), "200", 3) == 0  ||
  	 strncmp(m_pcdr[ATTRI_MSISDN_B_ORG].c_str(), "1001", 4) == 0 ||
  	 strncmp(m_pcdr[ATTRI_MSISDN_B_ORG].c_str(), r_tmp.c_str(), r_tmp.length()) == 0)
  {
  	r_servtype_b = "50";
  }
  else if(strncmp(m_pcdr[ATTRI_MSISDN_B_ORG].c_str(), "160", 3) == 0)
  {
  	r_servtype_b = "51";
  }
  else if(strncmp(m_pcdr[ATTRI_MSISDN_B_ORG].c_str(), "165", 3) == 0)
  {
  	r_servtype_b = "52";
  }
  else if(strncmp(m_pcdr[ATTRI_MSISDN_B_ORG].c_str(), "175", 3) == 0)
  {
  	 if(m_pcdr[ATTRI_HOME_CODE_B] == "0527")
  	 {
  	 	  r_servtype_b = "53";
  	 }
  	 else if(m_pcdr[ATTRI_HOME_CODE_B] == "025")
  	 {
        r_servtype_b = "54";
  	 }
  }

  if(r_servtype_b != "00")
  {
 	  m_pcdr[ATTRI_USER_SERVICE_TYPE_B] =  r_servtype_b;
 	  return true;
  }

	return false;
}

bool LocalizeIPPAAreaSetProc::setSameAreaType()
{
	T_ATTRI_VAL	r_eacode_a;
	T_ATTRI_VAL	r_eacode_b;
	T_ATTRI_VAL r_connecttype;

	if(m_pcdr[ATTRI_HOME_CODE_A] == m_pcdr[ATTRI_VISIT_CODE_A] &&
		 m_pcdr[ATTRI_HOME_CODE_A] == m_pcdr[ATTRI_HOME_CODE_B] &&
		 m_pcdr[ATTRI_IP_SERV_TYPE_A] != SERVICE_TYPE_193_SCATTER)
	{
		m_pcdr[ATTRI_PA_TYPE] = "0"; //区内
    m_pcdr[ATTRI_LONG_TYPE1] = LONG_TYPE_NONE;   //区内肯定为非长途
	}
	else
	{
		return false;
	}

	if(!getBusinessAreaByphone(m_pcdr[ATTRI_MSISDN_A],m_pcdr[ATTRI_HOME_CODE_A],r_eacode_a))
	{
		r_eacode_a = m_pcdr[ATTRI_HOME_CODE_A];
	}

	if(!getBusinessAreaByphone(m_pcdr[ATTRI_MSISDN_B],m_pcdr[ATTRI_HOME_CODE_B],r_eacode_b))
	{
		r_eacode_b = m_pcdr[ATTRI_HOME_CODE_B];
	}

  if(getconnectTypeByABBusiness(m_pcdr[ATTRI_VISIT_CODE_A],r_eacode_a,r_eacode_b,r_connecttype))
  {
  	 if(r_connecttype[0] == '9')
  	 {
  	 	 m_pcdr[ATTRI_PA_TYPE] = "1"; //区间
  	 }
  }

//cout<<"r_eacode_a:"<<r_eacode_a<<endl;
//cout<<"r_eacode_b:"<<r_eacode_b<<endl;
//cout<<"m_pcdr[ATTRI_PA_TYPE]:"<<m_pcdr[ATTRI_PA_TYPE]<<endl;

	return true;
}

//根据用户号段获取中继用户业务代码
bool LocalizeIPPAAreaSetProc::getBusinessAreaByphone(const T_ATTRI_VAL &r_phone,
	       const T_ATTRI_VAL &r_areacode,
	       T_ATTRI_VAL &r_businessareacode)
{
    PO_BpsPstnNumseg po;
    vector<PO_BpsPstnNumseg*> rpo;
    vector<PO_BpsPstnNumseg*>::iterator itr;
		for(int i=5;i>0;i--)
		{
			 po.recordStruct.m_numberhead=r_phone.substr(0,i);
			 po.recordStruct.m_areacode=r_areacode;
	     if(m_PO_BpsPstnNumsegContainer->searchMulti(po,rpo))
	     {
		     for(itr = rpo.begin(); itr != rpo.end(); itr++)
		     {
		 	 		 if((*itr)->recordStruct.m_areacode <= r_areacode)
			 		 {
			 			  r_businessareacode = (*itr)->recordStruct.m_businessareacode;
			 			  return true;
			 		  }
			   }
	     }
		}
    return false;
}

    //根据AB用户区码和交换机归属获取获取用户之间的网络类型 TD_BPS_BUSINESS_AREA_REL
bool LocalizeIPPAAreaSetProc::getconnectTypeByABBusiness(const T_ATTRI_VAL &r_areacode,
         const T_ATTRI_VAL &r_businessareacode_a,
         const T_ATTRI_VAL &r_businessareacode_b,
         T_ATTRI_VAL &r_connecttype)
{
   PO_BpsRelation po;
	 PO_BpsRelation* rpo;

 	 po.recordStruct.m_areacode = r_areacode;
 	 po.recordStruct.m_businessareacode1 = r_businessareacode_a;
   po.recordStruct.m_businessareacode2 = r_businessareacode_b;
	 if (m_PO_BpsRelationContainer->search(po, rpo))
	 {
	 	r_connecttype = (*rpo).recordStruct.m_connecttype;
	 	return true;
	 }
	 
	 return false;
}

///////////////////////////////////////////////////////////////////////////
INSTALL_PROCESSBASE_INSTANCE(LocalizeIPSwitchSetProc);

int LocalizeIPSwitchSetProc::execute()
{
  //无锡小天鹅的中继（在南京的华为CC08平台上的如下中继认为是小天鹅的中继）000094 000095 000096 000097 000099
  //预处理规则配置
/*
	if(m_pcdr[ATTRI_FILE_NO].length()>=4 &&
		 m_pcdr[ATTRI_FILE_NO].substr(m_pcdr[ATTRI_FILE_NO].length()-4,4) == "CC08" &&
		 m_pcdr[ATTRI_VISIT_CODE_A] == "025")
	{
		 if(m_pcdr[ATTRI_TRUNKGROUPIN] == "000094" || m_pcdr[ATTRI_TRUNKGROUPIN] == "000095" || m_pcdr[ATTRI_TRUNKGROUPIN] == "000096" ||
		 	  m_pcdr[ATTRI_TRUNKGROUPIN] == "000097" || m_pcdr[ATTRI_TRUNKGROUPIN] == "000099")
		 m_pcdr[ATTRI_VISIT_CODE_A] = "0510";
		 else if(m_pcdr[ATTRI_TRUNKGROUPOUT] == "000094" || m_pcdr[ATTRI_TRUNKGROUPOUT] == "000095" || m_pcdr[ATTRI_TRUNKGROUPOUT] == "000096" ||
		 	  m_pcdr[ATTRI_TRUNKGROUPOUT] == "000097" || m_pcdr[ATTRI_TRUNKGROUPOUT] == "000099")
		 m_pcdr[ATTRI_VISIT_CODE_A] = "0510";
	}
*/
	return 0;
}

///////////////////////////////////////////////////////////////////////////
INSTALL_PROCESSBASE_INSTANCE(LocalizeGprsSpecialProc);

int LocalizeGprsSpecialProc::execute()
{
	setdecTohexAddr(m_pcdr[ATTRI_GPRS_SGSN],m_pcdr[ATTRI_GPRS_SGSN_HEX]);

	return 0;
}

///////////////////////////////////////////////////////////////////////////
INSTALL_PROCESSBASE_INSTANCE(LocalizeSpSpecialProc);

int LocalizeSpSpecialProc::execute()
{
  m_PO_SpServCodeSpnameContainer  = (ParamContainer<PO_SpServCodeSpname>*) PMHelperall::GetParamContainer("PO_SpServCodeSpname");
  m_PO_ServiceCodeSpnameContainer = (ParamContainer<PO_ServiceCodeSpname>*) PMHelperall::GetParamContainer("PO_ServiceCodeSpname");

  m_spProperty.begin_time   = m_pcdr[ATTRI_BEGIN_DATE]+m_pcdr[ATTRI_BEGIN_TIME];
  m_spProperty.end_time     = m_pcdr[ATTRI_END_DATE]+m_pcdr[ATTRI_END_TIME];

 //联通在信
 if(m_pcdr[ATTRI_SOURCE_TYPE]==SOURCE_TYPE_SMS_PSMG || m_pcdr[ATTRI_SOURCE_TYPE]==SOURCE_TYPE_SMS_NSMG)
 {
    if (getSpServCodeName(m_pcdr[ATTRI_CALL_TYPE],m_pcdr[ATTRI_SP_CODE],m_pcdr[ATTRI_OPER_CODE],m_pcdr[ATTRI_SERVICE_CODE],m_spProperty))
    {
       m_pcdr[ATTRI_MSISDN_B_FOR_ROAM]    = m_spProperty.service_code;
       m_pcdr[ATTRI_USER_SERVICE_TYPE_B]  = m_spProperty.serv_type;
       m_pcdr[ATTRI_SM_SERV_CODE_TYPE_B]  = m_spProperty.serv_code_type;
       m_pcdr[ATTRI_USER_ASP_B]           = m_spProperty.asp;
       m_pcdr[ATTRI_SM_TYPESTAT]          = m_spProperty.sm_typestat;
       m_pcdr[ATTRI_USER_HOME_TYPE_B]     = m_spProperty.home_type;
       m_pcdr[ATTRI_SP_SERV_AREA]         = m_spProperty.sp_serv_area;
       m_pcdr[ATTRI_SP_PROV_CODE]         = m_spProperty.prov_code;
       m_pcdr[ATTRI_SP_NAME]              = m_spProperty.sp_name;
       return 1;
    }
    else
    {
        m_pcdr[ATTRI_ERROR_CODE]=SM_ERROR_SPCODE;
        return 1;
    }
 }

 //PPS联通在信上行的话单中PDSCP不提供企业代码，由计费进行匹配，对于那种一个接入码对应多个企业代码的情况将随机进行匹配
 if((m_pcdr[ATTRI_SOURCE_TYPE]==SOURCE_TYPE_SMS_PPS1 || m_pcdr[ATTRI_SOURCE_TYPE]==SOURCE_TYPE_SMS_PPS2) && m_pcdr[ATTRI_CALL_TYPE]==CALL_TYPE_SMO)
 {
 	 //1. 智能网点对点短信-A2，上行话单发移动、联通、小灵通之外的话单，SERVICE_CODE填OTHER_PARTY
   //2. 智能网联通在信-A3，上行话单发移动、联通、小灵通的话单作无效处理，其余的话单SERVICE_CODE填OTHER_PARTY
 	  m_pcdr[ATTRI_SERVICE_CODE] = m_pcdr[ATTRI_MSISDN_B];
    if (getSpServCodeName(m_pcdr[ATTRI_CALL_TYPE],m_pcdr[ATTRI_SERVICE_CODE],m_spProperty))
    {
       m_pcdr[ATTRI_MSISDN_B_FOR_ROAM]    = m_spProperty.service_code;
       m_pcdr[ATTRI_USER_SERVICE_TYPE_B]  = m_spProperty.serv_type;
       m_pcdr[ATTRI_SM_SERV_CODE_TYPE_B]  = m_spProperty.serv_code_type;
       m_pcdr[ATTRI_USER_ASP_B]           = m_spProperty.asp;
       m_pcdr[ATTRI_SM_TYPESTAT]          = m_spProperty.sm_typestat;
       ROR_CODE]=SM_ERROR_SPCODE;
        return 1;
    }
 }

 //PPS鑱旈
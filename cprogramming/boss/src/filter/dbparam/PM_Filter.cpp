#include "PM_Filter.h"
#include "base/StringUtil.h"
#include "base/supdebug.h"
#include <fstream>
#include <map>


PM_Filter::PM_Filter()
{
}

PM_Filter::~PM_Filter()
{
	m_paramContainerList.clear();
}

bool PM_Filter::getData()
{
	if(ParamManager::getData()==false)
	{
		return false;
	}
	return afterGetData();
}

bool PM_Filter::afterGetData()
{
	//建立内部指针关联
	m_PO_B_CompCondContainer.buildNextRelation();
	
	//建立外部指针关联
	brCondRuleRela();
	
	if (isLoopDefineCondition())	 //组合条件存在循环定义
	{
		return false;
	}
	
	return true;
}

//判断组合条件是否存在循环定义
bool PM_Filter::isLoopDefineCondition()
{	
	vector<PO_B_CompCond> & vCond = m_PO_B_CompCondContainer.getParamVector();
	vector<PO_B_CompCond>::iterator iter=vCond.begin();
	for(; iter!=vCond.end(); ++iter)
	{		
		if ( (*iter).recordStruct.m_condRule != NULL)
		{
			//if ( ! strcmp(typeid(*(*iter).recordStruct.m_condRule).name(),"PO_B_CompCond") )
			if ( typeid(*(*iter).recordStruct.m_condRule) != typeid(PO_B_CompCond) )
			{
			/*
			for(set<int>::iterator r_test = m_idSet.begin();r_test != m_idSet.end();++r_test)
			{
			cout<<" "<<*r_test<<" ,";
			}
			cout<<endl;
				*/
				m_idSet.clear();
				m_idSet.insert((*iter).recordStruct.m_condId);
				if (!addCondRule( (*iter).recordStruct.m_subCondId, vCond))
				{
					return true;
				}
			}
		}
	}
	return false;
}

bool	PM_Filter::addCondRule(int &subCondRuleId, 
							   vector<PO_B_CompCond> &vCond)
{
	if (m_idSet.find(subCondRuleId) != m_idSet.end())
	{
		char errorMessage[128];
		sprintf(errorMessage,"addCondRule : PO_B_CompCond exist loop define :condId=%d",subCondRuleId); 
		m_errorMessage=errorMessage;
#ifdef _DEBUG_
		cout<<m_errorMessage<<__FILE__<<__LINE__<<endl;
#endif
		return false;
	}
	
	m_idSet.insert(subCondRuleId);
	
	PO_B_CompCond po;
	po.recordStruct.m_condId = subCondRuleId;
	po.recordStruct.m_orderNo = 0;
	
	vector<PO_B_CompCond>::iterator itr;
	itr=lower_bound(vCond.begin(), vCond.end(),	po);
	for(;itr!=vCond.end();++itr)
	{
		if (!((*itr) == po))
			break;
		if ( (*itr).recordStruct.m_condRule != NULL)
		{
			//if ( ! strcmp(typeid(*(*itr).recordStruct.m_condRule).name(),"PO_B_CompCond") )
			if ( typeid(*(*itr).recordStruct.m_condRule) != typeid(PO_B_CompCond) )
			{
				if (!addCondRule((*itr).recordStruct.m_subCondId, vCond))
				{
					return false;
				}
			}
		}
	}
	return true;	
}



void PM_Filter::brCondRuleRela()
{
	PO_B_CompCond po;
	PO_B_CompCond *rpo;
	
	vector<PO_B_CompCond> & vCondRuleRela = 
							 m_PO_B_CompCondContainer.getParamVector();	
	
	vector<PO_B_CompCond>::iterator iterRela = vCondRuleRela.begin();	
	for ( ; iterRela!=vCondRuleRela.end(); ++iterRela)
	{
		po.recordStruct.m_condId = (*iterRela).recordStruct.m_subCondId;
		po.recordStruct.m_orderNo = 0;
		if (m_PO_B_CompCondContainer.search(po, rpo)>0)
		{
			(*iterRela).recordStruct.m_condRule = rpo;
		}
		else
		{
			(*iterRela).recordStruct.m_condRule = NULL;
		}
	}
}


//##ModelId=3B8F7F0A01BE
bool PM_Filter::addParamObject()
{
    ParamContainerBase *Param1;
	
    Param1=&m_PO_TimeMethodBindContainer;  //add by shenglj 2009-10-16 11:28:00
    m_paramContainerList.push_back(Param1);
	
    Param1=&m_PO_NetTypeCodeBindContainer; //add by shenglj 2009-04-06 22:33:00
    m_paramContainerList.push_back(Param1);
	
    Param1=&m_PO_SmServCodeContainer;
    m_paramContainerList.push_back(Param1);
	
    Param1=&m_PO_SpServCodeContainer;
    m_paramContainerList.push_back(Param1);
	
	/**/
    Param1=&m_PO_AreaCodeImsiIContainer;
    m_paramContainerList.push_back(Param1);
	
    Param1=&m_PO_HomeAreaCodeIContainer;
    m_paramContainerList.push_back(Param1);
	
    Param1=&m_PO_HomeAreaCodeNContainer;
    m_paramContainerList.push_back(Param1);
	
    Param1=&m_PO_HomeAreaCodeOContainer;
    m_paramContainerList.push_back(Param1);
	
    Param1=&m_PO_HomeAreaCodePContainer;
    m_paramContainerList.push_back(Param1);
	
    Param1=&m_PO_HomeAreaCodeByPhoneNContainer;
    m_paramContainerList.push_back(Param1);
	
    Param1=&m_PO_HomeAreaCodeBySSPContainer;
    m_paramContainerList.push_back(Param1);
	
	Param1=&m_PO_SpOperFeeContainer;
	m_paramContainerList.push_back(Param1);
	
	Param1=&m_PO_UssdServCodeContainer;
	m_paramContainerList.push_back(Param1);
	
    Param1 = &m_PO_MmsServCodeBySpOperContainer;
    m_paramContainerList.push_back(Param1);
	
    Param1 = &m_PO_AspByMsisdnHeaderContainer;
    m_paramContainerList.push_back(Param1);
	
    Param1 = &m_PO_NrChannelByProvCodeContainer;
    m_paramContainerList.push_back(Param1);
	
	//////////////////////////////////////////////////
    Param1 = &m_PO_DetailChannelContainer;
    m_paramContainerList.push_back(Param1);
	
    Param1 = &m_PO_DetailChannelGsmContainer;
    m_paramContainerList.push_back(Param1);
	
    Param1 = &m_PO_DetailChannelSmContainer;
    m_paramContainerList.push_back(Param1);
	
    Param1 = &m_PO_DetailChannelGprsContainer;
    m_paramContainerList.push_back(Param1);
	
    Param1 = &m_PO_DetailChannelMiscContainer;
    m_paramContainerList.push_back(Param1);
	
    Param1 = &m_PO_UserChannelContainer;
    m_paramContainerList.push_back(Param1);
	
    Param1 = &m_PO_VirtualAreaContainer;
    m_paramContainerList.push_back(Param1);
	
    Param1 = &m_PO_HomeAreaCodeCContainer;
    m_paramContainerList.push_back(Param1);
	
    Param1 = &m_PO_HomeAreaCodeImsiNContainer;
    m_paramContainerList.push_back(Param1);
	
    Param1 = &m_PO_HomeAreaCodeImsiPContainer;
    m_paramContainerList.push_back(Param1);
	
    Param1 = &m_PO_MscByAliasContainer;
    m_paramContainerList.push_back(Param1);
	
    Param1 = &m_PO_VisitAreaCodeLacContainer;
    m_paramContainerList.push_back(Param1);
	
    Param1 = &m_PO_AVByMscCellidContainer;
    m_paramContainerList.push_back(Param1);
	
    Param1 = &m_PO_AVByMscLacContainer;
    m_paramContainerList.push_back(Param1);
	
    Param1 = &m_PO_AVByMscLacCellidContainer;
    m_paramContainerList.push_back(Param1);
	
    Param1 = &m_PO_AVByMscContainer;
    m_paramContainerList.push_back(Param1);
	
    Param1 = &m_PO_AVByLacContainer;
    m_paramContainerList.push_back(Param1);
	
    Param1 = &m_PO_AVByCellidContainer;
    m_paramContainerList.push_back(Param1);
	
    Param1 = &m_PO_AVByLacCellidContainer;
    m_paramContainerList.push_back(Param1);
	
    Param1 = &m_PO_HomeAreaCodeContainer;
    m_paramContainerList.push_back(Param1);
	
    Param1 = &m_PO_FixTelephoneContainer;
    m_paramContainerList.push_back(Param1);
	
    Param1 = &m_PO_HomeAreaCodeBySpecialMsisdnContainer;
    m_paramContainerList.push_back(Param1);
	
    Param1 = &m_PO_TransNumberContainer;
    m_paramContainerList.push_back(Param1);
	
    Param1 = &m_PO_TransNumberCNCContainer;
    m_paramContainerList.push_back(Param1);    //add by shenglj 2009-03-26 10:43:00
	//
    Param1 = &m_PO_SpecialPhoneGroupContainer;
    m_paramContainerList.push_back(Param1);
	
    Param1 = &m_PO_SpecialPhoneGroupCNCContainer;  //add by shenglj 2009-03-26 10:43:00
    m_paramContainerList.push_back(Param1);
	
    Param1 = &m_PO_VoiceMagzineContainer;
    m_paramContainerList.push_back(Param1);
	
    Param1 = &m_PO_ProvInnerCodeContainer;
    m_paramContainerList.push_back(Param1);
	
    Param1 = &m_PO_ProvFixTelephoneContainer;
    m_paramContainerList.push_back(Param1);
	
    Param1 = &m_PO_EdgeCellIdContainer;
    m_paramContainerList.push_back(Param1);
	
    Param1 = &m_PO_EdgeLacCellIdContainer;
    m_paramContainerList.push_back(Param1);
	
    Param1 = &m_PO_EdgeMscCellIdContainer;
    m_paramContainerList.push_back(Param1);
	
    Param1 = &m_PO_EdgeMscLacCellIdContainer;
    m_paramContainerList.push_back(Param1);
	
    Param1 = &m_PO_EdgeVisitCodeLacCellIdContainer;
    m_paramContainerList.push_back(Param1);
	
    Param1 = &m_PO_AreaCodeInterAspContainer;
    m_paramContainerList.push_back(Param1);
	
    Param1 = &m_PO_SpecialPhoneContainer;
    m_paramContainerList.push_back(Param1);
	
    Param1 = &m_PO_SpecialPhoneCNCContainer;  //add by shenglj 2009-03-26 10:43:00
    m_paramContainerList.push_back(Param1);
	
    Param1 = &m_PO_B_ObjectContainer;
    m_paramContainerList.push_back(Param1);
	
    Param1 = &m_PO_B_CompCondContainer;
    m_paramContainerList.push_back(Param1);
	
    Param1 = &m_PO_B_SimpleCondContainer;
    m_paramContainerList.push_back(Param1);
	
    Param1 = &m_PO_B_EventelemContainer;
    m_paramContainerList.push_back(Param1);
	//GPRS
    Param1 = &m_PO_GprsSgsnAddrContainer;
    m_paramContainerList.push_back(Param1);
	
    Param1 = &m_PO_GprsApnContainer;
    m_paramContainerList.push_back(Param1);
	
    Param1 = &m_PO_GprsApnniTypeContainer;
    m_paramContainerList.push_back(Param1);
	
    Param1 = &m_PO_SatelCarContainer;
    m_paramContainerList.push_back(Param1);
	
    Param1 = &m_PO_TrunkContainer;
    m_paramContainerList.push_back(Param1);
	
    Param1 = &m_PO_StateGroupContainer;
    m_paramContainerList.push_back(Param1);
	
    Param1 = &m_PO_VistitAreaRoamContainer;
    m_paramContainerList.push_back(Param1);
	
    Param1 = &m_PO_HomeAreaCodeFixContainer;
    m_paramContainerList.push_back(Param1);
	
    Param1 = &m_PO_ExchangeToSectionFixContainer;
    m_paramContainerList.push_back(Param1);   //add by shenglj 2009-08-14 11:06:00
	
    Param1 = &m_PO_FixTrunkContainer;
    m_paramContainerList.push_back(Param1);   //add by shenglj 2009-03-27 11:01:00
	
    Param1 = &m_PO_MsisdnSectionFixContainer;
    m_paramContainerList.push_back(Param1);   //add by shenglj 2009-08-14 11:06:00
	
    Param1 = &m_PO_SectionRelFixContainer;
    m_paramContainerList.push_back(Param1);   //add by shenglj 2009-08-14 11:06:00
	
    Param1 = &m_PO_NbrGroupContainer;
    m_paramContainerList.push_back(Param1);   //add by shenglj 2009-08-14 11:06:00
	
    Param1 = &m_PO_NumberConvertContainer;
    m_paramContainerList.push_back(Param1);   //add by shenglj 2009-08-14 11:06:00
	
    Param1 = &m_PO_ExchangeDiscountContainer;
    m_paramContainerList.push_back(Param1);   //add by shenglj 2009-08-14 11:06:00
	
    Param1 = &m_PO_FixSpecialPhoneSectionContainer;
    m_paramContainerList.push_back(Param1);   //add by shenglj 2009-09-02 11:06:00
	
    Param1 = &m_PO_JudgeFemtoByLacCellidContainer;
    m_paramContainerList.push_back(Param1);   //add by shenglj 2009-11-03 23:00:00

    Param1 = &m_PO_HomeAreaCodeByBureauContainer;
    m_paramContainerList.push_back(Param1);
	
    return true;
}

void PM_Filter::setHostNameFilter(const string hostName)
{
	setHostName(hostName);
}

void PM_Filter::setAppNameFilter(const string appName)
{
	setAppName(appName);
}

void PM_Filter::setChannelNoFilter(const int channelNo)
{
	setChannelNo(channelNo);
}

/*
* 短信中心号码属性求取
* @param r_number      号码
* @param r_smProperty  短信中心话单结构
* @return true 找到, false 未找到
*/
bool PM_Filter::getSmServCode(const T_MSISDN &r_number,SmProperty &r_smProperty)
{
    vector<PO_SmServCode>::iterator itr;
    vector<PO_SmServCode> &rpo =m_PO_SmServCodeContainer.getParamVector();
	
    for (itr=rpo.begin(); itr!=rpo.end(); ++itr)
    {
        //完全匹配
        if (itr->recordStruct.match_type=="0")
        {
            if (r_number==itr->recordStruct.number)
            {
				if(r_smProperty.begin_time>=itr->recordStruct.begin_time &&
                    r_smProperty.end_time<=itr->recordStruct.end_time &&
                    ( r_smProperty.call_type==itr->recordStruct.call_type ||
					itr->recordStruct.call_type=="*" ) )
                {
					r_smProperty=itr->recordStruct;
					return true;
                }
            }
        }
        //移动号段 判断号码长度是否为11位
        else if(itr->recordStruct.match_type=="2")
        {
            if (strncmp(r_number.c_str(),itr->recordStruct.number.c_str(),itr->recordStruct.number.length()) == 0 &&
                r_number.length() == 11)
            {
				if(r_smProperty.begin_time>=itr->recordStruct.begin_time &&
                    r_smProperty.end_time<=itr->recordStruct.end_time &&
                    ( r_smProperty.call_type==itr->recordStruct.call_type ||
					itr->recordStruct.call_type=="*" ) )
				{
					r_smProperty=itr->recordStruct;
					return true;
                }
            }
        }
        //不完全匹配
        else
        {
            if (strncmp(r_number.c_str(),itr->recordStruct.number.c_str(),itr->recordStruct.number.length()) == 0)
            {
				if(r_smProperty.begin_time>=itr->recordStruct.begin_time &&
                    r_smProperty.end_time<=itr->recordStruct.end_time &&
                    ( r_smProperty.call_type==itr->recordStruct.call_type ||
					itr->recordStruct.call_type=="*" ) )
				{
					r_smProperty=itr->recordStruct;
					return true;
                }
            }
        }
		
    }
    return false;
}


/*
* 梦网网关号码属性求取
* @param r_sp_code      sp代码
* @param r_oper_code   业务代码
* @param r_smProperty  梦网话单结构体
* @return true 找到, false 未找到
*/
bool PM_Filter::getSpServCode(const T_SP_CODE &r_sp_code,const T_OPER_CODE &r_oper_code,SpProperty &r_spProperty)
{
    vector<PO_SpServCode>::iterator itr;
    vector<PO_SpServCode>   &rpo=m_PO_SpServCodeContainer.getParamVector();
	
    for (itr=rpo.begin(); itr!=rpo.end(); ++itr)
    {
        if( r_sp_code==itr->recordStruct.sp_code &&
			(itr->recordStruct.oper_code=="*" || r_oper_code==itr->recordStruct.oper_code))
		{
			if(r_spProperty.begin_time>=itr->recordStruct.begin_time &&
                r_spProperty.end_time<=itr->recordStruct.end_time)
			{
				r_spProperty=itr->recordStruct;
				return true;
			}
		}
    }
	
    return false;
}

/*
* 梦网网关号码属性求取
* @param r_call_type    呼叫类型
* @param r_sp_code      sp代码
* @param r_oper_code   业务代码
* @param r_smProperty  梦网话单结构体
* @return true 找到, false 未找到
*/
bool PM_Filter::getSpServCode(const T_ATTRI_VAL &r_call_type,
							  const T_ATTRI_VAL &r_sp_code,
							  const T_ATTRI_VAL &r_oper_code,
							  const T_ATTRI_VAL &r_service_code,
							  SpProperty &r_spProperty)
{
	PO_SpServCode po;
	vector<PO_SpServCode*> rpo;
	vector<PO_SpServCode*>::iterator itr;
	
	po.recordStruct.sp_code=r_sp_code;
	po.recordStruct.oper_code=r_oper_code;
	if (m_PO_SpServCodeContainer.searchMulti(po,rpo)<=0)
	{
		return false;
	}
	
	for (itr=rpo.begin(); itr!=rpo.end(); itr++)
	{
		if(r_spProperty.begin_time>=(*itr)->recordStruct.begin_time &&
			r_spProperty.end_time<=(*itr)->recordStruct.end_time &&
			strncmp(r_service_code.c_str(),(*itr)->recordStruct.service_code.c_str(),(*itr)->recordStruct.service_code.length())==0)
		{
			r_spProperty=(*itr)->recordStruct;
			return true;
		}
	}
	return false;
}

/*
* 梦网话单是否要进行稽核
* @param r_sp_code      sp代码
* @param r_oper_code    业务代码
* @param r_begintime    开始时间
* @param r_isVerifyState获取稽核用户状态
* @param r_isVerifyOrder获取稽核用户订购关系
* @return true 找到, false 未找到
*/
bool PM_Filter::getSpOperVerify(const T_ATTRI_VAL &r_sp_code,
								const T_ATTRI_VAL &r_oper_code,
								const T_ATTRI_VAL &r_service_code,
								const T_ATTRI_VAL &r_call_type,
								const T_DATE_TIME &r_begintime,
								char	&r_isVerifyState,
								char	&r_isVerifyOrder)
{
	PO_SpServCode po;
	vector<PO_SpServCode*> rpo;
	vector<PO_SpServCode*>::iterator itr;
	
	po.recordStruct.sp_code=r_sp_code;
	po.recordStruct.oper_code=r_oper_code;
	if (m_PO_SpServCodeContainer.searchMulti(po,rpo)<=0)
	{
		return false;
	}
	
	for (itr=rpo.begin(); itr!=rpo.end(); itr++)
	{
		if(r_begintime>=(*itr)->recordStruct.begin_time &&
			r_begintime<=(*itr)->recordStruct.end_time &&
			strncmp(r_service_code.c_str(),(*itr)->recordStruct.service_code.c_str(),(*itr)->recordStruct.service_code.length())==0 &&
			((*itr)->recordStruct.call_type=="*" || (*itr)->recordStruct.call_type==r_call_type))
		{
			r_isVerifyState=(*itr)->recordStruct.is_verifystae;
			r_isVerifyOrder=(*itr)->recordStruct.is_verifyorder;
			return true;
		}
	}
	return false;
}

/*
* 是否是国际来访
* @param r_sp_code      IMSI段
* @param r_begintime    开始时间
* @param r_homeType     获取地区类型
* @return true 找到, false 未找到
*/
bool PM_Filter::isImsiI(const T_ATTRI_VAL &r_imsi,const T_DATE_TIME &r_begintime,T_ATTRI_VAL &r_homeType)
{
    PO_AreaCodeImsiI po;
    vector<PO_AreaCodeImsiI*> rpo;
    vector<PO_AreaCodeImsiI*>::iterator itr;
	
	for(int i=5;i<7;i++)
	{
		po.recordStruct.m_imsi=r_imsi.substr(0,i);
		if(m_PO_AreaCodeImsiIContainer.searchMulti(po,rpo))
		{
			for(itr=rpo.begin();itr!=rpo.end();itr++)
			{
				if(r_begintime>=(*itr)->recordStruct.m_beginTime&&
					r_begintime<=(*itr)->recordStruct.m_endTime)
				{
					r_homeType = (*itr)->recordStruct.m_userType[3];
					return true;
				}
			}
		}
	}
    return false;
}

/*
* 国际来访根据国际区号获取归属信息
* @param msisdn         号码
* @param begintime      开始时间
* @param areaCode       获取国际长途区号
* @param groupId1       获取国际长途分组1
* @param groupIdEx      获取国际长途分组2
* @param groupIdEx1     获取国际长途分组3
* @param groupIdEx2     获取国际长途分组4
* @param groupIdGprs    获取国际GPRS分组
* @param userType       获取用户类型
* @param nationCode     获取国家代码
* @return true 找到, false 未找到
*/
bool PM_Filter::getHomeAreaCodeByMsisdnI(const T_ATTRI_VAL &msisdn,
										 const T_DATE_TIME &begintime,
										 T_ATTRI_VAL &areaCode,
										 T_ATTRI_VAL &groupId1,
										 T_ATTRI_VAL &groupIdEx,
										 T_ATTRI_VAL &groupIdEx1,
										 T_ATTRI_VAL &groupIdEx2,
										 T_ATTRI_VAL &groupIdGprs,
										 T_ATTRI_VAL &userType,
										 T_ATTRI_VAL &nationCode)
{
	PO_HomeAreaCodeI po;
	vector <PO_HomeAreaCodeI*> rpo;
	vector <PO_HomeAreaCodeI*>::iterator itr;
	int i;
	
	for(i=9;i>2;i--)
	{
		po.recordStruct.feeCode=msisdn.substr(0,i);
		if(m_PO_HomeAreaCodeIContainer.searchMulti(po,rpo))
		{
			for(itr = rpo.begin(); itr != rpo.end(); itr++)
			{
				if((*itr)->recordStruct.beginTime <= begintime && (*itr)->recordStruct.endTime >= begintime)
				{
					areaCode=(*itr)->recordStruct.feeCode;
					groupId1=(*itr)->recordStruct.groupId;
					groupIdEx=(*itr)->recordStruct.groupIdEx;
					groupIdEx1=(*itr)->recordStruct.groupIdEx1;
					groupIdEx2=(*itr)->recordStruct.groupIdEx2;
					groupIdGprs=(*itr)->recordStruct.groupIdGprs;
					userType=(*itr)->recordStruct.userType;
					nationCode=(*itr)->recordStruct.nation_code;
					return true;
				}
			}
		}
	}
	return false;
}

/*
* 根据国际区号找长途分组
* @param area_codeI     长途区号
* @param begintime      开始时间
* @param groupId        获取国际长途分组
* @return true 找到, false 未找到
*/
bool PM_Filter::getGroupIdByAreaCodeI(const T_ATTRI_VAL &area_codeI,
									  const T_DATE_TIME &begintime,
									  T_ATTRI_VAL &group_id)
{
	PO_HomeAreaCodeI po;
	vector<PO_HomeAreaCodeI*> rpo;
	vector<PO_HomeAreaCodeI*>::iterator itr;
	
	po.recordStruct.feeCode=area_codeI;
	if (m_PO_HomeAreaCodeIContainer.searchMulti(po,rpo)==0)
	{
		return false;
	}
	
	for (itr=rpo.begin(); itr!=rpo.end(); itr++)
	{
		if(((*itr)->recordStruct.beginTime <= begintime)&&((*itr)->recordStruct.endTime >= begintime))
		{
			group_id=(*itr)->recordStruct.groupId;
			return true;
		}
	}
	return false;
}

/*
* 根据国内区号获取归属信息
* @param msisdn         固网号码
* @param begintime      开始时间
* @param areaCode       获取长途区号
* @param groupId        获取长途分组
* @param userType       获取用户类型
* @return true 找到, false 未找到
*/
bool PM_Filter::getHomeAreaCodeByMsisdnN(const T_ATTRI_VAL &msisdn,
										 const T_DATE_TIME &begintime,
										 T_ATTRI_VAL &areaCode,
										 T_ATTRI_VAL &groupId,
										 T_ATTRI_VAL &userType)
{
	PO_HomeAreaCodeN po;
	vector <PO_HomeAreaCodeN*> rpo;
	vector <PO_HomeAreaCodeN*>::iterator itr;
	int i;
	
	for(i=5;i>2;i--)
	{
		po.recordStruct.feeCode=msisdn.substr(0,i);
		if(m_PO_HomeAreaCodeNContainer.searchMulti(po,rpo))
			break;
	}
	if(i==2)return false;
	for(itr = rpo.begin(); itr != rpo.end(); itr++)
	{
		if(((*itr)->recordStruct.beginTime <= begintime)
			&&((*itr)->recordStruct.endTime >= begintime))
		{
			areaCode=(*itr)->recordStruct.feeCode;
			groupId=(*itr)->recordStruct.groupId;
			userType=(*itr)->recordStruct.userType;
			return true;
		}
	}
	return false;
}

/*
* 根据国内区号找长途分组
* @param msisdn         长途区号
* @param begintime      开始时间
* @param home_type       获取地区类型
* @param groupId        获取长途分组
* @return true 找到, false 未找到
*/
bool PM_Filter::getGroupIdByAreaCodeN(const T_ATTRI_VAL &area_codeN,
									  const T_DATE_TIME &begintime,
									  T_ATTRI_VAL &home_type,
									  T_ATTRI_VAL &group_id)
{
	PO_HomeAreaCodeN po;
	vector<PO_HomeAreaCodeN*> rpo;
	vector<PO_HomeAreaCodeN*>::iterator itr;
	
	po.recordStruct.feeCode=area_codeN;
	if (m_PO_HomeAreaCodeNContainer.searchMulti(po,rpo)==0)
	{
		return false;
	}
	for (itr=rpo.begin(); itr!=rpo.end(); itr++)
	{
		if(((*itr)->recordStruct.beginTime <= begintime)&&((*itr)->recordStruct.endTime >= begintime))
		{
			home_type = (*itr)->recordStruct.userType[3];
			group_id = (*itr)->recordStruct.groupId;
			return true;
		}
	}
	return false;
}

/*
* 根据长途区号获取归属信息（城市代码）
* @param areaCode       长途区号
* @param begintime      开始时间
* @param citycode       获取城市代码
* @param provcode       获取省代码
* @param groupId        获取长途分组
* @param userType       获取用户类型
* @return true 找到, false 未找到
*/
bool PM_Filter::getCitycodeByAreaCode(T_ATTRI_VAL& citycode,
									  const T_DATE_TIME &begintime,
									  const T_ATTRI_VAL &areacode,
									  T_ATTRI_VAL &provcode,
									  T_ATTRI_VAL &groupid,
									  T_ATTRI_VAL &usertype)
{
	PO_HomeAreaCodeN po;
	vector <PO_HomeAreaCodeN*> rpo;
	vector <PO_HomeAreaCodeN*>::iterator itr;
	
	po.recordStruct.feeCode=areacode;
	if(m_PO_HomeAreaCodeNContainer.searchMulti(po,rpo)==0)
	{
		return false;
	}
	for(itr = rpo.begin(); itr != rpo.end(); itr++)
	{
		if(((*itr)->recordStruct.beginTime <= begintime)
			&&((*itr)->recordStruct.endTime >= begintime))
		{
			citycode=(*itr)->recordStruct.cityCode;
			provcode=(*itr)->recordStruct.provCode;
			groupid=(*itr)->recordStruct.groupId;
			usertype=(*itr)->recordStruct.userType;
			return true;
		}
	}
	return false;
}

/*
* 根据城市代码获取归属信息（长途区号）
* @param citycode      城市代码
* @param begintime      开始时间
* @param areaCode       获取长途区号
* @param provcode       获取省代码
* @param groupId        获取长途分组
* @param userType       获取用户类型
* @return true 找到, false 未找到
*/
bool PM_Filter::getAreacodeByCityCode(const T_ATTRI_VAL& citycode,
									  const T_DATE_TIME &begintime,
									  T_ATTRI_VAL &areacode,
									  T_ATTRI_VAL &provcode,
									  T_ATTRI_VAL &groupid,
									  T_ATTRI_VAL &usertype)
{
	PO_HomeAreaCodeC po;
	vector <PO_HomeAreaCodeC*> rpo;
	vector <PO_HomeAreaCodeC*>::iterator itr;
	
	po.recordStruct.cityCode=citycode;
	if(m_PO_HomeAreaCodeCContainer.searchMulti(po,rpo)==0)
		return false;
	for(itr = rpo.begin(); itr != rpo.end(); itr++)
	{
		if(((*itr)->recordStruct.beginTime <= begintime)
			&&((*itr)->recordStruct.endTime >= begintime))
		{
			areacode=(*itr)->recordStruct.feeCode;
			provcode=(*itr)->recordStruct.provCode;
			groupid=(*itr)->recordStruct.groupId;
			usertype=(*itr)->recordStruct.userType;
			return true;
		}
	}
	return false;
}

/*
* 根据城市代码获取归属信息（长途区号）联通漫游出访话单会下一个区号多个城市代码
* @param citycode      城市代码
* @param begintime      开始时间
* @param areaCode       获取长途区号
* @param provcode       获取省代码
* @param groupId        获取长途分组
* @param userType       获取用户类型
* @return true 找到, false 未找到
*/
bool PM_Filter::getAreacodeByCityCodeRoamOut(const T_ATTRI_VAL& citycode,
											 const T_DATE_TIME &begintime,
											 T_ATTRI_VAL &areacode,
											 T_ATTRI_VAL &provcode,
											 T_ATTRI_VAL &groupid,
											 T_ATTRI_VAL &usertype)
{
	PO_VistitAreaRoam po;
	vector <PO_VistitAreaRoam*> rpo;
	vector <PO_VistitAreaRoam*>::iterator itr;
	
	po.recordStruct.cityCode=citycode;
	if(m_PO_VistitAreaRoamContainer.searchMulti(po,rpo)==0)
		return false;
	for(itr = rpo.begin(); itr != rpo.end(); itr++)
	{
		if(((*itr)->recordStruct.beginTime <= begintime)
			&&((*itr)->recordStruct.endTime >= begintime))
		{
			areacode=(*itr)->recordStruct.feeCode;
			provcode=(*itr)->recordStruct.provCode;
			groupid=(*itr)->recordStruct.groupId;
			usertype=(*itr)->recordStruct.userType;
			return true;
		}
	}
	return false;
}

bool PM_Filter::getHomeAreaCodeByMsisdnO(const T_ATTRI_VAL &msisdn,
										 const T_DATE_TIME &begintime,
										 T_ATTRI_VAL &homeAreaCode,
										 T_ATTRI_VAL &groupId,
										 T_ATTRI_VAL &userType,
										 T_ATTRI_VAL &calledType)
{
	PO_HomeAreaCodeO po;
	vector <PO_HomeAreaCodeO*> rpo;
	vector <PO_HomeAreaCodeO*>::iterator itr;
	
	po.recordStruct.m_beginMsisdn=msisdn;
	po.recordStruct.m_endMsisdn=msisdn;
	if(m_PO_HomeAreaCodeOContainer.searchMulti(po,rpo)==0)
		return false;
	for(itr = rpo.begin(); itr != rpo.end(); itr++)
	{
		if(((*itr)->recordStruct.m_beginTime <= begintime)
			&&((*itr)->recordStruct.m_endTime >= begintime))
		{
			homeAreaCode=(*itr)->recordStruct.m_feeCode;
			groupId=(*itr)->recordStruct.m_groupId;
			userType=(*itr)->recordStruct.m_userType;
			//Added by Sunam 2005-12-14
			calledType=(*itr)->recordStruct.m_calledType;
			return true;
		}
	}
	return false;
}

bool PM_Filter::getHomeAreaCodeByMsisdnP(const T_ATTRI_VAL &msisdn,
										 const T_DATE_TIME &begintime,
										 T_ATTRI_VAL &homeAreaCode,
										 T_ATTRI_VAL &groupId,
										 T_ATTRI_VAL &userType,
										 T_ATTRI_VAL &officeCode,
										 T_ATTRI_VAL &calledType)
{
	PO_HomeAreaCodeP po;
	vector <PO_HomeAreaCodeP*> rpo;
	vector <PO_HomeAreaCodeP*>::iterator itr;
	
	po.recordStruct.m_beginMsisdn=msisdn;
	po.recordStruct.m_endMsisdn=msisdn;
	if(m_PO_HomeAreaCodePContainer.searchMulti(po,rpo)==0)
		return false;
	for(itr = rpo.begin(); itr != rpo.end(); itr++)
	{
		if(((*itr)->recordStruct.m_beginTime <= begintime)
			&&((*itr)->recordStruct.m_endTime >= begintime))
		{
			homeAreaCode=(*itr)->recordStruct.m_feeCode;
			groupId=(*itr)->recordStruct.m_groupId;
			userType=(*itr)->recordStruct.m_userType;
			officeCode=(*itr)->recordStruct.m_officeCode;
			//Added by Sunam 2005-12-13
			calledType=(*itr)->recordStruct.m_calledType;
			return true;
		}
	}
	return false;
}

bool PM_Filter::getHomeAreaCodeByPhoneN(const T_ATTRI_VAL &msisdn,
										const T_DATE_TIME &begintime,
										T_ATTRI_VAL &areaCode,
										T_ATTRI_VAL &groupId,
										T_ATTRI_VAL &userType,
										T_ATTRI_VAL &provinceCode,
										T_ATTRI_VAL &calledType)
{
	PO_HomeAreaCodeByPhoneN po;
	vector <PO_HomeAreaCodeByPhoneN*> rpo;
	vector <PO_HomeAreaCodeByPhoneN*>::iterator itr;
	
	po.recordStruct.m_beginMsisdn=msisdn;
	po.recordStruct.m_endMsisdn=msisdn;
	if(m_PO_HomeAreaCodeByPhoneNContainer.searchMulti(po,rpo)==0)
	{
		return false;
	}
	for(itr = rpo.begin(); itr != rpo.end(); itr++)
	{
		if(((*itr)->recordStruct.m_beginTime <= begintime)
			&&((*itr)->recordStruct.m_endTime >= begintime))
		{
			areaCode=(*itr)->recordStruct.m_feeCode;
			groupId=(*itr)->recordStruct.m_groupId;
			userType=(*itr)->recordStruct.m_userType;
			provinceCode=(*itr)->recordStruct.m_provCode;
			calledType = (*itr)->recordStruct.m_calledType;
			return true;
		}
	}
	return false;
}

bool PM_Filter::getHomeAreaCodeBySSP(const T_ATTRI_VAL &msisdn,
									 const T_DATE_TIME &begintime,
									 T_ATTRI_VAL &areaCode,
									 T_ATTRI_VAL &groupId,
									 T_ATTRI_VAL &userType,
									 T_ATTRI_VAL &officeCode,
									 T_ATTRI_VAL &calledType)
{
	PO_HomeAreaCodeBySSP po;
	vector <PO_HomeAreaCodeBySSP*> rpo;
	vector <PO_HomeAreaCodeBySSP*>::iterator itr;
	
	po.recordStruct.m_beginMsisdn=msisdn;
	po.recordStruct.m_endMsisdn=msisdn;
	if(m_PO_HomeAreaCodeBySSPContainer.searchMulti(po,rpo)==0)
		return false;
	for(itr = rpo.begin(); itr != rpo.end(); itr++)
	{
		if(((*itr)->recordStruct.m_beginTime <= begintime)
			&&((*itr)->recordStruct.m_endTime >= begintime))
		{
			areaCode=(*itr)->recordStruct.m_feeCode;
			groupId=(*itr)->recordStruct.m_groupId;
			userType=(*itr)->recordStruct.m_userType;
			officeCode=(*itr)->recordStruct.m_officeCode;
			calledType = (*itr)->recordStruct.m_calledType;
			return true;
		}
	}
	return false;
}


int PM_Filter::getInfoFeeBySpOper(const T_ATTRI_VAL &r_sp_code,
								  const T_ATTRI_VAL &r_oper_code,
								  const T_ATTRI_VAL &r_fee_type,
								  const T_DATE_TIME &r_begintime,
								  int   &r_infofee)
{
	PO_SpOperFee po;
	vector<PO_SpOperFee*> rpo;
	vector<PO_SpOperFee*>::iterator itr;
	
	r_infofee = 0;
	po.recordStruct.spCode=r_sp_code;
	if(m_PO_SpOperFeeContainer.searchMulti(po,rpo)==0)
		return 0; //找不到资料
	
	for(itr=rpo.begin();itr!=rpo.end();itr++)
	{
		if(r_begintime>=(*itr)->recordStruct.m_beginTime && r_begintime<=(*itr)->recordStruct.m_endTime &&
			(r_oper_code==(*itr)->recordStruct.operCode || (*itr)->recordStruct.operCode=="*"))
		{
			if(r_fee_type==(*itr)->recordStruct.feeType || (*itr)->recordStruct.feeType=="*")
			{
				r_infofee = (*itr)->recordStruct.infoFee;
				return 1;
			}
			else
			{
				return -1; //业务代码和费用类型曾经不匹配
			}
		}
	}
	
	return 0;
}


int PM_Filter::getInfoFeeBySpOper(const T_ATTRI_VAL &r_sp_code,
								  const T_ATTRI_VAL &r_oper_code,
								  const T_ATTRI_VAL &r_service_code,
								  const T_ATTRI_VAL &r_fee_type,
								  const T_DATE_TIME &r_begintime,
								  int   &r_infofee)
{
	bool t_flag=false;
	PO_SpOperFee po;
	vector<PO_SpOperFee*> rpo;
	vector<PO_SpOperFee*>::iterator itr;
	
	r_infofee = 0;
	po.recordStruct.spCode=r_sp_code;
	po.recordStruct.operCode=r_oper_code;
	if(m_PO_SpOperFeeContainer.searchMulti(po,rpo)==0)
		return 0; //找不到资料
	
	for(itr=rpo.begin();itr!=rpo.end();itr++)
	{
		if(r_begintime>=(*itr)->recordStruct.m_beginTime && r_begintime<=(*itr)->recordStruct.m_endTime &&
			strncmp(r_service_code.c_str(),(*itr)->recordStruct.serviceCode.c_str(),(*itr)->recordStruct.serviceCode.length())==0)
		{
			//在通信费表中配置，在信息费表中，sp代码、接入码有配置，但是业务代码不一致，那么作为测试单处理
			t_flag=true;
			if(strcmp(r_oper_code.c_str(),(*itr)->recordStruct.operCode.c_str())==0 || (*itr)->recordStruct.operCode=="*")
			{
				if(r_fee_type==(*itr)->recordStruct.feeType || (*itr)->recordStruct.feeType=="*")
				{
					r_infofee = (*itr)->recordStruct.infoFee;
					return 1;
				}
				else
				{
					return -2; //费用类型不匹配
				}
			}
		}
	}
	if(t_flag==true)
		return -1; //业务代码不一致，测试单
	
	return 0;
}

bool PM_Filter::getAspByMsisdnHeader(const T_ATTRI_VAL &r_msisdn,
									 const T_DATE_TIME &r_begintime,
									 T_ASP &r_asp)
{
	PO_AspByMsisdnHeader po;
	PO_AspByMsisdnHeader *rpo;
	int i;
	
	if (r_msisdn.length() < 3)
	{
		return false;
	}
	
	for(i = 4;i > 2;i--)
	{
		if (r_msisdn.length()<i)
		{
			po.recordStruct.m_msisdnSeg = r_msisdn;
		}
		else
		{
			po.recordStruct.m_msisdnSeg = r_msisdn.substr(0,i);
		}
		if(m_PO_AspByMsisdnHeaderContainer.search(po,rpo))
		{
			break;
		}
	}
	if (i==2)
	{
		return false;
	}
	if(r_begintime>=(*rpo).recordStruct.m_beginTime && r_begintime<=(*rpo).recordStruct.m_endTime)
	{
		r_asp = (*rpo).recordStruct.m_asp;
		//r_td_type = (*rpo).recordStruct.m_tdType;
		return true;
	}
	return false;
}


bool PM_Filter::getNrChannelByProvCode(const T_ATTRI_VAL &provCdoe, int &channelNo)
{
	PO_NrChannelByProvCode po;
	PO_NrChannelByProvCode *rpo;
	
	po.recordStruct.m_provCode = provCdoe;
	if ( m_PO_NrChannelByProvCodeContainer.search(po, rpo))
	{
		channelNo = (*rpo).recordStruct.m_channelNo;
	}
	else
	{
		channelNo = 0;
	}
	return true;
}

bool PM_Filter::getDetailChannelItem(const DetailChannelItem &r_detailChannelItem,
									 const int &r_cout,
									 T_ATTRI_VAL &r_filterchannel)
{
    vector<PO_DetailChannel>::iterator itr;
    vector<PO_DetailChannel>   &rpo=m_PO_DetailChannelContainer.getParamVector();
	
    int i;
    bool result;
    for (itr=rpo.begin(); itr!=rpo.end(); ++itr)
    {
		result = true;
		for(i=0;i<r_cout;i++)
		{
			if(itr->recordS
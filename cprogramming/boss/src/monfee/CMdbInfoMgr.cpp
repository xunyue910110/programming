#include "CMdbInfoMgr.h"

bool operator==(const SpSvcKey& left,const SpSvcKey& right)
{
	return ( left.m_spCode == right.m_spCode &&
	         left.m_spBizCode == right.m_spBizCode);
}
    
bool operator<(const SpSvcKey& left,const SpSvcKey& right)
{
	if (left.m_spCode == right.m_spCode )
	{
		return (left.m_spBizCode<right.m_spBizCode);
	}
	else
	{
		return (left.m_spCode<right.m_spCode);
	}
}

CMdbInfoMgr::CMdbInfoMgr()
{    
    m_needUpdated = true;
}


CMdbInfoMgr::~CMdbInfoMgr()
{
}


bool CMdbInfoMgr::initialization()
{
    //自身的初始化工作
    m_needUpdated = true;    
    bool ret = ParamManager::initialization();    
    if(!ret)
    	cout <<m_errorMessage << endl;
    return ret;
}


bool CMdbInfoMgr::end()
{
	//自身清理工作
	if ( !ParamManager::end() )
	{
		return false;
	}	

	return true;
}


bool CMdbInfoMgr::needUpdate()
{	
	//自身更新判断
	m_needUpdated = ParamManager::needUpdate();
	
	return m_needUpdated;
}


bool CMdbInfoMgr::update()
{	
	//自身更新操作
	if ( m_needUpdated )
	{
		if ( !ParamManager::update() )
		{
			return false;
		}
	}
	
	return true;
}


bool CMdbInfoMgr::addParamObject()
{
    ParamContainerBase *paramcontainer;

    paramcontainer = &m_PO_CycPara;
    m_paramContainerList.push_back(paramcontainer);
    
    paramcontainer = &m_PO_SpDiscount;
    m_paramContainerList.push_back(paramcontainer);
    
    paramcontainer = &m_PO_SpTariff;
    m_paramContainerList.push_back(paramcontainer);

    paramcontainer = &m_PO_UserInfo;
    m_paramContainerList.push_back(paramcontainer);

    paramcontainer = &m_PO_UserInfoIncre;
    m_paramContainerList.push_back(paramcontainer);

    paramcontainer = &m_PO_UserDiscount;
    m_paramContainerList.push_back(paramcontainer);

    paramcontainer = &m_PO_UserDiscountIncre;
    m_paramContainerList.push_back(paramcontainer);    
    
    paramcontainer = &m_PO_UserSVCState;
    m_paramContainerList.push_back(paramcontainer);

    paramcontainer = &m_PO_UserSVCStateIncre;
    m_paramContainerList.push_back(paramcontainer);    
    
    paramcontainer = &m_PO_AdcECuserNum;
    m_paramContainerList.push_back(paramcontainer);    
    
    paramcontainer = &m_PO_AdcECuserState;
    m_paramContainerList.push_back(paramcontainer);    
    
    paramcontainer = &m_PO_SysDiscount;
    m_paramContainerList.push_back(paramcontainer);   
    
    // Add by Malone 2008-1-26 start
    paramcontainer = &m_PO_SpDumbness;
    m_paramContainerList.push_back(paramcontainer);
    // Add by Malone 2008-1-26 end
    
    paramcontainer = &m_PO_SpPunish;
    m_paramContainerList.push_back(paramcontainer);
    
    // Add by yih 2008-08-06 start
    paramcontainer = &m_PO_SpRelaDiscnt;
    m_paramContainerList.push_back(paramcontainer);

    paramcontainer = &m_PO_SpRelaDiscntDetail;
    m_paramContainerList.push_back(paramcontainer);
    // Add by yih 2008-08-06 end

    paramcontainer = &m_PO_SysParamInfo;
    m_paramContainerList.push_back(paramcontainer);
     
    return true;
}


bool CMdbInfoMgr::getData()
{
    if (!ParamManager::getData())
    {
        return false;
    }

    return true;
}


bool CMdbInfoMgr::getCycParaFromDate(const string& dateTime,
                                         CycPara& cycPara)
{
    PO_CycPara                     po;
    PO_CycPara                     rpo;

    po.m_record.m_cycStartTime = dateTime;
    po.m_record.m_cycEndTime = dateTime;
    if (m_PO_CycPara.search(po,rpo) == 0)
    {
        return false;
    }
    else
    {
        cycPara = rpo.m_record;
        return true;
    }
}


bool CMdbInfoMgr::getCycIdFromDate(const string& dateTime, int& cycId)
{
    cycId = 0;

    PO_CycPara                     po;
    PO_CycPara                     rpo;

    po.m_record.m_cycStartTime = dateTime;
    po.m_record.m_cycEndTime = dateTime;
    if (m_PO_CycPara.search(po,rpo) == 0)
    {
        return false;
    }
    else
    {
        cycId = rpo.m_record.m_cycId;
        return true;
    }
}

bool CMdbInfoMgr::getCycParaFromCycId(const int& cycId,
                                           CycPara& cycPara)
{
    PO_CycPara     po;
    PO_CycPara     rpo;

    po.m_record.m_cycId = cycId;
    if (m_PO_CycPara.search(po,rpo) == 0)
    {
        return false;
    }
    else
    {
        cycPara = rpo.m_record;
        return true;
    }
}


bool CMdbInfoMgr::getSysParamInfo(const string& sysParamCode,
                                     vector<SysParamInfo>& sysParamInfoVector)
{
	PO_SysParamInfo                        po;
	vector<PO_SysParamInfo>                rposVector;
	vector<PO_SysParamInfo>::iterator      iter;
	
	sysParamInfoVector.clear();
	
	po.m_record.m_sysParamCode=sysParamCode;
	m_PO_SysParamInfo.searchMulti(po,rposVector);
	
	for (iter=rposVector.begin(); iter!=rposVector.end(); iter++)
	{
	    if (iter->m_record.m_useTag == '1')
	    {
	        sysParamInfoVector.push_back(iter->m_record);
	    }
	}
	
	if (sysParamInfoVector.size() == 0)
	{
	    return false;
	}
	else
	{
	    return true;
	}
}


bool CMdbInfoMgr::getUserInfo(const USERID& userId, UserInfo& userInfo)
{
    PO_UserInfoIncre  poInc;
    PO_UserInfoIncre  rpoInc;

    poInc.m_record.m_userId = userId;
    //增量资料中不存在，从全量资料中获取
    if (m_PO_UserInfoIncre.search(poInc,rpoInc) == 0)
    {
        PO_UserInfo       po;
        PO_UserInfo       rpo;

        po.m_record.m_userId = userId;
        if (m_PO_UserInfo.search(po,rpo) == 0)
        {
            return false;
        }
        else
        {
            userInfo.m_userId = rpo.m_record.m_userId;
            userInfo.m_custId = rpo.m_record.m_custId;
            userInfo.m_brandCode = string(rpo.m_record.m_brandCode);
            userInfo.m_eparchyCode = string(rpo.m_record.m_eparchyCode);
            userInfo.m_cityCode = string(rpo.m_record.m_cityCode);
            userInfo.m_userStateCodeSet=string(rpo.m_record.m_userStateCodeSet);
            userInfo.m_userTypeCode = rpo.m_record.m_userTypeCode;
            userInfo.m_accountTag = rpo.m_record.m_accountTag;
            userInfo.m_removeTag = rpo.m_record.m_removeTag;
            userInfo.m_openDate = (char *)rpo.m_record.m_openDate;
            userInfo.m_preDestroyTime = (char *)rpo.m_record.m_preDestroyTime;
            userInfo.m_destroyTime = (char *)rpo.m_record.m_destroyTime;
            userInfo.m_lastStopTime = (char *)rpo.m_record.m_lastStopTime;
            userInfo.m_scoreValue = rpo.m_record.m_scoreValue;
            userInfo.m_creditValue = rpo.m_record.m_creditValue;
            userInfo.m_serialNumber = rpo.m_record.m_serialNumber;

            return true;
        }
    }
    //增量资料中存在，从增量资料中获取
    else
    {
        userInfo.m_userId = rpoInc.m_record.m_userId;
        userInfo.m_custId = rpoInc.m_record.m_custId;
        userInfo.m_brandCode = string(rpoInc.m_record.m_brandCode);
        userInfo.m_eparchyCode = string(rpoInc.m_record.m_eparchyCode);
        userInfo.m_cityCode = string(rpoInc.m_record.m_cityCode);
        userInfo.m_userStateCodeSet=string(rpoInc.m_record.m_userStateCodeSet);
        userInfo.m_userTypeCode = rpoInc.m_record.m_userTypeCode;
        userInfo.m_accountTag = rpoInc.m_record.m_accountTag;
        userInfo.m_removeTag = rpoInc.m_record.m_removeTag;
        userInfo.m_openDate = (char *)rpoInc.m_record.m_openDate;
        userInfo.m_preDestroyTime = (char *)rpoInc.m_record.m_preDestroyTime;
        userInfo.m_destroyTime = (char *)rpoInc.m_record.m_destroyTime;
        userInfo.m_lastStopTime = (char *)rpoInc.m_record.m_lastStopTime;
        userInfo.m_scoreValue = rpoInc.m_record.m_scoreValue;
        userInfo.m_creditValue = rpoInc.m_record.m_creditValue;
        userInfo.m_serialNumber = rpoInc.m_record.m_serialNumber;

        return true;
    }
}

void CMdbInfoMgr::getUserDiscount(const char &dataType,
                                     const UserInfo& userInfo,                                     
                                     const CycPara& cycPara,
                                     vector<MonthFeeDiscount>& discountVector)
{
    vector<MonthFeeDiscount>::iterator    iterd;

    discountVector.clear();    

    // a.获取优惠信息
    IdDiscount                    idDiscount;
    vector<IdDiscount>            sysDiscountVector;
    vector<IdDiscount>            userDiscountVector;
    vector<IdDiscount>::iterator  iter;

    //获取用户普通优惠
    getUserCommonDiscount (userInfo.m_userId,
                           cycPara.m_cycStartTime,
                           cycPara.m_cycEndTime,
                           userDiscountVector);
    //获取系统级用户优惠信息
    getSysDiscount("04",
    				   cycPara.m_cycStartTime,
                       cycPara.m_cycEndTime,
                       sysDiscountVector);
    for (iter=sysDiscountVector.begin(); iter!=sysDiscountVector.end(); iter++)
    {
        if( iter->m_eparchyCode != userInfo.m_eparchyCode)
        {
            continue;
        }
        idDiscount.m_discountId = iter->m_discountId;
        if (userInfo.m_openDate > iter->m_startDate)
        {
            idDiscount.m_startDate = userInfo.m_openDate;
        }
        else
        {
            idDiscount.m_startDate = iter->m_startDate;
        }
        idDiscount.m_endDate =  iter->m_endDate;

        userDiscountVector.push_back(idDiscount);
    }    

    //根据用户普通优惠获取用户存在帐务优惠的有效优惠信息     
    vector<MonthFeeDiscount>    dVector;    
    for (iter=userDiscountVector.begin();
         iter!=userDiscountVector.end(); iter++)
    {       	
        if ( !getEffDiscntByIdDiscnt(dataType,
        							 *iter, cycPara,
                                     userInfo.m_openDate,
                                     dVector) )
        {
          continue;
        }

        //排重插入
        for (iterd=dVector.begin(); iterd!=dVector.end(); iterd++)
        {
        	//vector<MonthFeeDiscount>::iterator result = 
        	//         discountVector.find(discountVector.begin(),
        	//                             discountVector.end(),*iterd);
        	//                           
            //if (result== discountVector.end())
            //{
            //    break;
            //}
            discountVector.push_back(*iterd);
        }        
    }
    
    //d.排序
    ::partial_sort (discountVector.begin(),discountVector.end(),
                    discountVector.end());
}


void CMdbInfoMgr::getUserSVCState(const USERID& userId,
									 vector<UserSVCState>& userSVCStateVector)
{
    userSVCStateVector.clear();

    UserSVCState							userSVCStat;
    PO_UserSVCStateIncre                    poInc;
    vector<PO_UserSVCStateIncre>            rpoIncsVector;
    vector<PO_UserSVCStateIncre>::iterator  iterInc;

    poInc.m_record.m_userId = userId;
    //增量资料中不存在，从全量资料中获取
    if (m_PO_UserSVCStateIncre.searchMulti(poInc,rpoIncsVector) == 0)
    {
        PO_UserSVCState                         po;
        vector<PO_UserSVCState>                 rposVector;
        vector<PO_UserSVCState>::iterator       iter;

        po.m_record.m_userId = userId;
        m_PO_UserSVCState.searchMulti(po,rposVector);
        for (iter=rposVector.begin(); iter!=rposVector.end(); iter++)
        {
			userSVCStat.m_userId = iter->m_record.m_userId;
			userSVCStat.m_serviceId = iter->m_record.m_serviceId;
			userSVCStat.m_mainTag = iter->m_record.m_mainTag;
			userSVCStat.m_stateCode = iter->m_record.m_stateCode;
			userSVCStat.m_startDate = (char *)iter->m_record.m_startDate;
			userSVCStat.m_endDate = (char *)iter->m_record.m_endDate;

			userSVCStateVector.push_back(userSVCStat);
        }
    }
    //增量资料中存在，从增量资料中获取
    else
    {
        for (iterInc=rpoIncsVector.begin();
             iterInc!=rpoIncsVector.end(); iterInc++)
        {
			userSVCStat.m_userId = iterInc->m_record.m_userId;
			userSVCStat.m_serviceId = iterInc->m_record.m_serviceId;
			userSVCStat.m_mainTag = iterInc->m_record.m_mainTag;
			userSVCStat.m_stateCode = iterInc->m_record.m_stateCode;
			userSVCStat.m_startDate = (char *)iterInc->m_record.m_startDate;
			userSVCStat.m_endDate = (char *)iterInc->m_record.m_endDate;

			userSVCStateVector.push_back(userSVCStat);
        }
    }
}


bool CMdbInfoMgr::getLocalProvinceCode(string& provinceCode)
{
    vector<SysParamInfo>            sysParamInfoVector;

    //获取当前的省份编码
    if ( !getSysParamInfo("AS_ASP_CurrProvinceCode", sysParamInfoVector) )
    {
        return false;
    }
    else
    {
        vector<SysParamInfo>::iterator  iter;
        for (iter = sysParamInfoVector.begin();
             iter != sysParamInfoVector.end(); ++iter)
        {
            m_localProvinceCode = iter->m_valueS;
            break;
        }
    }

    return true;
}




void CMdbInfoMgr::getUserCommonDiscount(const USERID& userId,
                                           const string& cycStartTime,
                                           const string& cycEndTime,
                                        vector<IdDiscount>& userDiscountVector)
{
    userDiscountVector.clear();

    string                                  startDate="";
    string                                  endDate="";
    IdDiscount                              userDiscount;
        
    //1.获取用户普通优惠
    PO_UserDiscountIncre                    poInc;
    vector<PO_UserDiscountIncre>            rpoIncsVector;
    vector<PO_UserDiscountIncre>::iterator  iterInc;

    poInc.m_record.m_userId = userId;
    
    //增量资料中不存在，从全量资料中获取
    if (m_PO_UserDiscountIncre.searchMulti(poInc,rpoIncsVector) == 0)
    {
        PO_UserDiscount                         po;
        vector<PO_UserDiscount>                 rposVector;
        vector<PO_UserDiscount>::iterator       iter;

        po.m_record.m_userId = userId;
        m_PO_UserDiscount.searchMulti(po,rposVector);
        for (iter=rposVector.begin(); iter!=rposVector.end(); iter++)
        {
            startDate = iter->m_record.m_startDate;
            endDate = iter->m_record.m_endDate;
            if ((startDate<cycEndTime)    &&
                (endDate>cycStartTime)    &&
                (startDate<endDate))
            {
                userDiscount.m_discountId = iter->m_record.m_discountId;
                userDiscount.m_startDate = startDate;
                userDiscount.m_endDate = endDate;

                userDiscountVector.push_back(userDiscount);
            }
        }
    }
    //增量资料中存在，从增量资料中获取
    else
    {
        for (iterInc=rpoIncsVector.begin();
             iterInc!=rpoIncsVector.end(); iterInc++)
        {
            startDate = iterInc->m_record.m_startDate;
            endDate = iterInc->m_record.m_endDate;
            if ((startDate<cycEndTime) &&
                (endDate>cycStartTime) &&
                (startDate<endDate))
            {
                userDiscount.m_discountId = iterInc->m_record.m_discountId;
                userDiscount.m_startDate = startDate;
                userDiscount.m_endDate = endDate;

                userDiscountVector.push_back(userDiscount);
            }
        }
    }        
}


void CMdbInfoMgr::getSysDiscount(
                                const string& discntObjectType,
                                const string& cycStartTime,
                                const string& cycEndTime,
                                vector<IdDiscount>& acctDiscountVector)
{
    IdDiscount                          idDiscount;
    vector<PO_SysDiscount>              rposVector;
    vector<PO_SysDiscount>::iterator    iter;

    acctDiscountVector.clear();

    m_PO_SysDiscount.getParamVector(rposVector);
    for (iter=rposVector.begin(); iter!=rposVector.end(); ++iter)
    {
        if (strcmp(iter->m_record.m_bizType.c_str(),discntObjectType) ==0 &&
            iter->m_record.m_startDate < cycEndTime         &&
            iter->m_record.m_endDate > cycStartTime)
        {
            idDiscount.m_discountId = iter->m_record.m_feePolicyId;
            idDiscount.m_startDate = iter->m_record.m_startDate;
            idDiscount.m_endDate = iter->m_record.m_endDate;
            idDiscount.m_eparchyCode = iter->m_record.m_homeAreaCode;

            acctDiscountVector.push_back(idDiscount);
        }
    }
}

bool CMdbInfoMgr::getInfoFromDiscountId(
                                const char& dataType,
                                //Modified by Malone start
                                /*const int& discountId,*/ 
                                const IdDiscount& idDiscnt,
                                //Modified by Malone end 
                                const CycPara& cycPara,   
                                vector<MonthFeeDiscount> & dVector)
{
    MonthFeeDiscount                   monthFeeDiscount;
    vector<PO_SpDiscount>              rposVector;
    vector<PO_SpDiscount>::iterator    iter;

    dVector.clear();
       
    PO_SpDiscount po;    
    po.m_record.m_discntId = idDiscnt.m_discountId;
    
    m_PO_SpDiscount.searchMulti(po,rposVector);    
    
    for (iter=rposVector.begin(); iter!=rposVector.end(); ++iter)
    {   
    	if(iter->m_record.m_dataTypeCode != dataType)
    	{
    		continue;
    	}
    	     
    	if ((iter->m_record.m_startDate<cycPara.m_discountJudgeTime)  &&
            (iter->m_record.m_endDate>=cycPara.m_discountJudgeTime))
    	{    	
    		monthFeeDiscount.m_discountId       =iter->m_record.m_discntId; 
    		monthFeeDiscount.m_orderNo         =iter->m_record.m_orderNo;     
        	monthFeeDiscount.m_priority         =iter->m_record.m_priority;
        	monthFeeDiscount.m_dataTypeCode     =iter->m_record.m_dataTypeCode;
        	monthFeeDiscount.m_spCode           =iter->m_record.m_spCode;
        	monthFeeDiscount.m_spSvcId          =iter->m_record.m_spSvcId;  
        	monthFeeDiscount.m_validType        =iter->m_record.m_validType; 
        	monthFeeDiscount.m_itemCode         =iter->m_record.m_itemCode; 
        	monthFeeDiscount.m_discountType     =iter->m_record.m_discountType; 
        	monthFeeDiscount.m_discountValueType=iter->m_record.m_discountValueType;
        	monthFeeDiscount.m_discountValue    =iter->m_record.m_discountValue; 
        	monthFeeDiscount.m_startCycId      =iter->m_record.m_validStartCycle;
        	monthFeeDiscount.m_endCycId        =iter->m_record.m_validEndCycle; 
        	monthFeeDiscount.m_startDate        =idDiscnt.m_startDate; //取用户的优惠开始时间
        	monthFeeDiscount.m_endDate          =idDiscnt.m_endDate;   //取用户的优惠结束时间     	
        	monthFeeDiscount.m_minValue         =iter->m_record.m_minValue;
        	monthFeeDiscount.m_maxValue         =iter->m_record.m_maxValue;
        	monthFeeDiscount.m_referValue       =iter->m_record.m_referValue;        	
        	
        	dVector.push_back(monthFeeDiscount);             	
        }
    }
    //Modified by Malone end
    
    if(dVector.size()>0)
    	return true;
	else
		return false;   	
    
}


bool CMdbInfoMgr::getEffDiscntByIdDiscnt(const char& dataType,
                                            const IdDiscount& idDiscnt,
                                            const CycPara& cycPara,
                                            const string& openDate,
                                            vector<MonthFeeDiscount> & dVector)
{
	dVector.clear();
	    
    //Modified by Malone start
    /*return getInfoFromDiscountId(dataType,idDiscnt.m_discountId,
    							cycPara,dVector);*/
    return getInfoFromDiscountId(dataType,idDiscnt,
    							cycPara,dVector);
    //Modified by Malone end
    
    
}
                                    
void CMdbInfoMgr::getTariffBySubInfo(
							 const char &dataTypeCode,
                             const UserSubInfo& userSubInfo,
                             const CycPara &cycPara,                             
                             vector<MonfeeTariff>& montfeeTariffVector)
{
    montfeeTariffVector.clear();

    MonfeeTariff                montfeeTariff;
    PO_SpTariff                   po;
    vector<PO_SpTariff>           rposVector;
    vector<PO_SpTariff>::iterator iter;

    po.m_record.m_dataTypeCode = dataTypeCode;
    po.m_record.m_spCode = userSubInfo.m_spCode;    
    po.m_record.m_spBizCode = userSubInfo.m_spBizCode;
    
    m_PO_SpTariff.searchMulti(po,rposVector);

    for (iter=rposVector.begin(); iter!=rposVector.end(); iter++)
    {
        if ((iter->m_record.m_startDate<cycPara.m_discountJudgeTime)  &&
            (iter->m_record.m_endDate>=cycPara.m_discountJudgeTime))
        {         
           montfeeTariff.m_dataTypeCode   =iter->m_record.m_dataTypeCode   ; 
           montfeeTariff.m_spCode         =iter->m_record.m_spCode         ;
           montfeeTariff.m_spSvcId        =iter->m_record.m_spSvcId        ;
           montfeeTariff.m_bizTypeCode    =iter->m_record.m_bizTypeCode    ;//zhaogc add 20070717
           montfeeTariff.m_recvObjectType =iter->m_record.m_recvObjectType ;
           montfeeTariff.m_itemCode       =iter->m_record.m_itemCode       ;
           montfeeTariff.m_billingCycle   =iter->m_record.m_billingCycle   ;
           montfeeTariff.m_relativeCycle  =iter->m_record.m_relativeCycle  ;
           montfeeTariff.m_price          =iter->m_record.m_price          ;
           montfeeTariff.m_newUserRecvType=iter->m_record.m_newUserRecvType;
           montfeeTariff.m_judgeHalfTime  =iter->m_record.m_judgeHalfTime  ;
           montfeeTariff.m_halfPrice      =iter->m_record.m_halfPrice      ;
           montfeeTariff.m_displayType    =iter->m_record.m_displayType    ;
           montfeeTariff.m_validStartCycle=iter->m_record.m_validStartCycle;
           montfeeTariff.m_validEndCycle  =iter->m_record.m_validEndCycle  ;
           montfeeTariff.m_startDate      =iter->m_record.m_startDate;
           montfeeTariff.m_endDate        =iter->m_record.m_endDate;
           
           // Add by Malone 2008-1-26 start
           montfeeTariff.m_dumbnessDeal        =iter->m_record.m_dumbnessDeal;
           montfeeTariff.m_averageDeal        =iter->m_record.m_averageDeal;
           // Add by Malone 2008-1-26 end

           montfeeTariffVector.push_back(montfeeTariff);
        }
    }

    //对优惠明细信息集进行排序
    ::partial_sort (montfeeTariffVector.begin(),
                    montfeeTariffVector.end(), montfeeTariffVector.end());
}

void CMdbInfoMgr::getSubInfoByDiscnt(
                       const MonthFeeDiscount& monthFeeDiscnt,
                       const CycPara &cycPara,                             
                       vector<UserSubInfo>& userAllSubInfoVector,
                       vector<UserSubInfoIter>& userSubInfoVector)
{
    userSubInfoVector.clear();
    
    vector<UserSubInfo>::iterator iter =userAllSubInfoVector.begin();
    for (;iter!=userAllSubInfoVector.end(); iter++)
    {
        if ((monthFeeDiscnt.m_spBizCode == iter->m_spBizCode)  &&
            (monthFeeDiscnt.m_spCode == iter->m_spCode) )
        {
           userSubInfoVector.push_back(iter);
        }
    }    
}                                         

bool CMdbInfoMgr::judgeNotOpen(const string& startDate,
                              const string& endDate,
                              const vector<UserSVCState>& userSVCStateVector)
{    
    bool                            isValid = true;
    
    vector<UserSVCState>::const_iterator  iters;
    for (iters=userSVCStateVector.begin();
         iters!=userSVCStateVector.end(); iters++)
    {
        if (iters->m_mainTag=='1'           && 
            ( (iters->m_stateCode=='0') ||
              (iters->m_stateCode=='N')   ) &&  
            iters->m_startDate<endDate      &&
            iters->m_endDate>startDate      &&
            iters->m_startDate<iters->m_endDate)
        {
           isValid = false;
           break;
        }
    }   
    
    return isValid;
}

void CMdbInfoMgr::getUserNum(const USERID& ecUserId, 
                           			const String<18>& spBizCode,                           			
                           			int &usernum)
{
    usernum = 0;     
        
    PO_AdcECuserNum po;
    vector<PO_AdcECuserNum> rposVector;
    vector<PO_AdcECuserNum>::iterator       iter;
    
    po.m_record.m_ecUserId = ecUserId;
    
    // Modified by Malone start
    /*m_PO_AdcECuserNum.searchMulti(po,rposVector) == 0;*/
    m_PO_AdcECuserNum.searchMulti(po,rposVector);
    // Modified by Malone end
    
    for (iter=rposVector.begin(); iter!=rposVector.end(); iter++)
    {   
	    if( iter->m_record.m_bizCode == spBizCode )
		{
			usernum = iter->m_record.m_userNum;
			
			break;
		}			
    }
}

bool CMdbInfoMgr::getUserState(const USERID& ecSerialNumber, 
                           			  const String<18>& spBizCode)                           			  
{        
    PO_AdcECuserState po;
    vector<PO_AdcECuserState> rposVector;
    vector<PO_AdcECuserState>::iterator       iter;
    
    po.m_record.m_ecSerialNumber = ecSerialNumber;    
    m_PO_AdcECuserState.searchMulti(po,rposVector);
        
    for (iter=rposVector.begin(); iter!=rposVector.end(); iter++)
    {   
	    if( iter->m_record.m_bizCode == spBizCode )
		{
			return false;
		}			
    }
    return true;
}

bool CMdbInfoMgr::judgeDumbnessUser(const int& cycId,
	                                      const string& serialNumber, 
	                                      const String<18>& spCode,
	                                      const String<18>& spBizCode,
	                                      const MonfeeTariff& monfeeTariff)
{
//  PO_SpDumbness                         po;
//  PO_SpDumbness                         rpo;
//	
//	po.m_record.m_serialNumber = serialNumber;
//	po.m_record.m_spCode = spCode;
//	po.m_record.m_spSvcId = spServCode;
//	
//	//结算提供的是非沉默用户和单高用户，所以查询未命中的一定是沉默用户；
//	//查询命中的为非沉默用户，但是有可能是单高用户。
//	
//	if ( (dumbnessDeal == '0') && (averageDeal != '0') ) //沉默用户减免，单高用户不减免
//	{
//		if ( m_PO_SpDumbness.search(po,rpo) == 0 )
//		{
//      	return true;
//  	}	
//  	else
//  	{
//  		return false;
//  	}
//	}
//	else if ( (dumbnessDeal != '0') && (averageDeal == '0') ) //沉默用户不减免，单高用户减免
//	{
//		if ( m_PO_SpDumbness.search(po,rpo) == 0 )
//		{
//      	return false;
//  	}
//  	else
//  	{
//  		if ( rpo.m_record.m_dumbnessTag == SINGLE_HIGH_USER )
//  		{
//  			return true;
//  		}
//  		else
//  		{
//  			return false;
//  		}
//  	}
//	}
//	else if ( (dumbnessDeal == '0') && (averageDeal == '0') ) //沉默用户减免，单高用户减免
//	{
//		if ( m_PO_SpDumbness.search(po,rpo) == 0 )
//		{
//      	return true;
//  	}	
//  	else
//  	{
//  		if ( rpo.m_record.m_dumbnessTag == SINGLE_HIGH_USER )
//  		{
//  			return true;
//  		}
//  		else
//  		{
//  			return false;
//  		}
//  	}
//	}
	bool dumbnessTag,averageTag,findTag;
	dumbnessTag = true;
	averageTag = true;
	findTag = false;
	int	average = 0;
	
	PO_SpDumbness                         po;
	PO_SpDumbness                         rpo;
	
	
	po.m_record.m_cycId = cycId;
	po.m_record.m_serialNumber = serialNumber;
	po.m_record.m_spCode = spCode;
	po.m_record.m_spSvcId = spServCode;
		
	if (monfeeTariff.m_judgeDumbness == '1')
	{
		findTag = true;
		if ( m_PO_SpDumbness.search(po,rpo) == 0 )
			dumbnessTag = true;
		else
			dumbnessTag = false;
	}
	
	if (monfeeTariff.m_judgeAverage == '1')
	{
		if (!findTag)
		{
			if ( m_PO_SpDumbness.search(po,rpo) == 0 )
			{
				averageTag = true;
	  	}
	  	else
	  	{
	  		average = rpo.m_record.m_dumbnessTag * 2000;
	  		if ( monfeeTariff.m_price > average )
	  		{
					averageTag = true;
	  		}
	  		else
	  		{
	  			averageTag = false;
	  		}
	  	}
	  }
	  else
	  {
	  	average = rpo.m_record.m_dumbnessTag * 2000;
	  	if ( monfeeTariff.m_price > average )
  		{
				averageTag = true;
  		}
  		else
  		{
  			averageTag = false;
  		}
	  }
	}
	
	return (dumbnessTag && averageTag);
}

bool CMdbInfoMgr::getSpPunishRule(const CycPara& cycPara,
                                         SpPunishMap& spPunishMap)
{
   	spPunishMap.clear();
   	
   	SpSvcKey spSvcKey;
   	vector<PO_SpPunish> rposVector;
   	vector<PO_SpPunish>::iterator       iter;
   	m_PO_SpPunish.getParamVector(rposVector);    
        
    int dayNum;
    string startTime,endTime;
    for (iter=rposVector.begin(); iter!=rposVector.end(); ++iter)
    {   
        startTime = (*iter).m_record.m_beginTime;
        endTime = (*iter).m_record.m_endTime;
        
        if(startTime<cycPara.m_cycStartTime)
        {
            startTime = cycPara.m_cycStartTime; 	
        }
        
        if(endTime>cycPara.m_cycEndTime)
        {
            endTime = cycPara.m_cycEndTime; 	
        }
        getDuration(endTime, startTime,dayNum);
        
        if(dayNum>10)
        {
	          spSvcKey.m_spCode = (*iter).m_record.m_spCode;
	          if((*iter).m_record.m_isAllSp == '1')
	          {
	              spSvcKey.m_spBizCode = "0";
	          }
	          else
	          {
	              spSvcKey.m_spBizCode = (*iter).m_record.m_spBizCode;
	          }
	          spPunishMap.insert(make_pair(spSvcKey,'1'));
	      }
					
    }
    return true;   	
}

//  Add by yih 2008-08-06 start
bool CMdbInfoMgr::getAllRelaDiscount(vector<SpRelaDiscnt> &spRelaDiscntVector)
{
    SpRelaDiscnt spRelaDiscnt;
    spRelaDiscnt.m_spRelaDiscntDetailVector.clear();
    spRelaDiscntVector.clear();
    
    PO_SpRelaDiscnt po;
    vector<PO_SpRelaDiscnt> rposVector;
    vector<PO_SpRelaDiscnt>::iterator       iter;
    	
    rposVector.clear();
    
    if (!m_PO_SpRelaDiscnt.getParamVector(rposVector))
    {
        //获取SP关联优惠集失败
        return false;
    }
    
    iter = rposVector.begin();
    for(; iter != rposVector.end(); iter++)
    {
        spRelaDiscnt.m_discntId = (*iter).m_record.m_discntId;
        spRelaDiscnt.m_effectValueType = (*iter).m_record.m_effectValueType;
        spRelaDiscnt.m_effectValue = (*iter).m_record.m_effectValue;
        spRelaDiscnt.m_startDate = (*iter).m_record.m_startDate;
        spRelaDiscnt.m_endDate = (*iter).m_record.m_endDate;
        
        PO_SpRelaDiscntDetail spo;
        vector<PO_SpRelaDiscntDetail> rsposVector;
        vector<PO_SpRelaDiscntDetail>::iterator       iterS;

        spo.m_record.m_discntId = (*iter).m_record.m_discntId;
        m_PO_SpRelaDiscntDetail.searchMulti(spo,rsposVector);
        
        iterS = rsposVector.begin();
        for(; iterS != rsposVector.end(); iterS++)
        {
            SpRelaDiscntDetail spRelaDiscntDetail;
            spRelaDiscntDetail.m_priority = iterS->m_record.m_priority;
            spRelaDiscntDetail.m_itemCode = iterS->m_record.m_itemCode;
            spRelaDiscntDetail.m_spCode = iterS->m_record.m_spCode;
            spRelaDiscntDetail.m_spSvcCode = iterS->m_record.m_spSvcCode;
           
            spRelaDiscnt.m_spRelaDiscntDetailVector.push_back(spRelaDiscntDetail);
        }
        
        spRelaDiscntVector.push_back(spRelaDiscnt);
        spRelaDiscnt.m_spRelaDiscntDetailVector.clear();
    }
    
    return true;

}
//  Add by yih 2008-08-06 end

void CMdbInfoMgr::getDuration(const string& endDate, 
                                     const string& startDate,
                                     int& dayNum)
{
    time_t endsecond,startsecond;  
    int    endyear,endmonth,endday,endhour,endminute,endseconds;
    int    startyear,startmonth,startday;
    struct tm p_end;
    struct tm p_start;
    
    endyear = atoi(endDate.substr(0,4).c_str());
    endmonth = atoi(endDate.substr(4,2).c_str());
    endday = atoi(endDate.substr(6,2).c_str());
    endhour = atoi(endDate.substr(8,2).c_str());
    endminute = atoi(endDate.substr(10,2).c_str());
    endseconds = atoi(endDate.substr(12,2).c_str());
    
    startyear = atoi(startDate.substr(0,4).c_str());
    startmonth = atoi(startDate.substr(4,2).c_str());
    startday = atoi(startDate.substr(6,2).c_str());  
    
    p_end.tm_sec = 0;
    p_end.tm_min = 0;
    p_end.tm_hour = 0;
    p_end.tm_mday = endday;
    p_end.tm_mon = endmonth - 1;
    p_end.tm_year = endyear - 1900;
    
    endsecond = mktime(&p_end);
    
    p_start.tm_sec = 0;
    p_start.tm_min = 0;
    p_start.tm_hour = 0;
    p_start.tm_mday = startday;
    p_start.tm_mon = startmonth - 1;
    p_start.tm_year = startyear - 1900;  
    
    startsecond = mktime(&p_start);
    
    if (endhour==0 && endm;
    p_end.tm_year = endyear - 1900;
    
    endsecond = mktime(&p_end);
    
    p_start.tm_sec = 0;
    p_start.tm_min = 0;
    p_start.tm_hour = 0;
    p_start.tm_mday = startda
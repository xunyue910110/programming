#include "RentProc.h"
#include "RateGlobalInfo.h"
//add for rent new version begin
#include "appfrm/Application.h"
#include "appfrm/ErrorHandle.h"
#include "base/TimeUtil.h"
//add for rent new version end
//add by xuf begin 2009-8-20 21:59:17
#include "CondBase.h"
//add by xuf end 2009-8-20 21:59:17

INSTALL_PROCEDURE_INSTANCE(RentProc);


bool RentProc::initialize()
{
  if(Procedure::initialize()==false)
  {
    return false;
  }
  return true;
}


int RentProc::execute()
{
    #ifdef FP_DEBUG
    FP_BEGIN(RentProc)
    #endif
    //1. 设置一些RateGlobalInfo中的值
    //  用户级
    //根据文件明取帐期
    //根据帐期取帐期的开始时间、半月时间、终止时间
    m_pcdr[ATTRI_NEED_BASETP]  = FLAG_NO ;
    /*
    以下这些结构体赋值都可以去掉了，前面的U文件在RateGlobalInfo::canonicalize()都是已经赋值过的
    //int t_cdrCycle = (int)(m_pRateInfo->m_fileName.substr(6,6));
    //m_pcdr[ATTRI_USER_ID]      = m_pRateInfo->m_billUserId;
    //m_pRateInfo->m_userId      = m_pRateInfo->m_billUserId;
    m_pcdr[ATTRI_DEAL_TIME]    =(m_pRateInfo->m_dealTime)+4;
    */
    //结构体清空
    m_pRateInfo->m_balAcctIdList.clear();
    m_pRateInfo->m_discountBills.clear();
    m_pRateInfo->m_rentFeeLogInfoVector.clear();
//  m_pRateInfo->m_addBillVector.clear(); //RateGlobalInfo 统一清，防止清前面过程产生的异常信息 add by xuf 2009-10-31 19:51:08

    if(m_pRateInfo->m_paramMgr->m_isCallRentProc==false)
    {
        //无需计算固定费，直接返回
        #ifdef FP_DEBUG
        FP_END(RentProc);
        #endif
        return 0;
    }
    
    
    //add for rent item auto clear begin 20091107 by liugj
    vector<UserBill>::iterator itrUserbill, itrUserbillEnd;
    itrUserbillEnd = m_pRateInfo->m_userBills_new.end();
    
    for(itrUserbill = m_pRateInfo->m_userBills_new.begin();
        itrUserbill != itrUserbillEnd; ++itrUserbill)
    {
        map<int, int>::iterator itrMapRentItem;
        itrMapRentItem = 
        m_pRateInfo->m_paramMgr->m_rentItemMap.find(itrUserbill->m_billId);
        //如果是固定费帐目
        if(itrMapRentItem!=m_pRateInfo->m_paramMgr->m_rentItemMap.end())
        {
            //清零 
            itrUserbill->m_value       = 0;
            itrUserbill->m_discntValue = 0;
            itrUserbill->m_adjustBefore = 0;//add for rentfeeItem 2011-3-23            
        }
    }
    //add for rent item auto clear end 20091107 by liugj
//erased by xuf 20091120 AcctAcctibProc 已经初始化
//    m_pRateInfo->m_comBill.m_cycParam = m_pRateInfo->m_cycParam;
//    m_pRateInfo->m_comBill.m_userId   = m_pRateInfo->m_billUserId;
    //确定用户信息

//erase by xuf begin
//    FP_BEGIN(getUserInfoProc_m_comBill)
//    //if(m_pRateInfo->m_paramMgr->getUserInfo(m_pRateInfo->m_userId, m_pRateInfo->m_idInfo)!=1)
//    if(!m_pRateInfo->m_comBill.getUserInfo(m_pRateInfo->m_userId, m_pRateInfo->m_idInfo))//zhaogc modified 20090627
//    {
//        DEBUG_PRINT("[%s:%d]用户[%s]getUserInfo失败!\n",__FILE__,__LINE__,m_pRateInfo->m_userId.c_str());
//        registerErrorUser(m_pRateInfo->m_userId, E_RENTFEE_GETUSERINFO_ERR, "0");
//        return -1;
//    }
//    FP_END(getUserInfoProc_m_comBill)
    
    m_pRateInfo->m_needIdInfo = false;

    //是无需计算固定费的用户
    if(!isNeedCalcRent())
    {
        #ifdef FP_DEBUG
        FP_END(RentProc);
        #endif
        return 0;
    }

    /*有些东西可去掉，存疑。。。
    //确定帐期
    if(!m_pRateInfo->m_paramMgr->getCycleTime(t_cdrCycle, m_pRateInfo->m_cycParam))
    {
        //m_pRateInfo->m_rentOutputCdrList.clear();
        registerErrorUser(m_pRateInfo->m_userId, E_RENTFEE_GETCYCINFO_ERR, "0");
      #ifdef _DEBUG_
        cout << "!m_pRateInfo->m_paramMgr->getCycleTime("
             << t_cdrCycle << ") false!"
             << __FILE__<<__LINE__<<endl;
      #endif
      #ifdef FP_DEBUG
      FP_END(RentProc);
      #endif
      return -1;
    }
    //设置CDR帐期
    m_pcdr[ATTRI_CYCLE_DATE] = m_pRateInfo->m_cycParam.m_cycleId;
    //第一次开打时间
    m_pRateInfo->m_firstCallTime = (m_pRateInfo->m_idInfo).m_idAttr[IdInfo::ID_FIRSTCALL_TIME];

    //通话开始时间、结束时间、帐期
    if((m_pRateInfo->m_idInfo).m_idAttr[IdInfo::ID_OPEN_DATE]>
       (m_pRateInfo->m_cycParam).m_beginTime)//如果是新开户用户
    {
      //设置新开户时间为通话开始时间
      m_pcdr[ATTRI_BEGIN_DATE] = (m_pRateInfo->m_idInfo).m_idAttr[IdInfo::ID_OPEN_DATE].substr(0, 8);
      m_pcdr[ATTRI_BEGIN_TIME] = (m_pRateInfo->m_idInfo).m_idAttr[IdInfo::ID_OPEN_DATE].substr(8, 6);
      m_pRateInfo->m_beginDateTime = (m_pRateInfo->m_idInfo).m_idAttr[IdInfo::ID_OPEN_DATE]; //通话起始时间
    }else//不是新开户
    {
      //设置月初开始时间为通话开始时间
      m_pcdr[ATTRI_BEGIN_DATE] = (m_pRateInfo->m_cycParam).m_beginTime.substr(0, 8);
      m_pcdr[ATTRI_BEGIN_TIME] = (m_pRateInfo->m_cycParam).m_beginTime.substr(8, 6);
    //m_pRateInfo->m_beginDateTime = (m_pRateInfo->m_idInfo).m_idAttr[IdInfo::ID_OPEN_DATE]; //通话起始时间
    //m_pRateInfo->m_beginDateTime = (m_pRateInfo->m_cycParam).m_beginTime;//zhaogc delete 20090706
    m_pRateInfo->m_beginDateTime = (m_pRateInfo->m_cycParam).m_endTime;    //zhaogc add 20090706
    }
    m_pRateInfo->m_endDateTime = (m_pRateInfo->m_cycParam).m_endTime;   //通话结束时间设置成月末时间


    */

    if(!calcTotalCycRent())
    {
        #ifdef FP_DEBUG
        FP_END(RentProc);
        #endif
        return -1;
    }
    #ifdef FP_DEBUG
    FP_END(RentProc);
    #endif
    return 0;
}


void RentProc::registerErrorUser(const T_USER_ID &r_id,
                         const int r_errCode,
                         const T_FLAG &r_operTypeTag)
{
  AdditionalBill   l_idErrorInfo;
  l_idErrorInfo.m_billType    = "0";            //"0"表示在其他帐单中为错误用户帐单
  l_idErrorInfo.m_id          = r_id;           //用户ID
  l_idErrorInfo.m_idType      = USER_ID_TYPE;   // FieldsXX.h中枚举enum ID_TYPE类型，表示类型为"用户",区别于账户、客户
  l_idErrorInfo.m_operateType = r_operTypeTag;  //为"0"表示未处理,"1"表示已处理
  l_idErrorInfo.m_itemCode       = r_errCode;      //错误编码
  l_idErrorInfo.m_fValue      = 0;              //保留
  l_idErrorInfo.m_sValue      = 0;              //保留

  m_pRateInfo->m_addBillVector.push_back(l_idErrorInfo);
}
/*
bool RentProc::getStartTimeOfServ(const int svc, T_DATE_TIME& r_startTime)
{
    long t_startDay,t_endDay;        //当前帐期开始时间结束时间
    long t_servBeginDay,t_servEndDay; //服务开始时间、结束时间
    bool hasFound = false;

    t_startDay = m_pRateInfo->m_cycParam.m_beginTime.value();
    t_endDay = m_pRateInfo->m_cycParam.m_endTime.value();

    vector<UserServ>::const_iterator citr_end = m_pRateInfo->m_vecUserServ.end();
    for(vector<UserServ>::const_iterator itr = m_pRateInfo->m_vecUserServ.begin();
        itr!=citr_end; ++itr)
    {
        t_servBeginDay = itr->m_startDate.value();
        t_servEndDay   = itr->m_endDate.value();
        if(t_servBeginDay>=t_servEndDay)
            continue;
        if(t_servBeginDay <= t_endDay && t_servEndDay > t_startDay)//如果是本帐期内的服务，用'<='t_endDay，因为是从帐期表获取的，是23:59:59，不是从王孙的数组中获取的
        {
            if(itr->m_servId==svc)
            {
                if(hasFound==false)//首次
                {
                    r_startTime = t_servBeginDay;
                    hasFound = true;
                }
                else//不是首次
                {
                    if(r_startTime > t_servBeginDay)//有更久远的时间
                    {
                        r_startTime = t_servBeginDay;
                    }
                }
            }
        }
    }

    return hasFound;
}


bool RentProc::getStartTimeOfServState(const int svc, const char svcState, T_DATE_TIME& r_startTime)
{
    long t_startDay,t_endDay;        //当前帐期开始时间结束时间
    long t_servStateBeginDay,t_servStateEndDay; //服务状态开始时间、结束时间
    bool hasFound = false;

    t_startDay = m_pRateInfo->m_cycParam.m_beginTime.value();
    t_endDay = m_pRateInfo->m_cycParam.m_endTime.value();

    vector<UserServState>::const_iterator citr_end = m_pRateInfo->m_vecUserServState.end();
    for(vector<UserServState>::const_iterator itr = m_pRateInfo->m_vecUserServState.begin();
        itr!=citr_end; ++itr)
    {
        t_servStateBeginDay = itr->m_startDate.value();
        t_servStateEndDay   = itr->m_endDate.value();
        if(t_servStateBeginDay>=t_servStateEndDay)
            continue;
        if(t_servStateBeginDay <= t_endDay && t_servStateEndDay > t_startDay)//如果是本帐期内的服务状态，用'<='t_endDay，因为是从帐期表获取的，是23:59:59，不是从王孙的数组中获取的
        {
            //是相应服务已经相应状态对应的记录
            if((itr->m_serviceId==svc)&&(itr->m_servStateCode==svcState))
            {
                if(hasFound==false)//首次
                {
                    r_startTime = t_servStateBeginDay;
                    hasFound = true;
                }
                else//不是首次
                {
                    if(r_startTime > t_servStateBeginDay)//有更久远的时间
                    {
                        r_startTime = t_servStateBeginDay;
                    }
                }
            }
        }

    }

    return hasFound;
}
*/

int RentProc::getUserRentEvent(vector<int>& userRentEventIdxVec)
{
    userRentEventIdxVec.clear();

    vector<RentEvent>::iterator itrEvent, itrEventEnd;
    itrEventEnd = m_pRateInfo->m_paramMgr->m_vecRentEvent.end();

    int i = -1;//计数器
    //逐个事件循环，判断是否有相应的服务，以及可以通过条件判断
    for(itrEvent=m_pRateInfo->m_paramMgr->m_vecRentEvent.begin();
        itrEvent!=itrEventEnd; ++itrEvent)
    {
        ++i;

        //判断是否可以通过条件判断
        if(itrEvent->m_condRule!=NULL)
        {
            try
            {
                if(!itrEvent->m_condRule->execute())
                {
                    //条件不通过，跳过
                    continue;
                }
            }catch (...)
            {
                DEBUG_PRINT("[%s:%d]用户[%s]事件%ld不通过，因为执行条件%d!异常\n",
                            __FILE__,__LINE__,m_pRateInfo->m_userId.c_str(),
                            itrEvent->m_eventCode.value(), itrEvent->m_condId);
            }
        }

        UserServ userServ;
        //判断是否有某个服务
        if(!hasUserServ(m_pRateInfo->m_cycParam.m_beginTime,
                        m_pRateInfo->m_cycParam.m_endTime,
                        itrEvent->m_serviceId, userServ))
        {
            //没有该服务，跳过
            continue;
        }
        /*青海需求，最后一天开通的服务不算作有效服务
        if(userServ.m_startDate.substr(0,8)==m_pRateInfo->m_cycParam.m_endTime.substr(0,8))
        {
            continue;
        }
        */
        if(m_pRateInfo->m_mtmapRentTpList.find(itrEvent->m_eventCode)
            ==m_pRateInfo->m_mtmapRentTpList.end())
        {
            continue;
        }

        userRentEventIdxVec.push_back(i);
    }

    return userRentEventIdxVec.size();

}


//是否需要计算固定费
bool RentProc::isNeedCalcRent()
{	  
	  //用户不需要计算固定费
   // CondBase *l_pCondBase = m_pRateInfo->m_paramMgr->getCondition(m_pRateInfo->m_paramMgr->m_isNeedCalcRentCondId);
    //if(l_pCondBase!=NULL)
   // {
   // 	if(l_pCondBase->execute())
    //		return false;
    //}
       
    //网别判断（14\15智能网OCS不需要）
    if((m_pRateInfo->m_idInfo.m_idAttr[IdInfo::ID_NETTYPE_CODE]=="14")
     ||(m_pRateInfo->m_idInfo.m_idAttr[IdInfo::ID_NETTYPE_CODE]=="15"))
    {
    		return false;
    }
    
     //add by xuf 2009-12-29 19:25:41
     if (m_pmManager->m_pCondBaseCalcRent!=NULL)
    {
     	 if (m_pmManager->m_pCondBaseCalcRent->execute())
        return false;
    }

    //remove_tag判断
    /*
    if(m_pRateInfo->m_idInfo.m_idAttr[IdInfo::ID_REMOVE_TAG]!="0")
    {
        if((m_pRateInfo->m_idInfo.m_idAttr[IdInfo::ID_PREDESTROY_TIME]<m_cycPara.)
          ||(m_pRateInfo->m_idInfo.m_idAttr[IdInfo::]<m_cycPara)
          )
        {
            return false;
        }
    }
    */

    return true;
}


bool RentProc::calcTotalCycRent()
{

    //获取用户服务
    #ifdef FP_DEBUG
    FP_BEGIN(getUserServProc)
    #endif
    
    //如前面已取则不必重复取 add by xuf 2009-11-17 4:40:08
    if (!m_pRateInfo->m_isExistServ&&m_pRateInfo->m_paramMgr->getUserServ(m_pRateInfo->m_userId, m_pRateInfo->m_vecUserServ)<=0)
    {
       //用户服务有问题
       DEBUG_PRINT("[%s:%d]用户[%s]getUserServ失败!\n",__FILE__,__LINE__,m_pRateInfo->m_userId.c_str());
       registerErrorUser(m_pRateInfo->m_userId,E_RENTFEE_GET_USERSERV,"0");
       #ifdef FP_DEBUG
       FP_END(getUserServProc);
       #endif
       return false;
    }
    #ifdef FP_DEBUG
    FP_END(getUserServProc);
    #endif

    //根据用户服务获取用户事件
    if(getUserRentEvent(m_userRentEventIdxVec)==0)
    {
        //没有获取到需要计算的事件，无需计算
        return true;
    }

    //获取用户状态
    #ifdef FP_DEBUG
    FP_BEGIN(getUserServStateProc)
    #endif
    //如前面已取则不必重复取 add by xuf 2009-11-17 4:40:08
    if (!m_pRateInfo->m_isExistServState&&m_pRateInfo->m_paramMgr->getUserServState(m_pRateInfo->m_userId, m_pRateInfo->m_vecUserServState)<=0)
    {
        //用户状态资料有问题
        DEBUG_PRINT("[%s:%d]用户[%s]getUserServState失败!\n",__FILE__,__LINE__,m_pRateInfo->m_userId.c_str());
        registerErrorUser(m_pRateInfo->m_userId,E_RENTFEE_GET_USERSERVSTATE,"0");
        #ifdef FP_DEBUG
        FP_END(getUserServStateProc);
        #endif
        return false;
    }
    #ifdef FP_DEBUG
    FP_END(getUserServStateProc);
    #endif
    //m_pRateInfo->m_comBill.m_userServStateVector = m_pRateInfo->m_vecUserServState;
    //移动版本
    m_pRateInfo->m_needServState = false;

    /*
    //获取用户有效性信息(天津移动专用)
    #ifdef FP_DEBUG
    FP_BEGIN(getUserValidInfo)
    #endif
    if (m_pRateInfo->m_paramMgr->getUserValidInfo(m_pRateInfo->m_userId, m_pRateInfo->m_)<=0)
    {
        //用户有效性资料有问题
        DEBUG_PRINT("[%s:%d]用户[%s]有效性资料不存在!\n",__FILE__,__LINE__,m_pRateInfo->m_userId.c_str());
        registerErrorUser(m_pRateInfo->m_userId,E_RENTFEE_GET_USER,"0");
        #ifdef FP_DEBUG
        FP_END(getUserValidInfo);
        #endif
        return -1;
    }
    #ifdef FP_DEBUG
    FP_END(getUserValidInfo);
    #endif
    */

    map<T_SUBCYC_TYPE, int> t_startSubCycIdMap, t_endSubCycIdMap;
    T_DATE_DAY t_startDay, t_endDay;

    #ifdef FP_DEBUG
    FP_BEGIN(getCalcDateInfoproc)
    #endif
    if(!getCalcDateInfo(t_startDay, t_endDay,
                    t_startSubCycIdMap, t_endSubCycIdMap))
    {
        //没有获取到计算的开始、结束时间
        #ifdef FP_DEBUG
        FP_END(getCalcDateInfoproc);
        #endif
        return false;
    }
    #ifdef FP_DEBUG
    FP_END(getCalcDateInfoproc);
    #endif

    //记录固定费计算明细日志
    UserRentPriceLog t_rentpricelog;

    //获取日志信息
    t_rentpricelog.m_userid = m_pRateInfo->m_userId;

    //初始化固定费帐单
    //m_userRentBills.clear();
    m_rentBillInfoVec.clear();//modified for rentfeeItem 2011-3-23
    
    vector<int>::iterator itrUserRentEvent, itrUserRentEventEnd;
    itrUserRentEventEnd = m_userRentEventIdxVec.end();
    for(itrUserRentEvent = m_userRentEventIdxVec.begin();
        itrUserRentEvent != itrUserRentEventEnd; ++itrUserRentEvent)
    {//对用户已有事件的轮询

        //用户事件在PM_Rate::m_vecRentEvent的下标序列号
        int t_iIdx = *itrUserRentEvent;

        //获取该子帐期类型的开始序列（第几个子帐期）
        map<T_SUBCYC_TYPE, int>::iterator itrSubCycStartDay;
        if( (itrSubCycStartDay=t_startSubCycIdMap.find(
             m_pRateInfo->m_paramMgr->m_vecRentEvent[t_iIdx].m_subCycType))==t_startSubCycIdMap.end())
        {
            //未找到该子帐期类型的失效子帐期序列
            cout << "error m_eventCode:"<<m_pRateInfo->m_paramMgr->m_vecRentEvent[t_iIdx].m_eventCode <<
                    "error m_subCycType:"<<m_pRateInfo->m_paramMgr->m_vecRentEvent[t_iIdx].m_subCycType <<endl;
            
            return false;
        }
        //获取该子帐期类型的结束序列（第几个子帐期）
        map<T_SUBCYC_TYPE, int>::iterator itrSubCycEndDay;
        if( (itrSubCycEndDay=t_endSubCycIdMap.find(
             m_pRateInfo->m_paramMgr->m_vecRentEvent[t_iIdx].m_subCycType))==t_endSubCycIdMap.end())
        {
            //未找到该子帐期类型的失效子帐期序列
            return false;
        }

        int calcSubCycId, startSubCycId, endSubCycId, t_minSubId, t_maxSubId;
        startSubCycId = itrSubCycStartDay->second;
        endSubCycId   = itrSubCycEndDay->second;
        //获取该类型最小帐期号和最大帐期号
        if(!m_pRateInfo->m_paramMgr->getMinMaxSubIdFromItem(m_pRateInfo->m_paramMgr->m_vecRentEvent[t_iIdx].m_eventCode,
                  m_pRateInfo->m_cycParam.m_cycleId, t_minSubId, t_maxSubId))
        {
        	DEBUG_PRINT("[%s:%d]用户[%s]getMinMaxSubIdFromItem失败!\n",__FILE__,__LINE__,m_pRateInfo->m_billUserId.c_str());
        	registerErrorUser(m_pRateInfo->m_billUserId, E_RENTFEE_GET_GETEVENTFROMITEM, "0");
        	continue; //异常处理
        }          
                 
        //先将CDR数组初始化与清空
        //开始计算子帐期
        m_pcdr[ATTRI_RENT_STARTSUBID]= startSubCycId;
        //结束计算子帐期
        m_pcdr[ATTRI_RENT_ENDSUBID]  = endSubCycId;
        //最小子帐期
        m_pcdr[ATTRI_RENT_CURRENT_MINSUBID]  = t_minSubId;
        //最大子帐期
        m_pcdr[ATTRI_RENT_CURRENT_MAXSUBID]  = t_maxSubId;
        //原来的明细帐目编码，现在的固定费事件编码
        m_pcdr[ATTRI_RENT_ITEMCODE]  = m_pRateInfo->m_paramMgr->m_vecRentEvent[t_iIdx].m_eventCode;
        m_pcdr[ATTRI_BIZ_TYPE]= m_pRateInfo->m_paramMgr->m_vecRentEvent[t_iIdx].m_eventCode;//add for rentfeeItem 2011-3-23        
        //事件的子帐期类型编码
        m_pcdr[ATTRI_RENT_SUBCYCLETYPE] = m_pRateInfo->m_paramMgr->m_vecRentEvent[t_iIdx].m_subCycType;
        //计算日期(8位日期)用于显示费用调整
        m_pcdr[ATTRI_RENT_COMPUTERDATE] = t_endDay;
        /*暂时不需要了
        //计算日期所在子帐期（不一定是sysdate）
        t_computeSubId = m_pRateInfo->m_paramMgr->getSubCycIdFromDay(
                       (int)(t_endDay.c_str()), t_itemCode);
        m_pcdr[ATTRI_RENT_CURRENT_COMPSUBID] = t_computeSubId;*/

        m_pcdr[ATTRI_RENT_SUMFEE]     = 0;//sumfee
        m_pcdr[ATTRI_RENT_DISPLAYFEE] = 0;//displayfee
        for(calcSubCycId = 1; calcSubCycId <= 31; ++calcSubCycId)
        {
            m_pcdr[ ATTRI_RENT_FEE_BASE +calcSubCycId] = -1;   //未知金额
            m_pcdr[ ATTRI_RENT_SVC_BASE +calcSubCycId] = -2;   //未知服务
            m_pcdr[ATTRI_RENT_STATE_BASE+calcSubCycId] = "*";  //未知状态
        }

        //初始化日志信息
        t_rentpricelog.m_item =
             atoi(m_pRateInfo->m_paramMgr->m_vecRentEvent[t_iIdx].m_eventCode.c_str());

        for(calcSubCycId = startSubCycId; calcSubCycId <= endSubCycId; ++calcSubCycId)
        {
            //初始化日志信息
            t_rentpricelog.m_svcId        = -2;
            t_rentpricelog.m_svcStateCode = '*';
            t_rentpricelog.m_policyCompId = 0;
            t_rentpricelog.m_subFee       = -1;

            long factFee=-1;
            int errorCode=0;
            
            
            #ifdef FP_DEBUG
            FP_BEGIN(calcSubFee)
            #endif
            
            if ( !calcSubFee(m_pRateInfo->m_paramMgr->m_vecRentEvent[t_iIdx], calcSubCycId,
                             m_pRateInfo->m_cycParam.m_cycleId,
                             t_rentpricelog, factFee, errorCode))
            {
                // 计算失败更新该子帐期该固定费用项的费用为不存在
                m_pcdr[ATTRI_RENT_FEE_BASE+calcSubCycId]= factFee;
                //重新汇总
                long tt_sumFee = 0;
                for(int i=startSubCycId; i<=endSubCycId; ++i)
                {
                  if(m_pcdr[ATTRI_RENT_FEE_BASE+i].value()>0)//-1和0都刨去
                  {
                    tt_sumFee += m_pcdr[ATTRI_RENT_FEE_BASE+i].value();
                  }
                }
                //归整费用
                m_pcdr[ATTRI_RENT_SUMFEE] = tt_sumFee;
                //displayFee
                //m_pcdr[ATTRI_RENT_DISPLAYFEE] = tt_sumFee;//erased by Malone 2009-10-13 23:51 start
                
                // 记录错误信息
                registerErrorUser(m_pRateInfo->m_userId, E_RENTFEE_CALCSUBFEE_ERR, "0");
                //插入进行到一半的计算日志
                m_pRateInfo->m_rentFeeLogInfoVector.push_back(t_rentpricelog);
            }
            #ifdef FP_DEBUG
            FP_END(calcSubFee);
            #endif
            //获取日志信息
            t_rentpricelog.m_subFee  = factFee;
            m_pRateInfo->m_rentFeeLogInfoVector.push_back(t_rentpricelog);
        }


        //更新相应事件的金额
        UserBill t_userBill;
        t_userBill.m_userId      = m_pRateInfo->m_userId;
        t_userBill.m_billId      = atoi(m_pRateInfo->m_paramMgr->m_vecRentEvent[t_iIdx].m_eventCode.c_str());
        t_userBill.m_value       = m_pcdr[ATTRI_RENT_DISPLAYFEE].value();
        t_userBill.m_discntValue = m_pcdr[ATTRI_RENT_DISPLAYFEE].value();
        t_userBill.m_adjustBefore = 0;
        
        /*removed for rentfeeItem 2011-3-23        
        m_userRentBills.push_back(t_userBill);


        //不需要认为输出 bool  Outputer::output(const int &r_billCycle)自动就输出CDR了
        //生成输出的话单
#ifdef __PROV090__  //如果是吉林		
       if (m_pcdr[ATTRI_NEED_OUTPUTCDR]!= FLAG_NO) //如果未禁止CDR输出，才输出到容器
       {
#endif	   
        m_pcdr->outputCdr(m_pRateInfo->m_OutputRule->m_pFormat); //CDR转成字符串
        
        #ifdef _DEBUG_
            cout << "m_pcdr->m_desBuff=" << m_pcdr->m_desBuff << endl;
        #endif
        m_pRateInfo->m_rentOutputCdrList.push_back(PString(m_pcdr->m_desBuff));
#ifdef __PROV090__  //如果是吉林		
		}
#endif*/	
        int t_itemCode=0, t_orderNo=0, t_sumMode=0; 
    
        if(!m_pRateInfo->m_paramMgr->getRentItemInfo(t_userBill.m_billId, t_itemCode, t_orderNo, t_sumMode))
        {
        	  char line[10], billid[10];
            memset(line, 0, 10);
            sprintf(line,"%d",__LINE__);
            memset(billid, 0, 10);
            sprintf(billid,"%d",t_userBill.m_billId);
            theErrorMessages->insert(E_RENTFEE_DEFAULT,string("m_pRateInfo->m_paramMgr->getRentItemInfo error! at")+
                         string(__FILE__)+string(line)+" eventId=" +string(billid));
            registerErrorUser(m_pRateInfo->m_billUserId, E_RENTFEE_GET_GETEVENTFROMITEM, "0");            
        	  return false;
        }
            
        RentBillInfo t_rentBillInfo;
        t_rentBillInfo.m_itemCode    = t_itemCode;
        t_rentBillInfo.m_eventTypeId = t_userBill.m_billId;
        t_rentBillInfo.m_orderNo     = t_orderNo;
        t_rentBillInfo.m_sumMode     = t_sumMode;
        t_rentBillInfo.m_value       = t_userBill.m_value;
            
      #ifdef _DEBUG_			
    		printf("t_itemCode=%d\n",t_itemCode);
    		printf("t_iter->m_userBill.m_billId==%d\n",t_userBill.m_billId);
    		printf("t_orderNo=%d\n",t_orderNo);
    		printf("t_sumMode=%d\n",t_sumMode);
    		printf("t_iter->m_userBill.m_value=%ld\n",t_userBill.m_value);
      #endif	
                
        RentOutBillCdr t_rentBillCdr;
    	  t_rentBillCdr.m_rentBillInfo       =	t_rentBillInfo;
    	      
        m_pcdr[ATTRI_RENT_ITEMCODE] =  t_itemCode; 
    	  setBillInfos(m_pcdr,t_rentBillInfo);	 
        m_pcdr->outputCdr(m_pRateInfo->m_OutputRule->m_pFormat); //CDR转成字符串
        t_rentBillCdr.m_rentCdr  = PString(m_pcdr->m_desBuff);
            
      #ifdef _DEBUG_
        cout << "m_rentCdr=" << t_rentBillCdr.m_rentCdr<< endl;
      #endif
            
    	  t_rentBillCdr.m_isOutPut = false;
    	  m_rentBillInfoVec.push_back(t_rentBillCdr);			
    	
    }// end of for(itrUserRentEvent = m_userRentEventIdxVec.begin();

    //从事件汇总提交到用户明细帐单
    #ifdef FP_DEBUG
    FP_BEGIN(mergeToUserBillProc);
    #endif
    if(!mergeToUserBill())
    {		cout<<"提交失败！！！"<<endl;
        #ifdef FP_DEBUG
        FP_END(mergeToUserBillProc);
        #endif
        return false;
    }
    #ifdef FP_DEBUG
    FP_END(mergeToUserBillProc);
    #endif
    
#ifdef _DEBUG_    
    cout<<"提交成功！！！"<<endl;
#endif
    return true;
}

//计算子帐期费用
bool RentProc::calcSubFee(const RentEvent& rentEvent,
                          const int subCycId,  //子帐期序列号
                          const int cycId,     //帐期
                          UserRentPriceLog& r_rentPricelog,
                          long& subfee, int& errCode)
{
    //设置当前子帐期id
    m_pcdr[ATTRI_RENT_CURRENT_SUBCYCID] = subCycId;
    //当前费用置-1，防止出现日志和实际收费不符的情况 added on 20090908
    m_pcdr[ATTRI_RENT_CURRENT_SUBFEE] = -1;

    //获取日志信息
    r_rentPricelog.m_subCycId = subCycId;

    //获取当前子帐期开始时间，结束时间
    T_DATE_TIME t_subStartDateTime, t_subEndDateTime;

    if(!m_pRateInfo->m_paramMgr->getSubCycBySubId(rentEvent.m_subCycType,
                                 subCycId, cycId, t_subStartDateTime, t_subEndDateTime))
    {
        //根据子帐期id,获取开始时间、结束时间失败
        return false;
    }

    //在某段时间内是否是有效用户（天津移动专用）
    if(!isEffectiveUser(t_subStartDateTime,t_subEndDateTime))
    {
        return false;
    }
    
    /*20090820 
    修改：先获取服务，再获取资费
    用服务的开始时间结束时间以及开户的时间，比照子帐期的开始结束时间，然后取资费，
    防止出现同一个子帐期内，虽然有服务，有资费，但是没有互相重叠的部分，从而获取到一个不重叠的资费，错误收取的情况
    begin*/

    UserServ userServ;
    //获取当前服务
    if(!hasUserServ(t_subStartDateTime, t_subEndDateTime, rentEvent.m_serviceId, userServ))
    {
        //没有获取到相应资费
        m_pcdr[ATTRI_RENT_SVC_BASE+subCycId] = -1;
        //获取日志信息
        r_rentPricelog.m_svcId = -1;
        return false;
    }
    else
    {
        //设置当前服务
        m_pcdr[ATTRI_RENT_SVC_BASE+subCycId] = rentEvent.m_serviceId;
        m_pcdr[ATTRI_RENT_CURRENT_SVC]           = rentEvent.m_serviceId;
        //获取日志信息
        r_rentPricelog.m_svcId = rentEvent.m_serviceId;
        //设置服务开始时间
        m_pcdr[ATTRI_RENT_CURRENT_SVCSTART]      = userServ.m_startDate;
        	  //设置服务结束时间 --add by duyu 2009.9.18
        m_pcdr[ATTRI_RENT_CURRENT_SVCEND]      = userServ.m_endDate;  
    }
    
    /*20090820
    获取当前服务的开始时间，结束时间，与子帐期开始时间、结束时间以及用户开户时间比较
    从而得到实际的开始时间结束时间，去做后面资费的获取
    防止出现同一个子帐期内，虽然有服务，有资费，但是没有互相重叠的部分，从而获取到一个不重叠的资费，错误收取的情况
    */
    T_DATE_TIME t_realStartDateTime, t_realEndDateTime;
    t_realStartDateTime = userServ.m_startDate;//默认服务开始时间为开始时间取资费
    t_realEndDateTime   = userServ.m_endDate;  //默认服务结束时间为结束时间取资费
    //取资费时候的开始时间
    if(t_realStartDateTime<t_subStartDateTime)//如果服务开始时间小于子帐期开始时间
    {    
        t_realStartDateTime=t_subStartDateTime;//取资费时候以子帐期起始时间为开始时间
    }
    //取资费时候的结束时间
    if(t_realEndDateTime>t_subEndDateTime)////如果服务结束时间大于子帐期结束时间
    {
        t_realEndDateTime=t_subEndDateTime;//取资费时候以子帐期结束时间为结束时间
    }    
    //如果是当月开户，再跟开户时间比较一下，以确定开始时间
    T_DATE_TIME t_openDate;
    t_openDate = (m_pRateInfo->m_idInfo).m_idAttr[IdInfo::ID_OPEN_DATE].substr(0, 14);
    if((t_openDate > m_pRateInfo->m_cycParam.m_beginTime)&&
       (t_openDate <= m_pRateInfo->m_cycParam.m_endTime))
    {
        if(t_realStartDateTime<t_openDate)//如果服务开始时间小于子帐期开始时间
        {    
            t_realStartDateTime=t_openDate;//取资费时候以子帐期起始时间为开始时间
        }    
    }
    
    //获取当前资费
    RateTpInfo* t_rateTpInfo;
    m_pRateInfo->m_paramMgr->m_rentPolicySortTag =
    m_pRateInfo->m_paramMgr->getRentPolicySortTag(rentEvent.m_eventCode);
    if((t_rateTpInfo = getUserSubCycRentTp(rentEvent.m_eventCode, 
                                           t_realStartDateTime, t_realEndDateTime))!=NULL)
    {
        //设置当前资费信息
        m_pcdr[ATTRI_RENT_CURRENT_TPSTARTDATE] = t_rateTpInfo->m_pUserTp->m_beginTime;
        //m_pcdr[ATTRI_RENT_CURRENT_TPENDDATE]   = t_rateTpInfo->m_pUserTp->m_endTime;
        //m_pcdr[ATTRI_RENT_CURRENT_TPINSID]     = t_rateTpInfo->m_pUserTp->m_tpObjId;

        //获取日志信息
        r_rentPricelog.m_policyCompId = t_rateTpInfo->m_rateTpId;
    }
    else
    {
        //没有取到相应资费
        return false;
    }
    
    /*20090820 
    修改：先获取服务，再获取资费
    用服务的开始时间结束时间以及开户的时间，比照子帐期的开始结束时间，然后取资费，
    防止出现同一个子帐期内，虽然有服务，有资费，但是没有互相重叠的部分，从而获取到一个不重叠的资费，错误收取的情况
    end*/

    UserServState servState;
    //Added by Malone 2009-9-23 14:26 start
    T_DATE_TIME t_servStateMinStartDateTime = "20501231235959";
    T_DATE_TIME t_servStateMaxEndDateTime = "19000101000000";
    //Added by Malone 2009-9-23 14:26 end
    
    //获取当前状态
    m_pRateInfo->m_paramMgr->m_rentServerSortTag =
    m_pRateInfo->m_paramMgr->getSvcStateSortTag(rentEvent.m_eventCode);

    //Added by Malone 2009-9-23 14:30 start
    /*
    if(!getUserServState(t_subStartDateTime, t_subEndDateTime, rentEvent.m_serviceId, servState))
    */
    if(!getUserServState(t_subStartDateTime, t_subEndDateTime, rentEvent.m_serviceId, servState,
    	                   t_servStateMinStartDateTime, t_servStateMaxEndDateTime)) 	
    //Added by Malone 2009-9-23 14:30 end
    {
        //获取状态有误
        m_pcdr[ATTRI_RENT_STATE_BASE+subCycId] = "?";
        //获取日志信息
        r_rentPricelog.m_svcStateCode = '?';
        return false;
    }
    else
    {
        //设置当前状态
        m_pcdr[ATTRI_RENT_STATE_BASE+subCycId] = servState.m_servStateCode;
        m_pcdr[ATTRI_RENT_CURRENT_SVCSTATE]        = servState.m_servStateCode;
        m_pcdr[ATTRI_RENT_CURRENT_SVCSTATESTART]   = servState.m_startDate;
        
        // Added by Malone 2009-9-23 11:43 start
        m_pcdr[ATTRI_RENT_CURRENT_SVCSTATEMINSTART]   = t_servStateMinStartDateTime;
        m_pcdr[ATTRI_RENT_CURRENT_SVCSTATEMAXEND]   = t_servStateMaxEndDateTime;
        // Added by Malone 2009-9-23 11:43 end
        
        //获取日志信息
        r_rentPricelog.m_svcStateCode = servState.m_servStateCode;
    }

    //执行该资费
    try
    {   m_pRateInfo->m_rateFeeInfo.m_pCurRateTpInfo = t_rateTpInfo;//add by xuf 2009-8-19 15:27:54
        EventFeePolicy* l_pEventTp = NULL;
        l_pEventTp = t_rateTpInfo->m_pEventTp;

        if (l_pEventTp!=NULL)
         {

//#ifdef _DEBUG_
//   l_pEventTp->dump("rent:"); 
//#endif
            l_pEventTp->execute();
	  }
    }
    catch(exception &e)
    {
        #ifdef _DEBUG_
        cout<<"["<<__FILservStateMaxEndDateTime = "19000101000000";
    //Added by Malone 2009-9-23 14:26 end
    
    //鑾峰彇褰撳墠鐘舵
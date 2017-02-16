#include "EffectDiscnt.h"
#include "EffectAddup.h"
#include "ObjectBase.h"
#include "ObjectBill.h"
#include "PolicyInfo.h"
#include "base/exceptions.h"
#include "base/TimeUtil.h"
#include "Price.h"
#include <bitset> 
#include "Calendar.h" //add by taijq


bool EffectDiscnt::execute()
{
    long fee = 0;
    long leftFee = 0;
    
    if (m_effectObj==NULL)
    {        
        THROW_C_P2(BusinessException, -1, "优惠作用错误,作用对象为空："
                    "effect_id=%d,order_no=%c", (long)m_id, m_orderNo);
    }
    //获取应该优惠的费用
    getDiscountFee(fee);    

    ObjectBase   * t_effectObj=(ObjectBase *)m_effectObj;
	//部分用户群优惠涉及全账单标记
	if (m_globalInfo->m_detailBillMgr.getIdType()==GROUP_ID_TYPE &&
		(t_effectObj->m_roleCode == ROLECODE_SELF || t_effectObj->m_roleCode == ROLECODE_WIDECARD))
	{
        m_globalInfo->m_detailBillMgr.m_userIdsUnionTag=USERIDSUNIONTAG_ALL;
	}
	//获取作用对象的用户集合
	if (m_effectObj != NULL && t_effectObj->m_id != m_computeObj->m_id)
	{
        m_effectObj->geteffectUsers();
	}
    //对费用进行累计
    if (m_addupRule == NULL)
    {
        //将应该优惠的费用作用到账目上
        leftFee = updateDiscountFee(fee);  
    }
    else    
    {        
        //如果是累计优惠，对优惠值进行调整（不能超过累计上限值）
        long sumBeginTime=0, sumEndTime;        
        long marginFee=0;
        //获取剩余的优惠累计量
        if (m_addupRule->getAcctMargin(m_effectObj->m_attrId, marginFee,
                          sumBeginTime, sumEndTime))//有优惠限定
        {
            if (fee>marginFee)                  
                fee = marginFee;                  
        }
        
        //将应该优惠的费用作用到账目上
        leftFee = updateDiscountFee(fee);         
        m_addupRule->acctExecute(sumBeginTime,sumEndTime);
    }
    
    //执行下一条优惠规则
    if (m_next != NULL)
    {
        m_next->execute();
    }
	//modify by taijq 20091213
    //m_effectObj->updateCurFee();
   // #ifdef _DEBUG_
    //cout<<m_id<<": "<<m_globalInfo->m_comBill.m_detailBill<<endl;
    //#endif
    return true;
}

void EffectDiscnt::getDiscountFee(long &fee)
{
    long sumFee=0;
    
    //根据计算方法得到应该优惠的费用
    dealComputeMethod(fee);
    //根据调整方法对优惠费用进行调整
    dealAdjustMethod(m_adjustMethod,fee);
    //将上月剩余的优惠值加进来
    addRemainFee(fee); 
    
    return;
}

//根据计算方法得到应该优惠的费用
void EffectDiscnt::dealComputeMethod(long &fee)
{
    if (m_computeObj != NULL)
    {
        m_computeObj->getObjectValue(fee);
    }

    long childValue;
    long parentValue;
    long baseFee;
    long discntFee;
    double dFee;
    double dFee1;
    double dFee2;

    //added by liugj 20090505 广东代码移植北方扩充功能
	//unsigned int svcnum=0;
    unsigned int statenum=0;
    unsigned int discntnum=0;
    unsigned int recv_num=0;
    //unsigned int recv_halfnum1=0;
    //unsigned int recv_halfnum2=0;
    T_DATE_TIME  curDateTime,t_beginTime,t_endTime;
    unsigned int currentDayNum=0,monthDayNum;
    T_OBJECT_ATTR laststopopentime;
    T_OBJECT_ATTR openDate;
    T_OBJECT_ATTR destroyDate;
    T_OBJECT_ATTR LastITime;
    //end of addition


    getParamValue(childValue,parentValue,baseFee,discntFee);    
   
   
    switch(m_computeMethod)
    {
    case COMP_METHOD_SVC:  //基本雷同于补收固定值，后面调整方式必须为FEE_ADJUST_SVC_STARTEND('s')，或者't'配合使用
        dFee = discntFee;  //补收金额写死固定在discntFee上，服务字段固定配置在baseFee上，后面adjustType必须为FEE_ADJUST_SVC_STARTEND('s')或者't'，以便配合使用
        break;    
        
    case COMP_METHOD_COMMON :    //通用

        //适用的计算方法：
        //1.减免固定值: let childValue=0
        //2.补收固定值: let childValue=0，discntFee = -500 
        //3.按比例减免: let baseFee=0,discntFee=0
        //4.按比例补收: let baseFee=0,discntFee=0,childValue=-10
        //5.设定: let baseFee=设定值,childValue=1,parentValue=1,discntFee=0
        // (X-优惠基准值)*折扣分子/折扣分母+减免固定值
        dFee = (fee - baseFee) * childValue / parentValue + discntFee;

        break;
        
    case COMP_METHOD_LIMIT_UP : //上限  or 超过参考值的部分进行打折(补收)(适用：云南特殊补收)
        if (fee>baseFee)
            dFee = (fee - baseFee) * childValue / parentValue + discntFee;
        else
            dFee = 0.0;
        break;
    
    case COMP_METHOD_LIMIT_DOWN://下限

        if (fee<baseFee)
            dFee = (fee - baseFee) * childValue / parentValue + discntFee;
        else
            dFee = 0.0;
        break;
            
    case COMP_METHOD_DERATE_LIMIT ://按比例减免或补收但不能超过限定的值
    case COMP_METHOD_PRESENT_LIMIT ://按比例赠送但不能超过指定的值 
        dFee = fee * childValue / parentValue + discntFee;
        //baseFee:限定值,>=0
        if (childValue>0)//减免有上限
        {
            if ((long)dFee>baseFee)
                dFee = baseFee;
        }
        else if (childValue<0)//补收有下限
        {
            if ((long)dFee < -baseFee)                
                dFee = -baseFee;
        }
        else
            dFee = 0.0;
        break;
        
    case COMP_METHOD_DERATE_XY:    //每打X元,减免y元        
        //适用：陕西特殊,超过prevaluen4以后减免prevaluen3,
        //以后每超过1元,减免prevaluen3
        if (fee>baseFee)
        {
            long num = (fee - baseFee) / parentValue + discntFee;
            dFee = childValue * num;
        }
        else
            dFee = 0.0;
        break;
        
    case  COMP_METHOD_FEE_DIVERT :    //费用转移
        dFee = - ((fee - baseFee) * childValue / parentValue + discntFee);        
        break;
    case  COMP_METHOD_PRESENT    :  //费用赠送到帐户存折
        dFee = (fee - baseFee) * childValue / parentValue + discntFee;
        break;
    
    case COMP_METHOD_SVC_ORDER :    
          // baseFee：服务（暂时不需要）
          // discntFee：每次计费的钱
            // childValue: 收取周期类型 1:按天   (暂时只支持按天)  2:按半月收取 3:整月收取 4:超过15天按全月收取,小于等于15天按半月收取
            // parentValue: 计算模式  1000:直接作用 1001:移动模式 1002:联通模式
        TimeUtil::now((char *)curDateTime.c_str());    
            
        if (curDateTime>m_globalInfo->m_cycParam.m_endTime)
        {
            curDateTime=m_globalInfo->m_cycParam.m_endTime;
        }
          
        //获取状态有效性的二进制数组
        
        if(m_SvcState.empty())
        	 m_SvcState = m_globalInfo->m_paramMgr->m_mapStateRecvTag;
        
        statenum=m_globalInfo->m_comBill.getSvcStateTime(m_SvcState,curDateTime);
        //获取优惠资料有效性的二进制数组
        
        t_beginTime=m_policyInfo.m_rateTpInfo->m_pUserTp->m_beginTime;
        t_endTime=m_policyInfo.m_rateTpInfo->m_pUserTp->m_endTime;
        
        if (t_endTime>curDateTime)
        {
            t_endTime=curDateTime;
        }
        
        if (t_beginTime<m_globalInfo->m_cycParam.m_beginTime)
        {
            t_beginTime=m_globalInfo->m_cycParam.m_beginTime;
        }
        
        discntnum=getTpTime(t_beginTime, t_endTime);
        
        //通过位运算获取本月每天的生效情况
        recv_num=statenum&discntnum;
        
        dFee = computedFee(recv_num,discntnum,childValue,parentValue,discntFee,curDateTime);
          
        break;        
// add by yuxk begin 2009-9-27 20:45:27          
    case COMP_METHOD_ADDUP :   //包使用量套餐退订时 需要根据未使用总量比例进行退费 
          openDate = m_globalInfo->m_idInfo.getIdAttr(IdInfo::ID_OPEN_DATE);
        (m_globalInfo->m_comBill).getLastStopOpenTime(laststopopentime);
         destroyDate = 
                 m_globalInfo->m_idInfo.getIdAttr(IdInfo::ID_REMOVE_DATE);

       if( laststopopentime!="20501231000000" &&
			   	laststopopentime > m_globalInfo->m_cycParam.m_beginTime)
            {
            t_beginTime = laststopopentime;
            }
        else if (openDate > m_globalInfo->m_cycParam.m_beginTime)
          {
          t_beginTime = openDate;
          }
        else 
            {
            t_beginTime = m_globalInfo->m_cycParam.m_beginTime;
           }
        if(m_policyInfo.m_rateTpInfo->m_pUserTp->m_beginTime >
           t_beginTime)
            t_beginTime = m_policyInfo.m_rateTpInfo->m_pUserTp->m_beginTime;
            
            
                 if ( 
               m_policyInfo.m_rateTpInfo->m_pUserTp->m_beginTime>m_globalInfo->m_cycParam.m_beginTime &&
               m_policyInfo.m_rateTpInfo->m_pUserTp->m_beginTime<m_globalInfo->m_cycParam.m_endTime &&
               (m_globalInfo->m_comBill).getVaildMonthDays(m_policyInfo.m_rateTpInfo->m_pUserTp->m_beginTime.substr(0,8)+"000000",m_policyInfo.m_rateTpInfo->m_pUserTp->m_beginTime.substr(0,8)+"235959")== 0 )
         	{
         T_OBJECT_ATTR firststopopenTime;
         (m_globalInfo->m_comBill).getFirstStopOpenTime(firststopopenTime,m_policyInfo.m_rateTpInfo->m_pUserTp->m_beginTime,m_globalInfo->m_cycParam.m_endTime);
         if (firststopopenTime >= t_beginTime && firststopopenTime!="20501231000000") 
         	{
         		t_beginTime=firststopopenTime;
         	}
         	else 
         		{
         			dFee=0.0;
         			return;
         			}
         		}
    
        //获取使用天数
        if (t_beginTime <= m_globalInfo->m_cycParam.m_endTime) 
        {	
        	currentDayNum = TimeUtil::daydiff(t_beginTime.c_str(), 
                                        m_globalInfo->m_cycParam.m_endTime.c_str());    
        }
        else
        {
        	currentDayNum = 0;
        }	                                
        //获取当月天数
        monthDayNum = TimeUtil::daydiff(m_globalInfo->m_cycParam.m_beginTime.c_str(), 
                                        m_globalInfo->m_cycParam.m_endTime.c_str());    

     if(destroyDate>m_globalInfo->m_cycParam.m_endTime || atol(destroyDate.c_str()) == 0)
        t_endTime = m_globalInfo->m_cycParam.m_endTime;
        else
        t_endTime = destroyDate;
     if(m_policyInfo.m_rateTpInfo->m_pUserTp->m_endTime <  t_endTime) t_endTime = m_policyInfo.m_rateTpInfo->m_pUserTp->m_endTime;
     if (t_endTime.substr(8,6)!="235959")			 t_endTime=t_endTime.substr(0,8)+"000000";

		if (t_endTime < m_globalInfo->m_cycParam.m_endTime &&  baseFee*monthDayNum < parentValue*currentDayNum )
 		{
			dFee1=baseFee*discntFee*1.0/(parentValue*1.0);
		}
		else
		{
			dFee1=discntFee*currentDayNum*1.0/(monthDayNum*1.0);
		}
    
    if (t_beginTime <= t_endTime)
    {	 
    	currentDayNum = TimeUtil::daydiff(t_beginTime.c_str(),t_endTime.c_str()); 
    }
    else
    {
    	currentDayNum = 0;
    }
        dFee2=discntFee*currentDayNum*1.0/(monthDayNum*1.0);
     if (childValue==0)//0：包使用量和非包使用量计算方法比较，按费用多的收取；1：按包使用量收取；2：按非包使用量收取
     {
     if (dFee1>dFee2) dFee=dFee2;
     else dFee=dFee1;
     }
     else if (childValue==1)
     {
     dFee=dFee1;
     }
     else
     {
     dFee=dFee2;
     }
        
        break;    
// add by yuxk end 2009-9-27 20:45:27          
        
    default:
        dFee = 0.0;
        THROW_C_P3(BusinessException, -1, "优惠配置错误,未知的计算方法："
                    "effect_id=%d,order_no=%d,comp_method=%d", 
                    (long)m_id, m_orderNo, m_computeMethod);
        break;
    }
    if(dFee>0.0)
        fee = dFee + 0.5;
    else
        fee = dFee - 0.5;

    return;
}

void EffectDiscnt::getParamValue(long &childValue,long &parentValue,
                              long &baseFee,long &discntFee)
{    
    T_TP_ID r_paramId;
    const char *paramValue;
    //获取分母值
    if (m_parentValue[0] == '?') //需要替换参数
    {
        r_paramId = &m_parentValue[1];
        paramValue = m_policyInfo.m_rateTpInfo->getParamValue(r_paramId);
        if (paramValue == NULL)
        {
            THROW_C_P2(BusinessException, -1, "优惠配置错误,实例化的参数值为空："
                    "effect_id=%d,order_no=%d", (long)m_id, m_orderNo);
        }
        parentValue = atol(paramValue);
    }
    else if (m_parentValue[0] == '*') //通过对象取分母值
  	{
    	if (m_parentObj!=NULL) 
    	{   
      	m_parentObj->getObjectValue(parentValue);
      	//如果分母从对象中去,值为0,则令分母为一个很大的值,使作用值为0
      	if(parentValue==0)
      	{
        	parentValue = 0xFFFFFFFFFFFFFFF;
      	}
    	}
    	else
     	THROW_C_P3(BusinessException, -1, "优惠配置错误,分母对象为空："
      	"effect_id=%d,order_no=%d,parent_value=%s",(long)m_id,m_orderNo,m_parentValue);    
  	}
    else
    {
        parentValue = atol(m_parentValue);
    }
    if ( parentValue == 0 )
    {
        THROW_C_P3(BusinessException, -1, "优惠配置错误,分母值为0："
                    "effect_id=%d,order_no=%d,parent_value=%d", 
                    (long)m_id, m_orderNo, parentValue);
    }

    //获取分子值
    if (m_childValue[0] == '?') //需要替换参数
    {
        r_paramId = &m_childValue[1];
        paramValue = m_policyInfo.m_rateTpInfo->getParamValue(r_paramId);
        if (paramValue == NULL)
        {
            THROW_C_P2(BusinessException, -1, "优惠配置错误,实例化的参数值为空："
                    "effect_id=%d,order_no=%d", (long)m_id, m_orderNo);
        }
        childValue = atol(paramValue);            
    }
    else if(m_childValue[0] == 'E')
    {
		char *expr = &m_childValue[1];
		if (!m_globalInfo->explanExpr(childValue,expr,m_policyInfo.m_rateTpInfo))
		{
			THROW_C_P2(BusinessException, -1, "优惠配置错误,表达式解析错误："
		                                  "effect_id=%d,order_no=%d",(long)m_id, m_orderNo);
		}    	
    }
    else
    {
        childValue = atol(m_childValue);
    }
        
    //减免固定值
    baseFee = 0;
    if (m_baseObj != NULL)
    {
        m_baseObj->getObjectValue(baseFee);
    }
    
    if (m_baseFee[0] == '?') //需要替换参数
    {
        r_paramId = &m_baseFee[1];
        paramValue = m_policyInfo.m_rateTpInfo->getParamValue(r_paramId);
        if (paramValue == NULL)
        {
            THROW_C_P2(BusinessException, -1, "优惠配置错误,实例化的参数值为空："
                    "effect_id=%d,order_no=%d", (long)m_id, m_orderNo);
        }
        baseFee += atol(paramValue);
    }
     else  if (m_baseFee[0] == 'E') //表达式 
		{
			char *expr = &m_baseFee[1];
			if (!m_globalInfo->explanExpr(baseFee,expr,m_policyInfo.m_rateTpInfo))
			{
				THROW_C_P2(BusinessException, -1, "优惠配置错误,表达式解析错误："
			                                  "effect_id=%d,order_no=%d",(long)m_id, m_orderNo);
			}
		}
    else
    {
        baseFee += atol(m_baseFee);
    }
    
    dealAdjustMethod(m_baseAdjustMethod,baseFee);    

    //优惠基准值
    if (m_discntFee[0] == '?') //需要替换参数
    {
        r_paramId = &m_discntFee[1];
        paramValue = m_policyInfo.m_rateTpInfo->getParamValue(r_paramId);
        if (paramValue == NULL)
        {
            THROW_C_P2(BusinessException, -1, "优惠配置错误,实例化的参数值为空："
                    "effect_id=%d,order_no=%d", (long)m_id, m_orderNo);
        }
        discntFee = atol(paramValue);
    }
    else  if (m_discntFee[0] == 'E') //表达式
		{
			char *expr = &m_discntFee[1];
			if (!m_globalInfo->explanExpr(discntFee,expr,m_policyInfo.m_rateTpInfo))
			{
				THROW_C_P2(BusinessException, -1, "优惠配置错误,表达式解析错误："
			                                  "effect_id=%d,order_no=%d",(long)m_id, m_orderNo);
			}
		}
    else
    {
        discntFee = atol(m_discntFee);
    }
    
    return;
}

void EffectDiscnt::dealAdjustMethod(char &adjustType,long &fee)
{    
    T_DATE_TIME  curDateTime;
    if(adjustType != FEE_ADJUST_COMMON)
    {    
    TimeUtil::now((char *)curDateTime.c_str());
      if (curDateTime >= m_globalInfo->m_cycParam.m_endTime)
      {
        curDateTime = m_globalInfo->m_cycParam.m_endTime;
      }
    }
    
    //0:无关系    
    if (adjustType == FEE_ADJUST_COMMON)
    {
        return;    
    }
    //1:实际作用值为(作用值×当月实际天数)/全月天数
    else if (adjustType == FEE_ADJUST_DAY_MONTH)
    {
        //帐期开始时间,当前时间
        computeFeeByDay(m_globalInfo->m_cycParam.m_beginTime,
                        curDateTime, fee);        
    }    
    //2:实际作用值为(作用值×开户日期起实际天数)/全月天数(针对新开户用户)    
    else if (adjustType == FEE_ADJUST_USER_NEW)
    {
        T_DATE_TIME beginTime;
        
        T_OBJECT_ATTR & openDate = 
                 m_globalInfo->m_idInfo.getIdAttr(IdInfo::ID_OPEN_DATE);
        if(openDate > m_globalInfo->m_cycParam.m_beginTime)
            beginTime = openDate;
        else
            beginTime = m_globalInfo->m_cycParam.m_beginTime;
        
        computeFeeByDay(beginTime,curDateTime, fee);
    }    
    //5:套餐实际生效天数
  else if (adjustType == FEE_ADJUST_DAY_USERED)
    {
        T_DATE_TIME  beginTime;
        
        if(m_policyInfo.m_rateTpInfo->m_pUserTp->m_beginTime <
           m_globalInfo->m_cycParam.m_beginTime)
            beginTime = m_globalInfo->m_cycParam.m_beginTime;
        else
            beginTime = m_policyInfo.m_rateTpInfo->m_pUserTp->m_beginTime;
        
        if(m_policyInfo.m_rateTpInfo->m_pUserTp->m_endTime < curDateTime)
            curDateTime = m_policyInfo.m_rateTpInfo->m_pUserTp->m_endTime;
        
        computeFeeByDay(beginTime, curDateTime, fee);
    }
    //6:申请和取消月 按享受天数收
  else if (adjustType == FEE_ADJUST_DAY_USERED_2)
    {
        T_DATE_TIME  endTime,beginTime;
        
        if(m_policyInfo.m_rateTpInfo->m_pUserTp->m_beginTime <
           m_globalInfo->m_cycParam.m_beginTime)
            beginTime = m_globalInfo->m_cycParam.m_beginTime;
        else
            beginTime = m_policyInfo.m_rateTpInfo->m_pUserTp->m_beginTime;
            
        if(m_policyInfo.m_rateTpInfo->m_pUserTp->m_endTime >
           m_globalInfo->m_cycParam.m_endTime)
            endTime = m_globalInfo->m_cycParam.m_endTime;
        else
            endTime = m_policyInfo.m_rateTpInfo->m_pUserTp->m_endTime;
        
        computeFeeByDay(beginTime, endTime, fee);
    }
    //3:实际作用值为(作用值×月初到最后停机时间的实际天数)/全月天数(针对停机用户)
    else if (adjustType == FEE_ADJUST_USER_STOP)
    {    
        T_DATE_TIME endTime;
        
        m_globalInfo->m_comBill.getLastStopTime(m_globalInfo->m_idInfo);
        T_OBJECT_ATTR &lastStopTime = 
                  m_globalInfo->m_idInfo.getIdAttr(IdInfo::ID_LASTSTOP_TIME);
        if(lastStopTime > curDateTime)
            endTime = curDateTime;
        else
            endTime = lastStopTime;
        
        computeFeeByDay(m_globalInfo->m_cycParam.m_beginTime, endTime, fee);
    }
  //4.实际作用值为(作用值×开户日期起到最后停机时间的实际天数)/全月天数(针对新开户停机用户)
    else if (adjustType == FEE_ADJUST_USER_NEWSTOP)
    {
        T_DATE_TIME beginTime,endTime;
        
        T_OBJECT_ATTR &openDate = 
                  m_globalInfo->m_idInfo.getIdAttr(IdInfo::ID_OPEN_DATE);
        if(openDate > m_globalInfo->m_cycParam.m_beginTime)
            beginTime = openDate;
        else
            beginTime = m_globalInfo->m_cycParam.m_beginTime;
        
        m_globalInfo->m_comBill.getLastStopTime(m_globalInfo->m_idInfo);
        T_OBJECT_ATTR &lastStopTime = 
                  m_globalInfo->m_idInfo.getIdAttr(IdInfo::ID_LASTSTOP_TIME);
        if(lastStopTime > curDateTime)
            endTime = curDateTime;
        else
            endTime = lastStopTime;
                
        computeFeeByDay(beginTime, endTime, fee);
    }
	//added by liugj for Tianj Unicom
	//6.实际作用值为(作用值×开户日期起实际天数)/全月天数(针对新开户用户 跟'2'的区别是，'2'按天分摊收取，'8'一次性收取，月末情况一样
    else if (adjustType == FEE_ADJUST_USER_NEW_2)
	{
		T_DATE_TIME beginTime;
		
		T_OBJECT_ATTR & openDate = 
		         m_globalInfo->m_idInfo.getIdAttr(IdInfo::ID_OPEN_DATE);
		if(openDate>m_globalInfo->m_cycParam.m_beginTime)
			beginTime = openDate;
		else
			beginTime = m_globalInfo->m_cycParam.m_beginTime;
		
		computeFeeByDay(beginTime,m_globalInfo->m_cycParam.m_endTime, fee);
	}
    //a:实际作用值为(作用值×[开户日期-销户日期]起实际天数)/全月天数
    else if (adjustType == FEE_ADJUST_OPEN_DESTROY)
    {
        T_DATE_TIME beginTime,endTime;
        
        T_OBJECT_ATTR & openDate = 
                 m_globalInfo->m_idInfo.getIdAttr(IdInfo::ID_OPEN_DATE);
        if(openDate > m_globalInfo->m_cycParam.m_beginTime)
            beginTime = openDate;
        else
            beginTime = m_globalInfo->m_cycParam.m_beginTime;

        T_OBJECT_ATTR & destroyDate = 
                 m_globalInfo->m_idInfo.getIdAttr(IdInfo::ID_PREDESTROY_TIME);
        if(destroyDate>curDateTime || atol(destroyDate.c_str()) == 0)
            endTime = curDateTime;
        else
            endTime = destroyDate;
        
        computeFeeByDay(beginTime,endTime, fee);
    }
    //x.实际作用值为(作用值×[开户日期-月末]起开通(0NAB57)的实际天数)/全月天数
    else if (adjustType == FEE_ADJUST_OPEN_FACTDAY)
    {
        T_DATE_TIME beginTime,endTime;
               
        T_OBJECT_ATTR & openDate = 
                 m_globalInfo->m_idInfo.getIdAttr(IdInfo::ID_OPEN_DATE);
        if(openDate > m_globalInfo->m_cycParam.m_beginTime)
        {
            beginTime = openDate;
        }
        else
        {
            beginTime = m_globalInfo->m_cycParam.m_beginTime;
        }
        
        if(curDateTime > m_globalInfo->m_cycParam.m_endTime)
        {
            endTime = m_globalInfo->m_cycParam.m_endTime;
        }
        else
        {
            endTime = curDateTime;
        }
        
        float adjustValue = 0.5;
        if (fee<0)
            adjustValue = -0.5;
        
        unsigned int currentDayNum=0,monthDayNum;
        //获取开通的实际天数
        (m_globalInfo->m_comBill).getDaysByState("0NAB57", beginTime, endTime, currentDayNum);
        if(currentDayNum < 1)
        {
            fee = 0;
            return;
        }
        //获取当月天数
        monthDayNum = TimeUtil::daydiff(m_globalInfo->m_cycParam.m_beginTime.c_str(), 
                                        m_globalInfo->m_cycParam.m_endTime.c_str());            

        if (currentDayNum > monthDayNum)
        {
            currentDayNum = monthDayNum;
        }
        fee = 1.0 * fee * currentDayNum / monthDayNum + adjustValue;
    }
    //y.实际作用值为(作用值×[开户日期-月末]起开通(0NAB)的实际天数)/全月天数
    else if (adjustType == FEE_ADJUST_OPEN_FACTDAY1)
    {
        T_DATE_TIME beginTime,endTime;
               
        T_OBJECT_ATTR & openDate = 
                 m_globalInfo->m_idInfo.getIdAttr(IdInfo::ID_OPEN_DATE);
        if(openDate > m_globalInfo->m_cycParam.m_beginTime)
        {
            beginTime = openDate;
        }
        else
        {
            beginTime = m_globalInfo->m_cycParam.m_beginTime;
        }
        
        if(curDateTime > m_globalInfo->m_cycParam.m_endTime)
        {
            endTime = m_globalInfo->m_cycParam.m_endTime;
        }
        else
        {
            endTime = curDateTime;
        }
        
        float adjustValue = 0.5;
        if (fee<0)
            adjustValue = -0.5;
        
        unsigned int currentDayNum=0,monthDayNum;
        //获取开通的实际天数
        (m_globalInfo->m_comBill).getDaysByState("0NAB", beginTime, endTime, currentDayNum);
        if(currentDayNum < 1)
        {
            fee = 0;
            return;
        }
        //获取当月天数
        monthDayNum = TimeUtil::daydiff(m_globalInfo->m_cycParam.m_beginTime.c_str(), 
                                        m_globalInfo->m_cycParam.m_endTime.c_str());            

        if (currentDayNum > monthDayNum)
        {
            currentDayNum = monthDayNum;
        }
        fee = 1.0 * fee * currentDayNum / monthDayNum + adjustValue;
    }
    //s.实际作用值为 作用值*[服务开始时间（月初）-服务结束时间（月末）]当月实际开通天数/全月天数
    else if (adjustType == FEE_ADJUST_SVC_STARTEND)
    {
        //取base值，获取服务ID
        long childValue;
        long parentValue;
        long baseFee;
        long discntFee;
        getParamValue(childValue,parentValue,baseFee,discntFee);    
        
        //获取该服务开始结束时间
        T_DATE_TIME startDate, endDate;
        if(!(m_globalInfo->m_comBill).getServStartEndDate((int)baseFee, startDate, endDate))
        {
            fee = 0;
            return;
        }
        
        if(startDate<m_globalInfo->m_cycParam.m_beginTime)
        {
            startDate = m_globalInfo->m_cycParam.m_beginTime;
        }
        
        if(endDate>m_globalInfo->m_cycParam.m_endTime)
        {
            endDate = m_globalInfo->m_cycParam.m_endTime;
        }
        
        if(endDate<=startDate)
        {
            //时间有错误
            fee = 0;
            return;
        }
        
        //获取当前时间
        
        //如果系统时间小于服务开始时间，暂时不收取
        if(curDateTime<startDate)
        {
            fee = 0;
            return;
        }
        
        if(curDateTime<endDate)
        {
            endDate = curDateTime;
        }
        
        computeFeeByDay(startDate,endDate, fee);
    }
    else if (adjustType == FEE_ADJUST_SVC_STARTEND2)  //当月15号以后生效||当月15号之前失效,收一半;否则收全部
    {
        //取base值，获取服务ID
        long childValue;
        long parentValue;
        long baseFee;
        long discntFee;
        getParamValue(childValue,parentValue,baseFee,discntFee);    
        
        //获取该服务开始时间
        T_DATE_TIME startDate, endDate;
        if(!(m_globalInfo->m_comBill).getServStartEndDate((int)baseFee, startDate, endDate))
        {
            fee = 0;
            return;
        }
        
        //月中以后生效||月中之前失效,收一半
        if( (startDate>m_globalInfo->m_cycParam.m_halfTime)
          ||(endDate<m_globalInfo->m_cycParam.m_halfTime))  
        {
            fee = fee/2;
            return;
        }
    }
// add by yuxk begin 2009-9-27 20:45:27          
    else if (adjustType == FEE_ADJUST_STOPOPEN)  //实际作用值为(作用值×[复话时间-月末]天数)/全月天数(一次性计算)
    {
    	  T_DATE_TIME beginTime;
         T_OBJECT_ATTR &openDate = 
                  m_globalInfo->m_idInfo.getIdAttr(IdInfo::ID_OPEN_DATE);
         T_OBJECT_ATTR laststopopentime;
        (m_globalInfo->m_comBill).getLastStopOpenTime(laststopopentime);

               if( laststopopentime!="20501231000000" &&
			   	laststopopentime > m_globalInfo->m_cycParam.m_beginTime)
            {
            beginTime = laststopopentime;
            }
        else if (openDate > m_globalInfo->m_cycParam.m_beginTime)
          {
          beginTime = openDate;
          }
        else 
            {
            beginTime = m_globalInfo->m_cycParam.m_beginTime;
           }
        computeFeeByDay(beginTime,m_globalInfo->m_cycParam.m_endTime, fee);

    }
    else if (adjustType == FEE_ADJUST_OPEN_MONTHEND)  //实际作用值为(作用值×[开户--月末]天数)/全月天数(一次性计算)
    {
        T_DATE_TIME beginTime;
        
        T_OBJECT_ATTR & openDate = 
                 m_globalInfo->m_idInfo.getIdAttr(IdInfo::ID_OPEN_DATE);
        if(openDate > m_globalInfo->m_cycParam.m_beginTime)
            beginTime = openDate;
        else
            beginTime = m_globalInfo->m_cycParam.m_beginTime;
        
        computeFeeByDay(beginTime,m_globalInfo->m_cycParam.m_endTime, fee);
    }
    else if (adjustType == FEE_ADJUST_USER_DESTROY)  //收全月租，拆机时按[拆机时间--月末]天数进行退费
    {
        T_DATE_TIME beginTime,endTime;
        
        beginTime = m_globalInfo->m_cycParam.m_beginTime;

        T_OBJECT_ATTR & destroyDate = 
                 m_globalInfo->m_idInfo.getIdAttr(IdInfo::ID_REMOVE_DATE);
        if(destroyDate>m_globalInfo->m_cycParam.m_endTime || atol(destroyDate.c_str()) == 0)
            endTime = m_globalInfo->m_cycParam.m_endTime;
        else
            endTime = destroyDate;
        
        computeFeeByDay(beginTime,endTime, fee);
    }
    else if (adjustType == FEE_ADJUST_NON_PACKAGE)  //宽带非包使用量套餐(宽带当月套餐变更：先按旧套餐按天退费，再按新套餐按天收费。新装、停保复话用户按天计算套餐费。拆机时按天退套餐费，但不退包使用量用户的套餐费。一次性收取)
    {
//         if (  (m_globalInfo->m_comBill).getVaildMonthDays(m_policyInfo.m_rateTpInfo->m_pUserTp->m_beginTime,
//         	                                                 m_policyInfo.m_rateTpInfo->m_pUserTp->m_beginTime)== 0 )
//         	{
//         		fee=0.0;
//         		}
//          		else
//{    	  T_DATE_TIME beginTime,endTime;
//         T_OBJECT_ATTR laststopopentime;
//         T_OBJECT_ATTR &openDate = 
//                  m_globalInfo->m_idInfo.getIdAttr(IdInfo::ID_OPEN_DATE);
//        (m_globalInfo->m_comBill).getLastStopOpenTime(laststopopentime);
//        T_OBJECT_ATTR & destroyDate = 
//                 m_globalInfo->m_idInfo.getIdAttr(IdInfo::ID_REMOVE_DATE);
//
//       if( laststopopentime!="20501231000000" &&
//			   	laststopopentime > m_globalInfo->m_cycParam.m_beginTime)
//            {
//            beginTime = laststopopentime;
//            }
//        else if (openDate > m_globalInfo->m_cycParam.m_beginTime)
//          {
//          beginTime = openDate;
//          }
//        else 
//            {
//            beginTime = m_globalInfo->m_cycParam.m_beginTime;
//           }
//        if(m_policyInfo.m_rateTpInfo->m_pUserTp->m_beginTime >
//           beginTime)
//            beginTime = m_policyInfo.m_rateTpInfo->m_pUserTp->m_beginTime;
//        if(destroyDate>m_globalInfo->m_cycParam.m_endTime || atol(destroyDate.c_str()) == 0)
//            endTime = m_globalInfo->m_cycParam.m_endTime;
//        else
//            endTime = destroyDate;
//        if(m_policyInfo.m_rateTpInfo->m_pUserTp->m_endTime <
//           endTime)
//            endTime = m_policyInfo.m_rateTpInfo->m_pUserTp->m_endTime;
//         

     	  T_DATE_TIME beginTime,endTime,tp_beginTime,tp_endTime;
         T_OBJECT_ATTR laststopopentime;
         T_OBJECT_ATTR &openDate = m_globalInfo->m_idInfo.getIdAttr(IdInfo::ID_OPEN_DATE);
        (m_globalInfo->m_comBill).getLastStopOpenTime(laststopopentime);
        T_OBJECT_ATTR & destroyDate = m_globalInfo->m_idInfo.getIdAttr(IdInfo::ID_REMOVE_DATE);
        tp_beginTime=m_policyInfo.m_rateTpInfo->m_pUserTp->m_beginTime;
        tp_endTime=m_policyInfo.m_rateTpInfo->m_pUserTp->m_endTime;

       if( laststopopentime!="20501231000000" && laststopopentime > m_globalInfo->m_cycParam.m_beginTime)
            {
            beginTime = laststopopentime;
            }
        else if (openDate > m_globalInfo->m_cycParam.m_beginTime)
            {
            beginTime = openDate;
            }
        else 
            {
            beginTime = m_globalInfo->m_cycParam.m_beginTime;
            }
        if(tp_beginTime > beginTime) beginTime = tp_beginTime;
        if(destroyDate>m_globalInfo->m_cycParam.m_endTime || atol(destroyDate.c_str()) == 0)
            endTime = m_globalInfo->m_cycParam.m_endTime;
        else
            endTime = destroyDate;
        if(tp_endTime <  endTime) endTime = tp_endTime;
   //粗略考虑包年转包月情况
         if (  beginTime<endTime &&
               tp_beginTime>m_globalInfo->m_cycParam.m_beginTime &&
               tp_beginTime<m_globalInfo->m_cycParam.m_endTime &&
               (m_globalInfo->m_com       

     	  T_DATE_TIME beginTime,endTime,tp_beginTime,tp_endTime;
         T_OBJECT_ATTR laststopopentime;
         T_OBJECT_ATTR &openDate = m_globalInfo->m_idInfo.getIdAttr(IdInfo::ID_OPEN_DATE);
        (m_globalInfo->m_comBill).getLastStopOpenTime(laststopopentime);
        T_OBJECT_ATTR & destroyDate = m_globalInfo->m_idInfo.getIdAttr(IdInfo::ID_REMOVE_DATE);
        tp_beginTime=m_policyInfo.m_rateTpInfo->m_pUserTp->m_beginTime;
        tp_endTime=m_policyInfo.m_rateTpInfo->m_pUserTp->m_endTime;

       if( laststopopentime!="20501231000000" && laststopopentime > m_globalInfo->m_cycParam.m_beginTime)
            {
            beginTime = laststopopentime;
            }
        else if (openDate > m_globalInfo->m_cycParam.m_beginTime)
            {
            beginTime = openDate;
            }
        else 
            {
            beginTime = m_globalInfo->m_cycParam.m_beginTime;
            }
        if(tp_beginTime > beginTime) beginTime = tp_beginTime;
        if(destroyDate>m_globalInfo->m_cycParam.m_endTime || atol(destroyDate.c_str()) == 0)
            endTime = m_globalInfo->m_cycParam.m_endTime;
        else
            endTime = destroyDate;
        if(tp_endTime <  endTime) endTime = tp_endTime;
   //绮楃暐鑰冭檻鍖呭勾杞
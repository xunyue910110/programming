#include "EffectFeeCount.h"
#include "EffectAddup.h"
#include "ObjectBase.h"
#include "RateGlobalInfo.h"
#include "Price.h"
#include "AddupItem.h"
#include "base/exceptions.h"
#include "EventFeePolicy.h"
//add by yuxk begin 2009-9-27 20:38:55
#include "PolicyInfo.h"
#include "base/TimeUtil.h"
#include "EffectDiscnt.h"
#include "base/StringUtil.h"
//add by yuxk end 2009-9-27 20:38:55
EffectFeeCount::EffectFeeCount(){}
EffectFeeCount::~EffectFeeCount(){}

//判断各个子帐期费用是否相等(有可能是第几个子帐期到第几个子帐期费用相等与否)
bool EffectFeeCount::isSubFeeEqual(int startSubId, int endSubId) const
{
  //结束子帐期小于开始子帐期，返回假
  if(endSubId<startSubId) return false;
    
  RateGlobalInfo* t_pGlobalInfo = NodeBase::m_globalInfo;
  T_ATTRIBUTE_VALUE t = t_pGlobalInfo->m_cdr[ATTRI_RENT_FEE_BASE+startSubId];
  if(t.value()<=0) return false;//有一个子帐期为-1或者为0都不算子帐期相等
    
  for(int i=startSubId; i<=endSubId; i++)
  {
    if(t!=t_pGlobalInfo->m_cdr[ATTRI_RENT_FEE_BASE+i])
    {
      return false;
    }
  }
  
  return true;
}
  
//modify for rent new version 
bool EffectFeeCount::execute()
{//用于实现固定费批价

  RateGlobalInfo* t_pGlobalInfo = NodeBase::m_globalInfo;
  PCDR           &t_pcdr        = t_pGlobalInfo->m_pCdr;
  PolicyInfo*     t_pPolicyInfo = &(NodeBase::m_policyInfo);
//add by yuxk begin 2009-9-27 20:36:45
  EffectDiscnt   oEffectDiscntTmp;
//add by yuxk end 2009-9-27 20:36:45
  
  int t_itemCode   = atoi(t_pcdr[ATTRI_RENT_ITEMCODE].c_str());
  int t_startSubId = atoi(t_pcdr[ATTRI_RENT_STARTSUBID].c_str()); 
  int t_endSubId   = atoi(t_pcdr[ATTRI_RENT_ENDSUBID].c_str()); 
  int t_currSubId  = atoi(t_pcdr[ATTRI_RENT_CURRENT_SUBCYCID].c_str());
  int t_minSubId   = atoi(t_pcdr[ATTRI_RENT_CURRENT_MINSUBID].c_str());
  int t_maxSubId   = atoi(t_pcdr[ATTRI_RENT_CURRENT_MAXSUBID].c_str());
  int t_servicId   = atoi(t_pcdr[ATTRI_RENT_CURRENT_SVC].c_str());//Added by Malone 2009-10-12 17:19
  
  //获取当前计算时间(并非系统sysdate，是计算结束日期，通过endDay传入)
  T_DATE_TIME t_computeDate   = (t_pcdr[ATTRI_RENT_COMPUTERDATE].substr(0, 8)+"000000");
  //服务开始时间
  T_DATE_TIME t_servStartDate = t_pcdr[ATTRI_RENT_CURRENT_SVCSTART].substr(0, 14).c_str();  
  //资费开始时间
  T_DATE_TIME t_tpBeginDate   = t_pcdr[ATTRI_RENT_CURRENT_TPSTARTDATE].substr(0, 14).c_str();
  //服务结束时间
  T_DATE_TIME t_servStartEnd = t_pcdr[ATTRI_RENT_CURRENT_SVCEND].substr(0, 14).c_str();
  
  //Added by Malone 2009-10-8 16:39 start
  //子账期内优先级最高的服务状态在子账期内的最小开始时间
  T_DATE_TIME	t_svcstateMinStart = t_pcdr[ATTRI_RENT_CURRENT_SVCSTATEMINSTART].substr(0, 14).c_str();
  //子账期内优先级最高的服务状态在子账期内的最大结束时间
  T_DATE_TIME	t_svcstateMaxEnd = t_pcdr[ATTRI_RENT_CURRENT_SVCSTATEMAXEND].substr(0, 14).c_str();
  //Added by Malone 2009-10-8 16:39 end
  //Added by Malone 2009-10-14 16:45 start
  T_DATE_TIME t_open_date = (t_pGlobalInfo->m_idInfo).m_idAttr[IdInfo::ID_OPEN_DATE].substr(0, 14).c_str();
  T_DATE_TIME t_remove_date = (t_pGlobalInfo->m_idInfo).m_idAttr[IdInfo::ID_REMOVE_DATE].substr(0, 14).c_str();
  //Added by Malone 2009-10-14 16:45 end
  
  int intComputeDate = atoi(t_computeDate.substr(0, 8).c_str());
  
  //子帐期费用
  long t_subFee = 0;
  //汇总费用和显示费用
  long t_sumFee = 0, t_displayFee = 0;
  
  //计算子帐期费用
  switch(m_computeMethod)//计算模式
  {
      case FEE_COMP_METHOD_DIRECT: // 直接作用，单元费率以m_unitRatio为准
      if(getRentUnitRatio(t_subFee)==false)
      {
        //获取明细费用失败
        return false;
      }  
      break;
      
      case FEE_COMP_METHOD_CMCCFORMULA: // 中移动公式，每天单元费率以t_sumToIntFee*12/365，其他子帐期类型以此类推
      if(getSumToIntFee(t_subFee)==false)
      {
        //获取汇总费用失败
        return false;
      }
      //事实上只有按天收会如此配置，所以此处偷懒，直接除以365，而没有区别不同子帐期类型
      t_subFee = t_subFee*12/365+0.5;
      break;  
      
    case FEE_COMP_METHOD_CNUCFORMULA: // 联通公式，每天单元费率以t_sumToIntFee/当月最大子帐期数，其他子帐期类型以此类推
      if(getSumToIntFee(t_subFee)==false)
      {
        //获取汇总费用失败
        return false;
      }
      t_subFee = t_subFee/t_maxSubId+0.5;
      break;
      
    default:
    	return false;    	
      //告警报错，计算模式错误
        
  }// end of switch
  
  //设置CDR中的当前计算子帐期费用
  t_pcdr[ATTRI_RENT_CURRENT_SUBFEE]= t_subFee;
  t_pcdr[ATTRI_RENT_FEE_BASE+t_currSubId]= t_subFee;
  
  //费用汇总sumfee
  t_sumFee=0;
  t_displayFee=0;
  for(int i=t_startSubId; i<=t_endSubId; ++i)
  {
    if(t_pcdr[ATTRI_RENT_FEE_BASE+i].value()>0)//-1和0都刨去
    {
      t_sumFee += t_pcdr[ATTRI_RENT_FEE_BASE+i].value();
    }
  }
  //displayfee暂时设置成和sumfee一样的
  t_displayFee = t_sumFee;
  
  //归整
  T_SUBCYC_TYPE t_subCycType = t_pcdr[ATTRI_RENT_SUBCYCLETYPE].c_str()[0];
  if(t_subCycType==DAY_CYCTYPE_CODE)//如果计算类型是按天收的(计算周期类型需要通过CDR传递)
  {
    char sumToIntTag = getSumToIntTag();
    if(sumToIntTag!='8')
    {//如果不是是天津奥运88套餐特有的28号归整
      //并且是最后一个子帐期
      if(t_currSubId==t_maxSubId)
      {
        if(sumToIntTag!='0')//不归整
        {
          //进行费用归整
          switch(sumToIntTag)
          {
            case '1':   //全月费用相同归整
              //并且每个子帐期费用是相等的
              if(isSubFeeEqual(t_minSubId, t_maxSubId))
              {
                //将sumfee的数值填写成“归整数值”
                if(!getSumToIntFee(t_sumFee))
                {
                  //获取汇总费用失败
                  //置0
                  t_sumFee = 0;
                  t_displayFee = 0;
                }
                else
                {
                  t_displayFee = t_sumFee;
                }
              }
              break;
              
            case '2':  //全月费用相同归整(新用户入网起费用相同，月末归整) 
              if( atol(((t_pGlobalInfo->m_idInfo).m_idAttr[IdInfo::ID_OPEN_DATE].substr(0, 14)).c_str())
                  > atol(((t_pGlobalInfo->m_cycParam).m_beginTime.substr(0, 14)).c_str()) )//新开户用户
              {
                //从开户子帐期开始每个子帐期费用是相等的
                if(isSubFeeEqual(t_startSubId, t_maxSubId))
                {
                  //将sumfee的数值填写成“归整数值”
                  if(!getSumToIntFee(t_sumFee))
                  {
                    //获取汇总费用失败
                    //置0
                    t_sumFee = 0;
                    t_displayFee = 0;
                  }
                  else
                  {
                    t_displayFee = t_sumFee;
                  }
                }
              }
              else  //老用户
              {
                //从开户子帐期开始每个子帐期费用是相等的
                if(isSubFeeEqual(t_minSubId, t_maxSubId))
                {
                  //将sumfee的数值填写成“归整数值”
                  if(!getSumToIntFee(t_sumFee))
                  {
                    //获取汇总费用失败
                    //置0
                    t_sumFee = 0;
                    t_displayFee = 0;
                  }
                  else
                  {
                    t_displayFee = t_sumFee;
                  }
                }
              }
              break;
              
            case '3': //全月费用相同进行归整(新用户入网起费用相同，上半月开户归整到全部，下半月开户归整到一半)
              if( atol(((t_pGlobalInfo->m_idInfo).m_idAttr[IdInfo::ID_OPEN_DATE].substr(0, 14)).c_str())
                  > atol(((t_pGlobalInfo->m_cycParam).m_beginTime.substr(0, 14)).c_str()) )//新开户用户
              {
                //从开户子帐期开始每个子帐期费用是相等的
                if(isSubFeeEqual(t_startSubId, t_maxSubId))
                {
                  //将sumfee的数值填写成“归整数值”
                  if(!getSumToIntFee(t_sumFee))
                  {
                    //获取汇总费用失败
                    //置0 
                    t_sumFee = 0;
                    t_displayFee = 0;
                  }
                  else
                  {
                    //下半月开户
                    if( atol(((t_pGlobalInfo->m_idInfo).m_idAttr[IdInfo::ID_OPEN_DATE].substr(0, 14)).c_str())
                        > atol(((t_pGlobalInfo->m_cycParam).m_halfTime.substr(0, 14)).c_str()) )
                    {
                      t_sumFee = 1.0 * t_sumFee/2+0.5;
                      t_displayFee = t_sumFee;
                    }
                    else //上半月开户
                    {
                      t_sumFee = t_sumFee;
                      t_displayFee = t_sumFee;
                    }
                  }
                }
              }
              else  //老用户
              {
                //从开户子帐期开始每个子帐期费用是相等的
                if(isSubFeeEqual(t_minSubId, t_maxSubId))
                {
                  //将sumfee的数值填写成“归整数值”
                  if(!getSumToIntFee(t_sumFee))
                  {
                    //获取汇总费用失败
                    //置0
                    t_sumFee = 0;
                    t_displayFee = 0;
                  }
                  else
                  {
                    t_displayFee = t_sumFee;
                  }
                }
              }
              break;
              
            case '4':   //无条件归整
              //将sumfee的数值填写成“归整数值”
              if(!getSumToIntFee(t_sumFee))
              {
                //获取汇总费用失败
                //置0
                t_sumFee = 0;
                t_displayFee = 0;
              }
              else
              {
                t_displayFee = t_sumFee;
              }
              break;
            default:
            	;
          }  //end of switch
        }//if(sumToIntTag!=SUMTOINT_NONEED)//不归整
      }//if(t_currSubId==t_maxSubId)
    }
    else if (sumToIntTag=='8')//如果是天津奥运88套餐特有的28号归整
    {
      //大于28号，判断子帐期费用是否相同，进行归整
      if(t_currSubId>=28)
      {
        if( atol(((t_pGlobalInfo->m_idInfo).m_idAttr[IdInfo::ID_OPEN_DATE].substr(0, 14)).c_str())
                  > atol(((t_pGlobalInfo->m_cycParam).m_beginTime.substr(0, 14)).c_str()) )//新开户用户
        {
          //从开户子帐期开始每个子帐期费用是相等的
          if(isSubFeeEqual(t_startSubId, t_currSubId))
          {
            //将sumfee的数值填写成“归整数值”
            if(!getSumToIntFee(t_sumFee))
            {
              //获取汇总费用失败
              //置0 
              t_sumFee = 0;
              t_displayFee = 0;
            }
            else
            {
              //下半月开户
              if( atol(((t_pGlobalInfo->m_idInfo).m_idAttr[IdInfo::ID_OPEN_DATE].substr(0, 14)).c_str())
                  > atol(((t_pGlobalInfo->m_cycParam).m_halfTime.substr(0, 14)).c_str()) )
              {
                t_sumFee = 1.0 * t_sumFee/2+0.5;
                t_displayFee = t_sumFee;
              }
              else //上半月开户
              {
                t_sumFee = t_sumFee;
                t_displayFee = t_sumFee;
              }
            }
          }
        }else  //老用户
        {
          //从开户子帐期开始每个子帐期费用是相等的
          if(isSubFeeEqual(t_minSubId, t_currSubId))
          {
            //将sumfee的数值填写成“归整数值”
            if(!getSumToIntFee(t_sumFee))
            {
              //获取汇总费用失败
              //置0
              t_sumFee = 0;
              t_displayFee = 0;
            }
            else
            {
              t_displayFee = t_sumFee;
            }
          }
        }//end of 新开户用户 老用户
      }//end of if(t_currSubId>=28)
    }//end of else if (sumToIntTag=='8')//如果是天津奥运88套餐特有的28号归整
    
  }//if(t_subCycType==DAY_CYCTYPE_CODE)
  else
  {
    char sumToIntTag = getSumToIntTag();
    
    //Added by Malone 2009-10-6 16:22 start
    if (sumToIntTag=='9')//天津联通固定电话需求：
    	                   //在同一计费周期办理开停机或办理停开机次数在三次及以上的
    	                   //(停、开机均算一次)不论上下半月均收取一个月的月租。 
    	                   //有效的主服务状态持续时间段 >= 3 规整到全月。
    {
    		vector<UserServState>::iterator UserServState_iter;
    		
    		int iCountDiff  = 0;
    		
    		char stateReturn = '?';
    		char stateTemp = '?';
    		
    		//m_vecUserServState 已经按开始时间排序，如未排序，需要先排序
    		for(UserServState_iter = t_pGlobalInfo->m_vecUserServState.begin();
    		    UserServState_iter != t_pGlobalInfo->m_vecUserServState.end();
    		    ++UserServState_iter)
    	  {
    	  		if ( (UserServState_iter->m_mainTag == '1') &&
    	  			   (UserServState_iter->m_startDate < (t_pGlobalInfo->m_cycParam).m_endTime) &&
    	  			   (UserServState_iter->m_endDate > (t_pGlobalInfo->m_cycParam).m_beginTime)
    	  			 )
    	  		{
    	  			stateTemp = UserServState_iter->m_servStateCode;
    	  			
    	  			if ( stateReturn != '?' )
    	  			{
    	  					if ( stateTemp != stateReturn )
    	  					{
    	  							//Modified by Malone 2009-11-9 21:23 start
    	  							//天津要求只判断0和1、P、Q状态之间的变更情况
    	  							/*
    	  							//两个紧挨着的有效状态不一致，并且其中一个状态为'0'，代表状态变动一次
    	  							if ( (stateReturn == '0') || (stateTemp == '0')  )
    	  							*/
    	  							if ( (stateReturn == '0' && stateTemp == '1')||
    	  							     (stateReturn == '0' && stateTemp == 'P')||
    	  							     (stateReturn == '0' && stateTemp == 'Q')||
    	  							     (stateReturn == '1' && stateTemp == '0')||
    	  							     (stateReturn == 'P' && stateTemp == '0')||
    	  							     (stateReturn == 'Q' && stateTemp == '0')

    	  							   )
    	  							//Modified by Malone 2009-11-9 21:23 end
    	  							{
    	  									iCountDiff ++;
    	  							}
    	  					}
    	  					stateReturn = stateTemp;
    	  			}
    	  			else
    	  			{
    	  					stateReturn = stateTemp;
    	  			}
    	  		}
    	  }
    	  //状态变动次数大于等于3次
    	  if ( iCountDiff >= 3 )
    	  {    	  	  
    	  	  //将sumfee的数值填写成“归整数值”
            /*
            if(!getSumToIntFee(t_sumFee))
            {
              //获取汇总费用失败
              //置0
              t_sumFee = 0;
              t_displayFee = 0;
            }
            else
            {
              t_displayFee = t_sumFee;
            }
            */
            //由于同一个price_id可能存在两种费率，所以规整费用无法区分
            //由于都是月租，直接取各自的月租费率即可
            if(getRentUnitRatio(t_sumFee)==false)
      			{
              //获取汇总费用失败
              //置0
              t_sumFee = 0;
              t_displayFee = 0;
      			}
      			else
      			{
            	//Added by Malone start 2009-10-8 16:39 start
            	if ( (t_svcstateMinStart > (t_pGlobalInfo->m_cycParam).m_halfTime)||
    	  	  	     (t_svcstateMaxEnd < (t_pGlobalInfo->m_cycParam).m_halfTime)
    	  	  	 	 )//上半月停机或者下半月开机，天津现有的参数中td_b_feecount中的费率为半月费率，需做乘2处理。
    	  	  	{
    	  	  		t_sumFee = t_sumFee * 2;
    	  	  		
    	  	  		t_displayFee = t_sumFee;
    	  	  	}
    	  	  	//Added by Malone start 2009-10-8 16:39 end
    	  	  	else
    	  	  	{
    	  	  		t_displayFee = t_sumFee;
    	  	  	}
      			}
    	  }
    }
    //Added by Malone 2009-10-6 16:22 end
    //Added by Malone 2009-10-14 16:30 start
    else if (sumToIntTag=='A')//天津联通固定电话(私甲)需求：
    	                        //在同一计费周期办理开停机或办理停开机次数在三次及以上的
    	                        //(停、开机均算一次)不论上下半月均收取一个月的月租。 
    	                        //有效的主服务状态持续时间段 >= 3 规整到全月。
    	                        //新装：选取到的资费为标准费率，规整费用 = 标准费率 * (账期结束日 - 新装日 + 1)/当月最大天数
    	                        //拆机：选取到的资费为标准费率，规整费用 = 标准费率 * (拆机日 - 账期开始日 + 1)/当月最大天数
    {
    		vector<UserServState>::iterator UserServState_iter;
    		
    		int iCountDiff  = 0;
    		
    		char stateReturn = '?';
    		char stateTemp = '?';
    		
    		int t_open_day = atoi( t_open_date.substr(6, 2).c_str() );
    		int t_remove_day = atoi( t_remove_date.substr(6, 2).c_str() );
    		int t_acyc_begin_day = atoi( (t_pGlobalInfo->m_cycParam).m_beginTime.substr(6, 2).c_str() );
    		int t_acyc_end_day = atoi( (t_pGlobalInfo->m_cycParam).m_endTime.substr(6, 2).c_str() );
    		int t_compute_day = atoi(t_computeDate.substr(6, 2).c_str());
    		int t_day_interval = 0;
    		
    		//m_vecUserServState 已经按开始时间排序，如未排序，需要先排序
    		for(UserServState_iter = t_pGlobalInfo->m_vecUserServState.begin();
    		    UserServState_iter != t_pGlobalInfo->m_vecUserServState.end();
    		    ++UserServState_iter)
    	  {
    	  		if ( (UserServState_iter->m_mainTag == '1') &&
    	  			   (UserServState_iter->m_startDate < (t_pGlobalInfo->m_cycParam).m_endTime) &&
    	  			   (UserServState_iter->m_endDate > (t_pGlobalInfo->m_cycParam).m_beginTime)
    	  			 )
    	  		{
    	  			stateTemp = UserServState_iter->m_servStateCode;
    	  			
    	  			if ( stateReturn != '?' )
    	  			{
    	  					if ( stateTemp != stateReturn )
    	  					{
    	  							//Modified by Malone 2009-11-9 21:23 start
    	  							//天津要求只判断0和1、P、Q状态之间的变更情况
    	  							/*
    	  							//两个紧挨着的有效状态不一致，并且其中一个状态为'0'，代表状态变动一次
    	  							if ( (stateReturn == '0') || (stateTemp == '0')  )
    	  							*/
    	  							if ( (stateReturn == '0' && stateTemp == '1')||
    	  							     (stateReturn == '0' && stateTemp == 'P')||
    	  							     (stateReturn == '0' && stateTemp == 'Q')||
    	  							     (stateReturn == '1' && stateTemp == '0')||
    	  							     (stateReturn == 'P' && stateTemp == '0')||
    	  							     (stateReturn == 'Q' && stateTemp == '0')

    	  							   )
    	  							//Modified by Malone 2009-11-9 21:23 end
    	  							{
    	  									iCountDiff ++;
    	  							}
    	  					}
    	  					stateReturn = stateTemp;
    	  			}
    	  			else
    	  			{
    	  					stateReturn = stateTemp;
    	  			}
    	  		}
    	  }
    	  //状态变动次数大于等于3次
    	  if ( iCountDiff >= 3 )
    	  {    	  	  
    	  	  //将sumfee的数值填写成“归整数值”
            /*
            if(!getSumToIntFee(t_sumFee))
            {
              //获取汇总费用失败
              //置0
              t_sumFee = 0;
              t_displayFee = 0;
            }
            else
            {
              t_displayFee = t_sumFee;
            }
            */
            //由于同一个price_id可能存在两种费率，所以规整费用无法区分
            //由于都是月租，直接取各自的月租费率即可
            if(getRentUnitRatio(t_sumFee)==false)
      			{
              //获取汇总费用失败
              //置0
              t_sumFee = 0;
              t_displayFee = 0;
      			}
      			else
      			{
            	//Added by Malone start 2009-10-8 16:39 start
            	if ( (t_svcstateMinStart > (t_pGlobalInfo->m_cycParam).m_halfTime)||
    	  	  	     (t_svcstateMaxEnd < (t_pGlobalInfo->m_cycParam).m_halfTime)
    	  	  	 	 )//上半月停机或者下半月开机，天津现有的参数中td_b_feecount中的费率为半月费率，需做乘2处理。
    	  	  	{
    	  	  		t_sumFee = t_sumFee * 2;
    	  	  		
    	  	  		t_displayFee = t_sumFee;
    	  	  	}
    	  	  	//Added by Malone start 2009-10-8 16:39 end
    	  	  	else
    	  	  	{
    	  	  		t_displayFee = t_sumFee;
    	  	  	}
      			}
    	  }
//begin ... modified by chenyong 2009-10-16 
//对于当月新装或者当月拆机用户,需要考虑两者同时出现，即当月开户当月拆机的情况,另外,中间如果用户是申请停机状态,当天也不收取费用	    	  
    	  else if (
    	  				 (t_open_date>=(t_pGlobalInfo->m_cycParam).m_beginTime && t_open_date<=(t_pGlobalInfo->m_cycParam).m_endTime)			||
    	  				 (t_remove_date>=(t_pGlobalInfo->m_cycParam).m_beginTime && t_remove_date<=(t_pGlobalInfo->m_cycParam).m_endTime)
    	  				)
    	 	{
    	 		t_day_interval = 0;
    	 		
    	 		//定义计算范围的起始日期和终止日期
    	 		T_DATE_TIME t_compute_date = t_computeDate.substr(0,8)+"235959";
    	 		
    	 		//获取起始日期
    	 		T_DATE_TIME t_begin_date;
    	 		if (t_open_date>=(t_pGlobalInfo->m_cycParam).m_beginTime && t_open_date<=(t_pGlobalInfo->m_cycParam).m_endTime)
    	 			t_begin_date = t_open_date;
    	 		else
						t_begin_date = (t_pGlobalInfo->m_cycParam).m_beginTime;
					
					//获取终止日期
					T_DATE_TIME t_end_date;
					if (t_remove_date<(t_pGlobalInfo->m_cycParam).m_beginTime)
					{
							t_end_date=t_compute_date>(t_pGlobalInfo->m_cycParam).m_endTime ? (t_pGlobalInfo->m_cycParam).m_endTime:t_compute_date; 
					}
					else
					{
						if (t_remove_date>=(t_pGlobalInfo->m_cycParam).m_beginTime && t_remove_date<=(t_pGlobalInfo->m_cycParam).m_endTime)
							t_end_date = t_remove_date>t_compute_date ? t_compute_date:t_remove_date;
						else	    	 		
	    	 			t_end_date = t_compute_date>(t_pGlobalInfo->m_cycParam).m_endTime ? (t_pGlobalInfo->m_cycParam).m_endTime:t_compute_date; 
					}    	 		
    	 		int	t_begin_day = atoi(t_begin_date.substr(6,2).c_str());
    	 		int	t_end_day = atoi(t_end_date.substr(6,2).c_str());
    	 		
    	 		//如果起始日期>终止日期
    	 		if (t_begin_day > t_end_day)
    	 		{
    	 			t_day_interval = 0;
    	 		}
    	 		else
    	 		{
    	 			t_day_interval = t_end_day-t_begin_day+1;
    	 			
    	 			//获取起始时间~终止时间内的状态为'1','P','Q'的天数
						int startsubcycid=1;
						int endsubcycid=31;
						int svcstate[31]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
						for(UserServState_iter = t_pGlobalInfo->m_vecUserServState.begin();
    		    		UserServState_iter != t_pGlobalInfo->m_vecUserServState.end();
    		    		++UserServState_iter)
    	  		{
	    	  		if (
	    	  				UserServState_iter->m_mainTag=='1'						&&
	    	  			  UserServState_iter->m_startDate<t_end_date		&&
	    	  			  UserServState_iter->m_endDate>t_begin_date		&&
	    	  			  (UserServState_iter->m_servStateCode=='1'			||
	    	  			   UserServState_iter->m_servStateCode=='P'			||
	    	  			   UserServState_iter->m_servStateCode=='Q')
	    	  			 )
	    	  		{
	    	  			if (UserServState_iter->m_startDate <= t_begin_date)
	    	  			{	
	    	  				startsubcycid = atoi(t_begin_date.substr(6,2).c_str());
	    	  			}
	    	  			else
	    	  			{
	    	  				startsubcycid = atoi(UserServState_iter->m_startDate.substr(6,2).c_str()); 
	    	  				if ( !(t_pGlobalInfo->m_comBill).judgeNotOpen(
	    	  									UserServState_iter->m_startDate.substr(0,8)+"000000", 
	    	  									UserServState_iter->m_startDate,
	    	  									t_pGlobalInfo->m_vecUserServState) )
	    	  				{
	    	  					startsubcycid = startsubcycid+1;
	    	  				}
	    	  			}
	    	  			
	    	  			if (UserServState_iter->m_endDate >= t_end_date)
            		{
                	endsubcycid = atoi(t_end_date.substr(6,2).c_str());
            		}
            		else
            		{
                	endsubcycid = atoi(UserServState_iter->m_endDate.substr(6,2).c_str()); 
 	    	  				if ( !(t_pGlobalInfo->m_comBill).judgeNotOpen(
	    	  									UserServState_iter->m_endDate,
	    	  									UserServState_iter->m_endDate.substr(0,8)+"235959", 
	    	  									t_pGlobalInfo->m_vecUserServState) )
	    	  				{
                  	endsubcycid = endsubcycid-1;
                  }
            		}
            
            		for (int j=startsubcycid; j<=endsubcycid; ++j)
            		{
									if (svcstate[j-1] == 0)
                  {
                  	svcstate[j-1] = 1;
                  }
            		}
	    	  		}
    	  		}
    	  		int iDays = 0;
    	  		for (int j=0; j<31; ++j)
    	  		{
    	  			if (svcstate[j] == 1)
    	  				++iDays;
    	  		}
    	  		
    	  		t_day_interval = t_day_interval - iDays;
    	  	}
    	  

    	  	if(getRentUnitRatio(t_sumFee)==false)
      		{
          	//获取汇总费用失败,置0
            t_sumFee = 0;
            t_displayFee = 0;
      		}
      		else
      		{
    	  		//下半月新装 || 上半月拆机 
    	  		if(t_open_date>=(t_pGlobalInfo->m_cycParam).m_halfTime ||
    	  			 (t_remove_date>=(t_pGlobalInfo->m_cycParam).m_beginTime&&t_remove_date<(t_pGlobalInfo->m_cycParam).m_halfTime))
    	  		{
      				//Modified by Malone start 2009-11-9 23:06 start
      				/*
      				t_sumFee = 1.0 * t_sumFee * 2 * t_day_interval / t_acyc_end_day + 0.5;
      				t_displayFee = t_sumFee;
      				*/
							if ( (t_svcstateMinStart > (t_pGlobalInfo->m_cycParam).m_halfTime)||
							     (t_svcstateMaxEnd < (t_pGlobalInfo->m_cycParam).m_halfTime)
							    )//上半月停机或者下半月开机，天津现有的参数中td_b_feecount中的费率为半月费率，需做乘2处理。
							     //条件表中是根据状态来确定feecount中取到的是全月还是半月月租的
							{
									t_sumFee = 1.0 * t_sumFee * 2 * t_day_interval / t_acyc_end_day + 0.5;
									t_displayFee = t_sumFee;
							}
							else
							{
									t_sumFee = 1.0 * t_sumFee * t_day_interval / t_acyc_end_day + 0.5;
									t_displayFee = t_sumFee;
							}
							//Modified by Malone start 2009-11-9 23:06 end
      			}    	  		
						//上半月新装 || 下半月拆机
						else
						{
							//Modified by Malone start 2009-11-9 23:06 start
							/*
							t_sumFee = 1.0 * t_sumFee * t_day_interval / t_acyc_end_day + 0.5;
							t_displayFee = t_sumFee;
							*/
							if ( (t_svcstateMinStart > (t_pGlobalInfo->m_cycParam).m_halfTime)||
							     (t_svcstateMaxEnd < (t_pGlobalInfo->m_cycParam).m_halfTime)
							    )//上半月停机或者下半月开机，天津现有的参数中td_b_feecount中的费率为半月费率，需做乘2处理。
							     //条件表中是根据状态来确定feecount中取到的是全月还是半月月租的
							{
									t_sumFee = 1.0 * t_sumFee * 2 * t_day_interval / t_acyc_end_day + 0.5;
									t_displayFee = t_sumFee;
							}
							else
							{
									t_sumFee = 1.0 * t_sumFee * t_day_interval / t_acyc_end_day + 0.5;
									t_displayFee = t_sumFee;
							}
							//Modified by Malone start 2009-11-9 23:06 end
						}
					}
					
    	  }	
/*
    	  //新装用户
    	  else if( (t_open_date >= (t_pGlobalInfo->m_cycParam).m_beginTime)&&
    	  	       (t_open_date <= (t_pGlobalInfo->m_cycParam).m_endTime)
    	  	     )
    	  {
    	  	//下半月新装
    	  	if( t_open_date >= (t_pGlobalInfo->m_cycParam).m_halfTime )
    	  	{
    	  			//获取计费天数
    	  			if( t_compute_day < t_open_day )
    	  		  {
    	  					t_day_interval = 0;
    	  		  }
    	  		  else
    	  		  {
    	  		  		t_day_interval = t_compute_day - t_open_day + 1;
    	  		  }

    	  			if(getRentUnitRatio(t_sumFee)==false)
      				{
              		//获取汇总费用�
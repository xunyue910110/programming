#include "PriceComp.h"
#include "RateGlobalInfo.h"
#include "base/exceptions.h"

extern RateGlobalInfo g_globalInfo;
RateGlobalInfo * PriceComp::m_globalInfo = &g_globalInfo;

bool PriceComp::execute() 
{	
	char t_errmsg[128];
	bool notEffect = true;//当前节点还没有作用
	if ( m_nodeAction != NULL)
	{
		if (m_nodeType == NODE_TYPE_COND)//条件 
		{		
			if (m_nodeAction->execute())//满足条件
			{
#ifdef _DEBUG_
      cout<<"PASS--->"<<
          "NODE_ID="<<m_nodeId<<
          ",COND_ID="<<m_nodeActId<<
          ",PRICE_ID="<<m_id<<endl;
#endif								
				if (m_curNode != NULL)//条件下的子节点应该是作用
				{
					m_curNode->execute();
					notEffect = false;
				}
			}
		}
		else //作用
		{
			m_nodeAction->execute();
			if (m_curNode != NULL)
			{
				m_curNode->execute();
			}
		}
	}	
	else
	{
		sprintf(t_errmsg,"条件参数错误id:%d,m_nodeAction为空 !",m_nodeActId);
		m_globalInfo->setErrInfo(E_CONDITION_PARAM,t_errmsg);
		THROW_C_P1(BusinessException,-1,"条件参数错误id:%d,m_nodeAction为空!",
		           m_nodeActId);
	}

	PriceComp *pNext = m_next;
	while (pNext != NULL)
	{
		if (pNext->m_nodeGroup != m_nodeGroup)			
		{			
			notEffect = true;
		}
		if (notEffect)
		{
			break;
		}
		else
		{
			pNext = pNext->m_next;
		}
	}
	if (pNext != NULL)
		pNext->execute();

	return true;
}

bool PriceComp::rateExecute(vector<CdrSeg> &r_cdrSegList)
{
	bool  r_passValue=true; //条件通过情况: true 有通过、false 不通过
  bool t_splitFlag = false;//是否分割标记
  vector<CdrSeg> *t_pcdrSegList=&r_cdrSegList;
  RateGlobalInfo *t_pGlobalInfo = PriceComp::m_globalInfo;
  RateInfo       &t_rateInfo = t_pGlobalInfo->m_rateFeeInfo;  
  SumTpRule      t_sumRuleInfo;  //当前节点的总量条件信息
	char t_errmsg[128];
  t_sumRuleInfo.clear();
	if ( m_nodeAction != NULL)
	{
	  if (m_nodeType == NODE_TYPE_COND)//条件 
	  {
	    vector<CdrSeg> t_cdrSegs;
	    try
	    {
	      r_passValue = m_nodeAction->judge(&r_cdrSegList,&t_cdrSegs,t_splitFlag,t_sumRuleInfo);
	      /*
	      #ifdef _DEBUG_
	      if(t_splitFlag == true)
	      {
	        cout<<__FILE__<<__LINE__<<endl;
	        for(vector<CdrSeg>::const_iterator t_cItr=t_cdrSegs.begin();
	            t_cItr!=t_cdrSegs.end();++t_cItr)
	        {
	          cout<<*t_cItr<<endl;
	        }
	        cout<<__FILE__<<__LINE__<<endl;
	      }
	      #endif
	      */
	    }catch (const BusinessException &e)
	    {
    		sprintf(t_errmsg,"条件参数错误id:%d !",m_nodeActId);
	      #ifdef _DEBUG_
	        cout<<e.getMessage()<<" "<<__FILE__<<__LINE__<<endl;
      		cout<<"t_errmsg = "<<t_errmsg<<" "<<__FILE__<<__LINE__<<endl;
	      #endif
		    t_pGlobalInfo->setErrInfo(E_CONDITION_PARAM,t_errmsg);
		    t_pGlobalInfo->writeErrMsg(__FILE__,__LINE__,t_errmsg);
		    t_pGlobalInfo->writeErrMsg(__FILE__,__LINE__,e.getMessage().c_str());
        THROW_C_P1(BusinessException, -1, "条件参数错误id:%d !",m_nodeActId);
	    }
      t_pcdrSegList=t_splitFlag?&t_cdrSegs:&r_cdrSegList;
	    if(r_passValue == true )
	    {//不是全部不通过
  	    //为总量型可能通过
        if(t_sumRuleInfo.m_ruleList.empty()==false)
        {
          /*
          #ifdef _DEBUG_
            cout<<__FILE__<<__LINE__<<endl;
            for(vector<SumRule>::const_iterator 
                t_contItr2=t_sumRuleInfo.m_ruleList.begin();
                t_contItr2!=t_sumRuleInfo.m_ruleList.end();
                ++t_contItr2)
            {
              cout<<t_contItr2->m_billId<<","<<t_contItr2->m_curValue<<","<<
                    t_contItr2->m_minValue<<","<<t_contItr2->m_maxValue<<endl;
            }
            cout<<__FILE__<<__LINE__<<endl;
          #endif
          */
          //则将总量条件压入
          t_rateInfo.m_sumRuleList.push_back(t_sumRuleInfo);
        }
	      if(m_curNode != NULL)
        { //执行子节点动作
          m_curNode->rateExecute(*t_pcdrSegList);
        }else
        { //条件下，无子节点，判为异常
      		sprintf(t_errmsg,"条件节点:%d 下未定义子节点!",m_nodeId);
      		t_pGlobalInfo->setErrInfo(E_PRICECOMPPARAM,t_errmsg);
      		//抛出异常throw();
      		THROW_C_P1(BusinessException, -1, "资费构成条件配置错误,cond_id=%d", m_nodeActId);
      		return false;
        }
        //总量型可能通过
        if(t_sumRuleInfo.m_ruleList.empty()==false)
        {
          //则将总量条件抛出
          t_rateInfo.m_sumRuleList.pop_back();
        }
      }
	  }else 
	  {
	    if(m_nodeType == NODE_TYPE_FEECOUNT) //费用计算
  	  {
  	    m_nodeAction->rateExecute(r_cdrSegList);
  	  }else //累计
  	  {
        m_nodeAction->rateExecute(r_cdrSegList);
      }
    	if(m_curNode != NULL)
      { //执行子节点动作
        m_curNode->rateExecute(*t_pcdrSegList);
      }
    }
	}else
	{
		//error deal
		sprintf(t_errmsg,"批价作用或条件节点:%d 未定义!",m_nodeId);
		t_pGlobalInfo->setErrInfo(E_PRICECOMPPARAM,t_errmsg);
		//抛出异常throw(); 
		THROW_C_P1(BusinessException, -1, "批价作用或条件节点:%d 未定义", m_nodeId);
		return false;
	}
  //找到同层次的下个要执行的节点
	PriceComp *pNext = m_next;
	while (pNext != NULL)
	{
		if (pNext->m_nodeGroup == m_nodeGroup &&
		    r_passValue == true &&
		    m_nodeType  == NODE_TYPE_COND &&
		    pNext->m_nodeType ==  NODE_TYPE_COND &&
		    t_splitFlag == false  &&
		    t_sumRuleInfo.m_ruleList.empty()==true)			
		{	
		  //只有节点为条件（非分割、非总量）的情况下
		  //如果同组并且通过 		
		  //当前节点全部通过，则同组的其他节点不需再执行
			pNext = pNext->m_next;
		}else break;
	}
	//执行同层次的下个节点
	if(pNext != NULL)
	{
	  pNext->rateExecute(r_cdrSegList);
	}
	return true;
}


void PriceComp::dump(char *prefix)
{
	char myPrefix[256] = "  ";		
	strcat(myPrefix,prefix);
		
	cout <<myPrefix<<"{" <<endl;
	cout <<myPrefix<<"----PriceComp : "<<m_nodeId<<" ----" <<endl;
	cout <<myPrefix<<"m_nodeId       = "<< m_nodeId<<endl;		
	cout <<myPrefix<<"m_nodeType     = "<< m_nodeType <<endl;
	cout <<myPrefix<<"m_nodeActId    = "<< m_nodeActId<<endl;
	cout <<myPrefix<<"m_nodeGroup    = "<< m_nodeGroup<<endl;
	cout <<myPrefix<<"m_parentNode   = "<< m_parentNode<<endl;
	cout <<myPrefix<<"m_id(price_id) = "<< m_id <<endl;	
	cout <<myPrefix<<"m_orderNo      = "<< m_orderNo<<endl;
	
	if (m_nodeAction != 0)
	{
		cout <<myPrefix<<"m_nodeAction   = "<< m_nodeAction->m_id <<endl;	
		m_nodeAction->dump(myPrefix);   
	}
	else
		cout <<myPrefix<<"m_nodeAction   = NULL"<<endl;	
	
	if (m_curNode != 0)
	{
		cout <<myPrefix<<"m_curNode      = "<< m_curNode->m_parentNode<<endl;	
		m_curNode->dump(myPrefix);   
	}
	else
		cout <<myPrefix<<"m_curNode      = NULL"<<endl;

	if (m_next != 0)
	{
		cout <<myPrefix<<"m_next         = "<<&(m_next->m_curNode)<<endl;	
		m_next->dump(myPrefix);   
	}
	else
		cout <<myPrefix<<"m_next         = NULL"<<endl;	
	cout <<myPrefix<<"}" <<endl;
}

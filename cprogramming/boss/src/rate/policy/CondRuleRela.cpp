#include "CondRuleRela.h"
#include "base/exceptions.h"
#include "RateGlobalInfo.h"

bool CondRuleRela::execute() 
{
	bool result = true;
	
	if(m_condRule != 0)
	{
		result = m_condRule->execute();
	}else
  {
		THROW_C_P1(BusinessException, -1, "条件配置错误,cond_id=%d", (long)m_id);
  }
	CondRuleRela* pCur = (CondRuleRela*)this;
	CondRuleRela* pNext = m_next;
	//找到下一个要执行的next节点
	while(pNext!=NULL)
	{
	  if(pCur->m_andOrTag == '0' ) //and 
	  {
	    if(result == true)
	    { 
	      break;
	    }else
	    { //跳过
	    }
	  }else //if(pCur->m_andOrTag == '1') //or
	  {
	    if(result == false)
	    {
	      break;
	    }else
	    { //跳过
	    }
	  }
		pCur = pNext;
		pNext = pNext->m_next;
	};//找到下一个要执行的节点完毕
	//执行 pNext
	if(pNext != NULL)
	{
	  result = pNext->execute();
	}
  return 	result;
}

/* 判断条件是否通过
 *@param  r_pCdrSegList     输入的话单段列表
 *@param  r_pSepCdrSegList  如果部分通过，则存放通过的话单段列表
 *@param  r_splitFlag       是否分割标记
 *@param  r_sumRuleInfo     存放总量条件信息
 *@return true 通过,	   
 *        false 不通过
 */
bool CondRuleRela::judge(vector<CdrSeg> *r_pCdrSegList,
	                       vector<CdrSeg> *r_pSepCdrSegList,
	                       bool           &r_splitFlag,
	                       SumTpRule      &r_sumRuleInfo)
{
	bool result = true;
	bool t_splitFlag = false;
	
	r_splitFlag = false;
	vector<CdrSeg> t_CdrSegList[2];//话单段
	int            t_ouputIdx=1;
	vector<CdrSeg> *pInputCdrSeg  = r_pCdrSegList; 
	vector<CdrSeg> *pOutputCdrSeg = &(t_CdrSegList[t_ouputIdx]);
	CondRuleRela   *pCur,*pNext;
	
	if(m_condRule != 0)
	{	//执行第一个子条件	
		result = m_condRule->judge(pInputCdrSeg, pOutputCdrSeg,
		                           t_splitFlag , r_sumRuleInfo);
    if(result == true && t_splitFlag == true) //分割
    {
      pInputCdrSeg = pOutputCdrSeg;
      t_ouputIdx   = (t_ouputIdx+1)%2;
      pOutputCdrSeg= &(t_CdrSegList[t_ouputIdx]);
      r_splitFlag  = true;
    }  
	}else
  {
		THROW_C_P1(BusinessException, -1, "条件配置错误,cond_id=%d", (long)m_id);
  }
	
	//找到下一个要执行的next节点
	pCur = (CondRuleRela*)this;
	pNext = m_next;
	while(pNext!=NULL)
	{
	  if(pCur->m_andOrTag == '0' ) //and 
	  {
	    if(result == true)
	    { 
	      break;
	    }else
	    { //跳过
	    }
	  }else //if(pCur->m_andOrTag == '1') //or
	  {
	    if(result == false)
	    {
	      break;
	    }else
	    { //跳过
	    }
	  }
		pCur = pNext;
		pNext = pNext->m_next;
	};//找到下一个要执行的节点完毕
	
	//执行 pNext
	if(pNext != NULL)
	{
	  result = pNext->judge(pInputCdrSeg, pOutputCdrSeg,
                 		      t_splitFlag , r_sumRuleInfo);
    if(result == true && t_splitFlag == true) //分割
    {
      pInputCdrSeg = pOutputCdrSeg;
      t_ouputIdx   = (t_ouputIdx+1)%2;
      pOutputCdrSeg= &(t_CdrSegList[t_ouputIdx]);
      r_splitFlag  = true;
    }  
	}
	if(r_splitFlag == true && result==true)
  {
    *r_pSepCdrSegList = *pInputCdrSeg;
  }else
  {
    r_splitFlag = false;
  }
	return result;
}

void CondRuleRela::dump(char *prefix)
{	
	char myPrefix[256] = "  ";
	strcat(myPrefix,prefix);
	cout <<myPrefix<<"{" <<endl;
	cout <<myPrefix<<"----CondRuleRela : " <<m_id<<" subCondId:"<<m_condId <<" ----" <<endl;	
	cout <<myPrefix<<"m_condId   = " <<m_condId<<endl;
	cout <<myPrefix<<"m_andOrTag = " <<m_andOrTag<<endl;

	if(m_condRule != 0)
	{		
		cout <<myPrefix<<"m_condRule = " <<m_condRule->m_id<<endl;
		m_condRule->dump(myPrefix);
	}
	else
	    cout <<myPrefix<<"m_condRule = NULL"<<endl;

	CondRuleRela *pNext = m_next;
	if (pNext != 0)
	{
		cout <<myPrefix<<"m_next     = "<<m_next->m_condId<<endl;
		pNext->dump(myPrefix);
		pNext = pNext->m_next;
	}
	else
	    cout <<myPrefix<<"m_next     = NULL"<<endl;
	cout <<myPrefix<<"}" <<endl;
}

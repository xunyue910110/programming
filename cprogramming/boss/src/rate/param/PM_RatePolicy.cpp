#include "PM_Rate.h"

//资费参数管理接口
#include "PO_FeePolicyComp.h" 
#include "PO_EventFeePolicy.h"
#include "PO_Price.h"		 
#include "PO_PriceExtend.h"		 
#include "PO_PriceComp.h"	 
#include "PO_PriceParamImpl.h"  
#include "PO_CondRuleRela.h"  
#include "PO_CondRuleScope.h" 
#include "PO_CondRuleScopeL.h" 
#include "PO_CondRuleScopeR.h" 
#include "PO_CondRuleScopeLR.h" 
#include "PO_CondRuleIn.h" 
#include "PO_CondRuleExists.h" 
#include "PO_Condition.h" 
#include "PO_EffectDiscnt.h"  
#include "PO_EffectAddup.h"  
#include "PO_EffectFeeCount.h"  
#include "PO_ObjectBill.h"  
#include "PO_ObjectCdr.h"  
#include "PO_ObjectAddup.h"   
#include "PO_ObjectUser.h"  
#include "PO_Object.h" 
#include "PO_AddupItem.h"  
#include "PO_AddupCycleRule.h" 
#include "PO_EventElem.h"    
#include "PO_InfoElem.h" 
#include "PO_Item.h"  
#include "PO_CompositItem.h" 
#include "CompositItemMgr.h"

//获取帐目管理器指针指针    
CompositItemMgr * PM_Rate::getCompositItemMgr()
{
	return m_pCompositItemMgr;
} 

CondBase * PM_Rate::getCondition(const T_RULE_ID &condId)
{
	PO_Condition po;
	po.m_record.m_id = condId;
	PO_Condition *rpo;
	if(m_PO_Condition->search(po,rpo)>0)	
	{
		return rpo->m_record.m_cond;
	}		
	return NULL;
}

//根据对象编码返回对象指针
ObjectBase * PM_Rate::getObjectByObjId(const T_RULE_ID &objId)
{
	PO_Object po;
	po.m_record.m_id = objId;
	PO_Object *rpo;
	if(m_PO_Object->search(po,rpo)>0)	
	{
		return rpo->m_record.m_objectBase;
	}		
	return NULL;
}    

FeePolicyComp * PM_Rate::getFeePolicy(const T_TP_ID &feePolicyId)
{	
	PO_FeePolicyComp po;
	po.m_record.m_id = feePolicyId;
	po.m_record.m_eventTypeId = 0;
	po.m_record.m_priority = 0;
	
	PO_FeePolicyComp *rpo;
	if(m_PO_FeePolicyComp->search(po,rpo)>0)	
	{
		return & (rpo->m_record);
	}
			
	return NULL;	
}

//根据批价政策编码返回批价政策指针
Price * PM_Rate::getPrice(const int &priceId)
{
	PO_Price po;
	po.m_record.m_id = priceId;
	
	PO_Price *rpo;
	if(m_PO_Price->search(po,rpo)>0)	
	{
		return & (rpo->m_record);
	}		
	return NULL;	
}

//根据资费构成标识返回批价参数实现指针
EventFeePolicy * PM_Rate::getEventFeePolicy(const int &eventFeePolicyId)
{
	PO_EventFeePolicy po;
	po.m_record.m_id = eventFeePolicyId;
	po.m_record.m_feeId = 0;
	
	PO_EventFeePolicy *rpo;
	if (m_PO_EventFeePolicy->search(po,rpo)>0)	
	{
		return & (rpo->m_record);
	}		
	return NULL;	
}
	
//根据资费构成标识返回批价参数实现指针
PriceParamImpl * PM_Rate::getPriceParamImpl(const int &compId)
{
	PO_PriceParamImpl po;
	po.m_record.m_id = compId;	
	po.m_record.m_priceParamId = 0;	
	
	PO_PriceParamImpl *rpo;
	if(m_PO_PriceParamImpl->search(po,rpo)>0)	
	{
		return & (rpo->m_record);
	}		
	return NULL;	
}

//根据批价政策标识返回批价政策构成指针
PriceComp * PM_Rate::getPriceComp(const int &priceId)
{
	PO_PriceComp po;
	po.m_record.m_id = priceId;
	po.m_record.m_parentNode = 0;
	po.m_record.m_nodeGroup = 0;
	po.m_record.m_orderNo = 0;
	
	PO_PriceComp *rpo;
	if(m_PO_PriceComp->search(po,rpo)>0)	
	{
		return & (rpo->m_record);
	}

	return NULL;	
}

//根据批价政策标识返回批价政策扩展指针
PriceExtend * PM_Rate::getPriceExtend(const int &priceId)
{
	PO_PriceExtend po;
	po.m_record.m_id = priceId;	
	
	PO_PriceExtend *rpo;
	if(m_PO_PriceExtend->search(po,rpo)>0)	
	{
		return & (rpo->m_record);
	}

	return NULL;	
}

//建立外部关联-组合账目
void PM_Rate::brCompositItem()
{
	vector<PO_CompositItem> & vCompositItem = 
							 m_PO_CompositItem->getParamVector();
							 
	PO_Item po;
	PO_Item *rpo;	
	vector<PO_CompositItem>::iterator iter = vCompositItem.begin();	
	for ( ; iter!=vCompositItem.end(); ++iter)
	{
		po.m_record.m_id = (*iter).m_record.m_subItemId;
		//查询到记录
		if (m_PO_Item->search(po,rpo)>0)
		{	   
			(*iter).m_record.m_itemType = rpo->m_record.m_itemType;
		}
		else
		{
#ifdef _DEBUG_			
cout <<"Error:CompositItem="<<(*iter).m_record.m_id<<"  m_subItemId="<<(*iter).m_record.m_subItemId<<
"  在m_PO_Item中没有定义"<<endl;
#endif
		}
	}
}

//建立外部关联-账单对象
void PM_Rate::brObjectBill()
{
	vector<PO_ObjectBill> & vObjectBill = 
							 m_PO_ObjectBill->getParamVector();
							 
	PO_Item po;
	PO_Item *rpo;	
	vector<PO_ObjectBill>::iterator iter = vObjectBill.begin();	
	for ( ; iter!=vObjectBill.end(); ++iter)
	{
		po.m_record.m_id = (*iter).m_record.m_attrId;
		//查询到记录
		if (m_PO_Item->search(po,rpo)>0)
		{	   
			(*iter).m_record.m_itemType = rpo->m_record.m_itemType;
		}
		else
		{
#ifdef _DEBUG_			
cout <<"Error:ObjectBill="<<(*iter).m_record.m_id<<"  m_attrId="<<(*iter).m_record.m_attrId<<
"  在帐目定义表中没有定义"<<endl;
#endif
		}
	}
}

//建立外部关联-累计对象
void PM_Rate::brObjectAddup()
{
	vector<PO_ObjectAddup> & vObjectAddup = 
							 m_PO_ObjectAddup->getParamVector();
							 
	PO_AddupItem po;
	PO_AddupItem *rpo;	
	vector<PO_ObjectAddup>::iterator iter = vObjectAddup.begin();	
	for ( ; iter!=vObjectAddup.end(); ++iter)
	{
		po.m_record.m_id = (*iter).m_record.m_attrId;
		//查询到记录
		if (m_PO_AddupItem->search(po,rpo)>0)
		{	   
			(*iter).m_record.m_addupItem = &(rpo->m_record);
		}
		else
		{
			(*iter).m_record.m_addupItem = NULL;
#ifdef _DEBUG_
cout <<"Error:ObjectAddup="<<(*iter).m_record.m_id<<"  m_attrId="<<(*iter).m_record.m_attrId
<<"  在m_PO_AddupItem中没有定义"<<endl;
#endif
		}
	}
}

//建立外部关联-清单对象
void PM_Rate::brObjectCdr()
{
	vector<PO_ObjectCdr> & vObjectCdr = 
							 m_PO_ObjectCdr->getParamVector();
							 
	PO_ElemEvent po;
	PO_ElemEvent *rpo;	
	vector<PO_ObjectCdr>::iterator iter = vObjectCdr.begin();	
	for ( ; iter!=vObjectCdr.end(); ++iter)
	{
		po.m_record.m_id = (*iter).m_record.m_attrId;
		//查询到记录
		if (m_PO_ElemEvent->search(po,rpo)>0)
		{	   
			(*iter).m_record.m_key = rpo->m_record.m_eventElemKey;
		}
		else
		{
			(*iter).m_record.m_key = -1;
#ifdef _DEBUG_
cout <<"Error:ObjectCdr="<<(*iter).m_record.m_id<<"  m_attrId="<<(*iter).m_record.m_attrId
<<"  在m_PO_ElemEvent中没有定义"<<endl;
#endif
		}
	}
}

//建立外部关联-资料对象
void PM_Rate::brObjectUser()
{
	vector<PO_ObjectUser> & vObjectUser = 
							 m_PO_ObjectUser->getParamVector();
							 
	PO_ElemInfo po;
	PO_ElemInfo *rpo;	
	vector<PO_ObjectUser>::iterator iter = vObjectUser.begin();	
	for ( ; iter!=vObjectUser.end(); ++iter)
	{
		po.m_record.m_id = (*iter).m_record.m_attrId;
		//查询到记录
		if (m_PO_ElemInfo->search(po,rpo)>0)
		{	   
			(*iter).m_record.m_key = rpo->m_record.m_infoElemKey;
		}
		else
		{
			(*iter).m_record.m_key = -1;
#ifdef _DEBUG_
cout <<"Error:ObjectUser="<<(*iter).m_record.m_id<<"  m_attrId="<<(*iter).m_record.m_attrId
<<"  在m_PO_ElemInfo中没有定义"<<endl;
#endif
		}
	}
}


//建立外部关联-条件索引
void PM_Rate::brCondition()
{
	PO_Condition po;
	PO_Condition *rpo;
	
	//PO_CondRuleScope
	vector<PO_CondRuleScope> & vCondRuleScope = 
							 m_PO_CondRuleScope->getParamVector();							 
	
	vector<PO_CondRuleScope>::iterator iterScope = vCondRuleScope.begin();	
	for ( ; iterScope!=vCondRuleScope.end(); ++iterScope)
	{
		po.m_record.m_id = (*iterScope).m_record.m_id;
		if (m_PO_Condition->search(po, rpo)>0)
		{
			rpo->m_record.m_cond = &(*iterScope).m_record;
		}
		else
		{
#ifdef _DEBUG_			
cout <<"Error:简单条件()="<<(*iterScope).m_record.m_id<<"  在条件定义表中没有定义"<<endl;
#endif
		}
	}
	
	//PO_CondRuleScopeL
	vector<PO_CondRuleScopeL> & vCondRuleScopeL = 
							 m_PO_CondRuleScopeL->getParamVector();							 
	
	vector<PO_CondRuleScopeL>::iterator iterScopeL = vCondRuleScopeL.begin();	
	for ( ; iterScopeL!=vCondRuleScopeL.end(); ++iterScopeL)
	{
		po.m_record.m_id = (*iterScopeL).m_record.m_id;
		if (m_PO_Condition->search(po, rpo)>0)
		{
			rpo->m_record.m_cond = &(*iterScopeL).m_record;
		}
		else
		{
#ifdef _DEBUG_			
cout <<"Error:简单条件[)="<<(*iterScopeL).m_record.m_id<<"  在条件定义表中没有定义"<<endl;
#endif
		}
	}
	
	//PO_CondRuleScopeR
	vector<PO_CondRuleScopeR> & vCondRuleScopeR = 
							 m_PO_CondRuleScopeR->getParamVector();							 
	
	vector<PO_CondRuleScopeR>::iterator iterScopeR = vCondRuleScopeR.begin();	
	for ( ; iterScopeR!=vCondRuleScopeR.end(); ++iterScopeR)
	{
		po.m_record.m_id = (*iterScopeR).m_record.m_id;
		if (m_PO_Condition->search(po, rpo)>0)
		{
			rpo->m_record.m_cond = &(*iterScopeR).m_record;
		}
		else
		{
#ifdef _DEBUG_			
cout <<"Error:简单条件(]="<<(*iterScopeR).m_record.m_id<<"  在条件定义表中没有定义"<<endl;
#endif
		}
	}
	
	//PO_CondRuleScopeLR
	vector<PO_CondRuleScopeLR> & vCondRuleScopeLR = 
							 m_PO_CondRuleScopeLR->getParamVector();							 
	
	vector<PO_CondRuleScopeLR>::iterator iterScopeLR = vCondRuleScopeLR.begin();	
	for ( ; iterScopeLR!=vCondRuleScopeLR.end(); ++iterScopeLR)
	{
		po.m_record.m_id = (*iterScopeLR).m_record.m_id;
		if (m_PO_Condition->search(po, rpo)>0)
		{
			rpo->m_record.m_cond = &(*iterScopeLR).m_record;
		}
		else
		{
#ifdef _DEBUG_			
cout <<"Error:简单条件[]="<<(*iterScopeLR).m_record.m_id<<"  在条件定义表中没有定义"<<endl;
#endif
		}
	}
	
	//PO_CondRuleIn
	vector<PO_CondRuleIn> & vCondRuleIn = 
							 m_PO_CondRuleIn->getParamVector();							 
	
	vector<PO_CondRuleIn>::iterator iterIn = vCondRuleIn.begin();	
	for ( ; iterIn!=vCondRuleIn.end(); ++iterIn)
	{
		po.m_record.m_id = (*iterIn).m_record.m_id;
		if (m_PO_Condition->search(po, rpo)>0)
		{
			rpo->m_record.m_cond = &(*iterIn).m_record;
		}
		else
		{
#ifdef _DEBUG_			
cout <<"Error:简单条件IN="<<(*iterIn).m_record.m_id<<"  在条件定义表中没有定义"<<endl;
#endif
		}
	}
	
	//PO_CondRuleExists
	vector<PO_CondRuleExists> & vCondRuleExists = 
							 m_PO_CondRuleExists->getParamVector();							 
	
	vector<PO_CondRuleExists>::iterator iterExists = vCondRuleExists.begin();	
	for ( ; iterExists!=vCondRuleExists.end(); ++iterExists)
	{
		po.m_record.m_id = (*iterExists).m_record.m_id;
		if (m_PO_Condition->search(po, rpo)>0)
		{
			rpo->m_record.m_cond = &(*iterExists).m_record;
		}
		else
		{
#ifdef _DEBUG_
cout <<"Error:简单条件Exists="<<(*iterExists).m_record.m_id<<"  在条件定义表中没有定义"<<endl;
#endif
		}
	}
		
	//PO_CondRuleRela
	vector<PO_CondRuleRela> & vCondRuleRela = 
							 m_PO_CondRuleRela->getParamVector();							 
	
	int condId = 0;
	vector<PO_CondRuleRela>::iterator iterRela = vCondRuleRela.begin();	
	for ( ; iterRela!=vCondRuleRela.end(); ++iterRela)
	{
		if (condId == (*iterRela).m_record.m_id)
			continue;
		else
			condId = (*iterRela).m_record.m_id;
			
		po.m_record.m_id = condId;
		if (m_PO_Condition->search(po, rpo)>0)
		{
			rpo->m_record.m_cond = &(*iterRela).m_record;
		}
		else
		{
#ifdef _DEBUG_
cout <<"Error:组合条件="<<(*iterRela).m_record.m_id<<"  在条件定义表中没有定义"<<endl;
#endif
		}
	}
}

//建立外部关联-对象索引
void PM_Rate::brObject()
{
	PO_Object po;
	PO_Object *rpo;
	
	//PO_ObjectBill
	vector<PO_ObjectBill> & vObjectBill = m_PO_ObjectBill->getParamVector();	
	vector<PO_ObjectBill>::iterator iterBill = vObjectBill.begin();	
	for ( ; iterBill!=vObjectBill.end(); ++iterBill)
	{
		po.m_record.m_id = (*iterBill).m_record.m_id;
		if (m_PO_Object->search(po, rpo)>0)
		{
			rpo->m_record.m_objectBase = &(*iterBill).m_record;
		}		
		else
		{
#ifdef _DEBUG_
cout <<"Error:BILL对象="<<(*iterBill).m_record.m_id<<"  在对象定义表中没有定义"<<endl;
#endif
		}
	}
	
	//PO_ObjectCdr
	vector<PO_ObjectCdr> & vObjectCdr = m_PO_ObjectCdr->getParamVector();	
	vector<PO_ObjectCdr>::iterator iterCdr = vObjectCdr.begin();	
	for ( ; iterCdr!=vObjectCdr.end(); ++iterCdr)
	{
		po.m_record.m_id = (*iterCdr).m_record.m_id;
		if (m_PO_Object->search(po, rpo)>0)
		{
			rpo->m_record.m_objectBase = &(*iterCdr).m_record;
		} 
		else
		{
#ifdef _DEBUG_
cout <<"Error:CDR对象="<<(*iterCdr).m_record.m_id<<"  在对象定义表中没有定义"<<endl;
#endif
		}
	}
	
	//PO_ObjectUser
	vector<PO_ObjectUser> & vObjectUser = m_PO_ObjectUser->getParamVector();	
	vector<PO_ObjectUser>::iterator iterUser = vObjectUser.begin();	
	for ( ; iterUser!=vObjectUser.end(); ++iterUser)
	{
		po.m_record.m_id = (*iterUser).m_record.m_id;
		if (m_PO_Object->search(po, rpo)>0)
		{
			rpo->m_record.m_objectBase = &(*iterUser).m_record;
		} 
		else
		{
#ifdef _DEBUG_
cout <<"Error:USER对象="<<(*iterUser).m_record.m_id<<"  在对象定义表中没有定义"<<endl;
#endif
		}
	}
	
	//PO_ObjectAddup
	vector<PO_ObjectAddup> & vObjectAddup = m_PO_ObjectAddup->getParamVector();	
	vector<PO_ObjectAddup>::iterator iterAddup = vObjectAddup.begin();	
	for ( ; iterAddup!=vObjectAddup.end(); ++iterAddup)
	{
		po.m_record.m_id = (*iterAddup).m_record.m_id;
		if (m_PO_Object->search(po, rpo)>0)
		{
	  	  rpo->m_record.m_objectBase = &(*iterAddup).m_record;
		}	 
		else
		{
#ifdef _DEBUG_
cout <<"Error:ADDUP对象="<<(*iterAddup).m_record.m_id<<"  在对象定义表中没有定义"<<endl;
#endif
		}
	}	
}

//建立外部关联-组合条件(依赖brCondition)
void PM_Rate::brCondRuleRela()
{
	PO_Condition po;
	PO_Condition *rpo;
	
	vector<PO_CondRuleRela> & vCondRuleRela = 
							 m_PO_CondRuleRela->getParamVector();	
	
	vector<PO_CondRuleRela>::iterator iterRela = vCondRuleRela.begin();	
	for ( ; iterRela!=vCondRuleRela.end(); ++iterRela)
	{
		po.m_record.m_id = (*iterRela).m_record.m_condId;		
		if (m_PO_Condition->search(po, rpo)>0)
		{
			(*iterRela).m_record.m_condRule = rpo->m_record.m_cond;
		} 
		else
		{
			(*iterRela).m_record.m_condRule = NULL;
#ifdef _DEBUG_
cout <<"Error:组合条件="<<(*iterRela).m_record.m_id<<"  子条件="<<
       (*iterRela).m_record.m_condId<<"  在条件定义表中没有定义"<<endl;
#endif       
		}
	}
}

//建立外部关联-资费政策构成
void PM_Rate::brFeePolicyComp()
{
	vector<PO_FeePolicyComp> & vFeePolicyComp = 
							 m_PO_FeePolicyComp->getParamVector();			
	
	vector<PO_FeePolicyComp>::iterator iter = vFeePolicyComp.begin();	
	for ( ; iter!=vFeePolicyComp.end(); ++iter)
	{
		(*iter).m_record.m_eventFeePolicy = 
		               getEventFeePolicy((*iter).m_record.m_eventPolicyId);
		(*iter).m_record.m_priceParamImpl = 
							getPriceParamImpl((*iter).m_record.m_compId);
						
							
		if((*iter).m_record.m_eventFeePolicy == NULL)
		{
#ifdef _DEBUG_
cout <<"Error:FeePolicyComp="<<(*iter).m_record.m_id<<"  m_eventFeePolicy="<<
       (*iter).m_record.m_eventPolicyId<<"  在m_PO_EventFeePolicy中没有定义"<<endl;
#endif
		}
		
		if((*iter).m_record.m_priceParamImpl == NULL)
		{
#ifdef _DEBUG_
//cout <<"Error:FeePolicyComp="<<(*iter).m_record.m_id<<"  m_priceParamImpl="<<
//       (*iter).m_record.m_compId<<"  在m_PO_PriceParamImpl中没有定义"<<endl;
#endif
		}
	}
}

//建立外部关联-事件资费政策
void PM_Rate::brEventFeePolicy()
{
	vector<PO_EventFeePolicy> & vEventFeePolicy = 
							 m_PO_EventFeePolicy->getParamVector();			

	vector<PO_EventFeePolicy>::iterator iter = vEventFeePolicy.begin();	
	for ( ; iter!=vEventFeePolicy.end(); ++iter)
	{
		(*iter).m_record.m_price = getPrice((*iter).m_record.m_priceId);		
							
		if((*iter).m_record.m_price == NULL)
		{
#ifdef _DEBUG_
cout <<"Error:EventFeePolicy="<<(*iter).m_record.m_id<<"  m_priceId="<<
       (*iter).m_record.m_priceId<<"  在m_PO_Price中没有定义"<<endl;
#endif
		}		
	}
}

//建立外部关联-批价政策
void PM_Rate::brPrice()
{
	vector<PO_Price> & vPrice = m_PO_Price->getParamVector();
		
	vector<PO_Price>::iterator iter = vPrice.begin();	
	for ( ; iter!=vPrice.end(); ++iter)
	{
		(*iter).m_record.m_priceComp = getPriceComp((*iter).m_record.m_id);
		if((*iter).m_record.m_priceComp == NULL)
		{
#ifdef _DEBUG_
cout <<"Error:Price="<<(*iter).m_record.m_id<<"  在m_PO_PriceComp中没有定义"<<endl;
#endif
		}
		
		(*iter).m_record.m_priceExtend = getPriceExtend((*iter).m_record.m_id);		
	}
}

//建立外部关联-批价政策构成
void PM_Rate::brPriceComp()
{	
	PO_EffectDiscnt poDiscnt;
	PO_EffectDiscnt *rpoDiscnt;
	
	PO_EffectFeeCount poFee;
	PO_EffectFeeCount *rpoFee;
	
	PO_EffectAddup poAddup;
	PO_EffectAddup *rpoAddup;
	
	PO_Condition poCond;	
	PO_Condition *rpoCond;	
 
	vector<PO_PriceComp> & vPriceComp = m_PO_PriceComp->getParamVector();	   
	vector<PO_PriceComp>::iterator iter = vPriceComp.begin();	
	for ( ; iter!=vPriceComp.end(); ++iter)
	{	
		if ((*iter).m_record.m_nodeType == NODE_TYPE_FEECOUNT)//费用计算
		{
			poFee.m_record.m_id = (*iter).m_record.m_nodeActId;
			
			if (m_PO_EffectFeeCount->search(poFee, rpoFee)>0)
			{
				(*iter).m_record.m_nodeAction = & (rpoFee->m_record);
			}			
			else
			{
				(*iter).m_record.m_nodeAction = NULL;
#ifdef _DEBUG_
cout <<"Error:PriceComp="<<(*iter).m_record.m_id<<"  m_nodeActId="<<(*iter).m_record.m_nodeActId
<<"  在m_PO_EffectFeeCount中没有定义"<<endl;
#endif
			}
		}
		else if ((*iter).m_record.m_nodeType == NODE_TYPE_DISCNT)//费用优惠
		{
			poDiscnt.m_record.m_id = (*iter).m_record.m_nodeActId;
			poDiscnt.m_record.m_orderNo = 0;
			
			if (m_PO_EffectDiscnt->search(poDiscnt, rpoDiscnt)>0)
			{
				(*iter).m_record.m_nodeAction = & (rpoDiscnt->m_record);
			}			
			else
			{
				(*iter).m_record.m_nodeAction = NULL;
#ifdef _DEBUG_
cout <<"Error:PriceComp="<<(*iter).m_record.m_id<<"  m_nodeActId="<<(*iter).m_record.m_nodeActId
<<"  在m_PO_EffectDiscnt中没有定义"<<endl;
#endif
			}
		}
		else if ((*iter).m_record.m_nodeType == NODE_TYPE_COND)//条件
		{			
			poCond.m_record.m_id = (*iter).m_record.m_nodeActId;
			if (m_PO_Condition->search(poCond, rpoCond)>0)
			{
				(*iter).m_record.m_nodeAction = rpoCond->m_record.m_cond;
			} 
			else
			{
				(*iter).m_record.m_nodeAction = NULL;
#ifdef _DEBUG_
cout <<"Error:PriceComp="<<(*iter).m_record.m_id<<"  m_nodeActId="<<(*iter).m_record.m_nodeActId
<<"  在m_PO_Condition中没有定义"<<endl;
#endif
			}
		}
		else if ((*iter).m_record.m_nodeType == NODE_TYPE_ADDUP)//累计
		{
			poAddup.m_record.m_id = (*iter).m_record.m_nodeActId;
			poAddup.m_record.m_orderNo = 0;
			
			if (m_PO_EffectAddup->search(poAddup, rpoAddup)>0)
			{
				(*iter).m_record.m_nodeAction = & (rpoAddup->m_record);
			}			
			else
			{
				(*iter).m_record.m_nodeAction = NULL;
#ifdef _DEBUG_
cout <<"Error:PriceComp="<<(*iter).m_record.m_id<<"  m_nodeActId="<<(*iter).m_record.m_nodeActId
<<"  在m_PO_EffectAddup中没有定义"<<endl;
#endif
			}
		}
		else
		{
			(*iter).m_record.m_nodeAction = NULL;
#ifdef _DEBUG_
cout <<"Error:PriceComp="<<(*iter).m_record.m_id<<"  m_nodeType="<<(*iter).m_record.m_nodeType
<<"没有定义"<<endl;
#endif
		}		
	}
}

void PM_Rate::brEffectAddup()                //建立外部关联-累计处理
{
	PO_AddupItem po;
	PO_AddupItem *rpo;

	long buff;
	int length = sizeof(void *);
	
	ObjectBase *effectObj;
	vector<PO_EffectAddup> &vAddup = m_PO_EffectAddup->getParamVector();
	vector<PO_EffectAddup>::iterator iterAddup = vAddup.begin();
	for ( ; iterAddup!=vAddup.end(); ++iterAddup)
	{		
		po.m_record.m_id = (*iterAddup).m_record.m_addupItemCode;
		if ( m_PO_AddupItem->search(po, rpo)>0)
		{
			(*iterAddup).m_record.m_addupItem = &(*rpo).m_record;						
		}
		else
		{
			(*iterAddup).m_record.m_addupItem = NULL;
#ifdef _DEBUG_
cout <<"Error:EffectAddup="<<(*iterAddup).m_record.m_id<<"  m_addupItemCode="<<
(*iterAddup).m_record.m_addupItemCode<<"  在m_PO_AddupItem中没有定义"<<endl;
#endif
		}		
	}  
}

void PM_Rate::brEffectFeeCount()             //建立外部关联-费用计算 
{
	PO_Object poObject;
	PO_Object *rpoObject;	

	PO_EffectAddup poAddup;
	PO_EffectAddup *rpoAddup;
	 
	long buff;
	int length = sizeof(void *);
	
	vector<PO_EffectFeeCount> &vFeeCount = m_PO_EffectFeeCount->getParamVector();
	vector<PO_EffectFeeCount>::iterator iterFeeCount = vFeeCount.begin();
	for ( ; iterFeeCount!=vFeeCount.end(); ++iterFeeCount)
	{		
		if((*iterFeeCount).m_record.m_addupRule!=NULL)//m_addupRule可以为空
		{
			memcpy((void *)&buff,(void *) &(*iterFeeCount).m_record.m_addupRule,
				   length);
			
			//从累计量对象中查找 
			poAddup.m_record.m_id = buff;
			if ( m_PO_EffectAddup->search(poAddup, rpoAddup)>0)
			{
				(*iterFeeCount).m_record.m_addupRule = & (*rpoAddup).m_record;
			}
			else
			{
				(*iterFeeCount).m_record.m_addupRule = NULL;
#ifdef _DEBUG_				
cout <<"Error:EffectFeeCount="<<(*iterFeeCount).m_record.m_id<<"  m_addupRule="<<buff
<<"  在m_PO_EffectAddup中没有定义"<<endl;
#endif
			}		
		}
	}  
}

//建立外部关联-费用优惠
void PM_Rate::brEffectDiscnt()
{	
	PO_Object poObject;
	PO_Object *rpoObject;	

	PO_ObjectBill poBill;
	PO_ObjectBill *rpoBill;	

	PO_EffectAddup poAddup;
	PO_EffectAddup *rpoAddup;
	 
	long buff;
	int length = sizeof(void *);
	
	PO_DiscntStatecode poStatecode;
	PO_DiscntStatecode *rpoStatecode;
	 
	typedef map<char,int> stateMap;
	
	ObjectBase *effectObj;
	vector<PO_EffectDiscnt> &vDiscnt = m_PO_EffectDiscnt->getParamVector();
	vector<PO_EffectDiscnt>::iterator iterDiscnt = vDiscnt.begin();
	for ( ; iterDiscnt!=vDiscnt.end(); ++iterDiscnt)
	{		
		effectObj = (*iterDiscnt).m_record.m_effectObj;
		memcpy((void *)&buff, (void*) &effectObj, length);
		poBill.m_record.m_id = buff;
		if ( m_PO_ObjectBill->search(poBill, rpoBill)>0)
		{
			(*iterDiscnt).m_record.m_effectObj = &(*rpoBill).m_record;						
		}
		else
		{
			(*iterDiscnt).m_record.m_effectObj = NULL;
#ifdef _DEBUG_
cout <<"Error:EffectDiscnt="<<(*iterDiscnt).m_record.m_id<<"  m_effectObj="<<buff
<<"  在m_PO_ObjectBill中没有定义"<<endl;
#endif
		}
		
		if((*iterDiscnt).m_record.m_computeObj!=NULL)//m_computeObj可以为空
		{
			//如果参考对象和作用对象是同一个，不需要再查找了
			if((*iterDiscnt).m_record.m_computeObj == effectObj)
			{
				(*iterDiscnt).m_record.m_computeObj = 
							 (*iterDiscnt).m_record.m_effectObj;
			}
			else
			{
				memcpy((void *) &buff, 
					   (void *) &(*iterDiscnt).m_record.m_computeObj, length);
				
				//从对象索引中查找		   		   
				poObject.m_record.m_id = buff;
				if ( m_PO_Object->search(poObject, rpoObject)>0)
				{
					(*iterDiscnt).m_record.m_computeObj = 
											(*rpoObject).m_record.m_objectBase;
				}
				else
				{
					(*iterDiscnt).m_record.m_computeObj = NULL;
#ifdef _DEBUG_
cout <<"Error:EffectDiscnt="<<(*iterDiscnt).m_record.m_id<<"  m_computeObj="<<buff
<<"  在m_PO_Object中没有定义"<<endl;
#endif
				}  
			}
		}		
		
		if((*iterDiscnt).m_record.m_baseObj!=NULL)//m_baseObj可以为空
		{
			memcpy((void *)&buff, (void*) &(*iterDiscnt).m_record.m_baseObj,
				   length);
			
			//从对象索引中查找		   		   
			poObject.m_record.m_id = buff;
			if ( m_PO_Object->search(poObject, rpoObject)>0)
			{
				(*iterDiscnt).m_record.m_baseObj = 
											(*rpoObject).m_record.m_objectBase;
			}
			else
			{
				(*iterDiscnt).m_record.m_baseObj = NULL;
#ifdef _DEBUG_
cout <<"Error:EffectDiscnt="<<(*iterDiscnt).m_record.m_id<<"  m_baseObj="<<buff
<<"  在m_PO_Object中没有定义"<<endl;
#endif
			}  
		}
		
		(*iterDiscnt).m_record.m_parentObj = NULL;
		if((*iterDiscnt).m_record.m_parentValue[0]=='*')
		{
			//从对象索引中查找		   		   
			poObject.m_record.m_id = atoi(&(*iterDiscnt).m_record.m_parentValue[1]);
			if ( m_PO_Object->search(poObject, rpoObject)>0)
			{
				(*iterDiscnt).m_record.m_parentObj=(*rpoObject).m_record.m_objectBase;
			}
			else
			{
#ifdef _DEBUG_
cout <<"Error:EffectDiscnt="<<(*iterDiscnt).m_record.m_id<<"  m_parentObj="<<
poObject.m_record.m_id<<"  在m_PO_Object中没有定义"<<endl;
#endif
			}  
		}
		
		if((*iterDiscnt).m_record.m_addupRule!=NULL)//m_addupRule可以为空
		{
			memcpy((void *)&buff,(void *) &(*iterDiscnt).m_record.m_addupRule,
				   length);
			
			//从累计量对象中查找 
			poAddup.m_record.m_id = buff;
			if ( m_PO_EffectAddup->search(poAddup, rpoAddup)>0)
			{
				(*iterDiscnt).m_record.m_addupRule = & (*rpoAddup).m_record;
			}
			else
			{
				(*iterDiscnt).m_record.m_addupRule = NULL;
#ifdef _DEBUG_
cout <<"Error:EffectDiscnt="<<(*iterDiscnt).m_record.m_id<<"  m_addupRule="<<buff
<<"  在m_PO_EffectAddup中没有定义"<<endl;
#endif
			}		
		}
		
		(*iterDiscnt).m_record.m_SvcState.clear();
		
		if((*iterDiscnt).m_record.m_computeMethod==8)//获取生效状态集合
		{	
			PO_DiscntStatecode  t_po;
  		vector<PO_DiscntStatecode*> t_prpoList;
  		t_po.m_record.m_feediscntid = (*iterDiscnt).m_record.m_id;
  
  		if(m_PO_DiscntStatecode->scanSearchMulti(t_po,t_prpoList)<=0)
  		{
#ifdef _DEBUG_
		cout <<"["<<__FILE__<<":"<<__LINE__<<"] Error:EffectDiscnt="<<(*iterDiscnt).m_record.m_id
		     <<" 在m_PO_DiscntStatecode中没有定义!"<<endl;
#endif  			
  		}
       	
			vector<PO_DiscntStatecode*>::iterator iter;
			for (iter=t_prpoList.begin(); iter!=t_prpoList.end(); ++iter) 
			{
			 if ((*iter)->m_record.m_feediscntid==(*iterDiscnt).m_record.m_id)	
				{
					//(*iterDiscnt).m_record.m_SvcState.insert(stateMap::value_type((*iter)->m_record.m_statecode,1));
				  for(vector<char>::iterator t_it =(*iter)->m_record.m_stateCodeVector.begin();
				  	  t_it!=(*iter)->m_record.m_stateCodeVector.end();t_it++)
				  	(*iterDiscnt).m_record.m_SvcState.insert(stateMap::value_type(*t_it,1));
				}
			}
			
		}
	}  
}

void PM_Rate::brAddupItem()      //建立外部关联-累计帐目
{
	vector<PO_AddupItem> & vAddupItem = 
							 m_PO_AddupItem->getParamVector();
							 
	PO_AddupCycleRule po;
	PO_AddupCycleRule *rpo;	
	vector<PO_AddupItem>::iterator iter = vAddupItem.begin();	
	for ( ; iter!=vAddupItem.end(); ++iter)
	{
		po.m_record.m_id = (*iter).m_record.m_cycleRuleId;
		//查询到记录
		if (m_PO_AddupCycleRule->search(po,rpo)>0)
		{	   
			(*iter).m_record.m_cycleRule = &(rpo->m_record);
		}
		else
		{
			(*iter).m_record.m_cycleRule = NULL;
#ifdef _DEBUG_
cout <<"Error:AddupItem="<<(*iter).m_record.m_id<<"  m_cycleRule="<<(*iter).m_record.m_cycleRuleId
<<"  在m_PO_AddupCycleRule中没有定义"<<endl;
#endif
		}
	}
}

//判断组合条件是否存  在循环定义
bool PM_Rate::isLoopDefineCondition()
{	
	vector<PO_CondRuleRela> & vCond = m_PO_CondRuleRela->getParamVector();
	vector<PO_CondRuleRela>::iterator iter=vCond.begin();
	for(; iter!=vCond.end(); ++iter)
	{		
		if ( (*iter).m_record.m_condRule != NULL)
		{
			if(dynamic_cast<CondRuleRela *>((*iter).m_record.m_condRule)!=NULL) //zhaogc modified
			//if ( ! strcmp(typeid(*(*iter).m_record.m_condRule).name(),"CondRuleRela") )
			{
				m_idSet.clear();
				m_idSet.insert((*iter).m_record.m_id);
				if (!addCondRule( (*iter).m_record.m_condId, vCond))
				{
					return true;
				}
			}
		}
	}
	return false;
}


bool	PM_Rate::addCondRule(T_RULE_ID &subCondRuleId, 
                           vector<PO_CondRuleRela> &vCond)
{
	if (m_idSet.find(subCondRuleId) != m_idSet.end())
	{
		char errorMessage[128];
		sprintf(errorMessage,"addCondRule : CondRuleRela exist loop define :condId=%ld",(long)subCondRuleId); 
		m_errorMessage=errorMessage;
		return false;
	}

	m_idSet.insert(subCondRuleId);
	
	PO_CondRuleRela po;
	po.m_record.m_id = subCondRuleId;
	po.m_record.m_orderNo = 0;

	vector<PO_CondRuleRela>::iterator itr;
	itr=lower_bound(vCond.begin(), vCond.end(),	po);
	for(;itr!=vCond.end();++itr)
	{
		if (!((*itr) == po))
			break;
		if ( (*itr).m_record.m_condRule != NULL)
		{
			if(dynamic_cast<CondRuleRela *>((*itr).m_record.m_condRule)!=NULL) //zhaogc modified
			//if ( ! strcmp(typeid(*(*itr).m_record.m_condRule).name(),"CondRuleRela") )		
			{
				if (!addCondRule((*itr).m_record.m_condId, vCond))
				{
					return false;
				}
				m_idSet.erase((*itr).m_record.m_condId);//出栈
			}
		}
	}
	return true;	
}

//判断批价政策构成是否存  在循环定义
bool PM_Rate::isLoopDefinePriceComp()
{	
	vector<PO_PriceComp> & vPriceComp = m_PO_PriceComp->getParamVector();
	vector<PO_PriceComp>::iterator iter=vPriceComp.begin();
	for(; iter!=vPriceComp.end(); ++iter)
	{
		m_idSet.clear();
		m_idSet.insert((*iter).m_record.m_parentNode);		
		if ( (*iter).m_record.m_curNode != NULL)
		{
			if (!addPriceComp((*iter).m_record.m_id, 
												(*iter).m_record.m_nodeId,
			                  vPriceComp))
			{
				return true;
			}			
		}
	}
	return false;
}

bool	PM_Rate::addPriceComp(int &priceId, int &parentNodeId, 
                           vector<PO_PriceComp> &vPriceComp)
{
	if (m_idSet.find(parentNodeId) != m_idSet.end())
	{
		char errorMessage[128];
		sprintf(errorMessage,"addPriceComp : PriceComp exist loop define :priceId=%d nodeId=%d",priceId,parentNodeId); 
		m_errorMessage=errorMessage;
		return false;
	}
	m_idSet.insert(parentNodeId);
	
	PO_PriceComp po;
	po.m_record.m_id = priceId;
	po.m_record.m_parentNode = parentNodeId;
	po.m_record.m_nodeGroup = 0;
	po.m_record.m_orderNo = 0;
	
	vector<PO_PriceComp>::iterator itr;
	itr=lower_bound(vPriceComp.begin(), vPriceComp.end(),	po);
	for(;itr!=vPriceComp.end();++itr)
	{
		if (!((*itr) == po))
			break;
		if ( (*itr).m_record.m_curNode != NULL)
		{
			if (!addPriceComp(priceId, 
												(*itr).m_record.m_nodeId, 
			                  vPriceComp))
			{
				return false;
			}
		}
	}
	return true;	
}

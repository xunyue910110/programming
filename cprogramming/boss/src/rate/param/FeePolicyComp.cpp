#include "FeePolicyComp.h"
#include "EventFeePolicy.h"
#include "NodeBase.h"
#include "PolicyInfo.h"

PolicyInfo * FeePolicyComp::m_policyInfo = &NodeBase::m_policyInfo;


bool FeePolicyComp::execute()
{
	m_policyInfo->m_feePolicyComp = this;

	if(m_eventFeePolicy != 0)	
		m_eventFeePolicy->execute();
			
	return true;
}

void FeePolicyComp::dump() 
{
	cout <<"------------ FeePolicyComp : " <<  m_compId <<" --------------" <<endl;
	cout <<"{" <<endl;
	cout <<"m_id             = "<< m_id <<endl;                        
	cout <<"m_priority       = "<< m_priority<<endl;	
	cout <<"m_compId         = "<< m_compId<<endl;	                  
	cout <<"m_roleCode       = "<< m_roleCode<<endl; 
	cout <<"m_eventTypeId    = "<< m_eventTypeId<<endl;
	cout <<"m_eventPolicyId	 = "<< m_eventPolicyId<<endl; 
	cout <<"m_execMode       = "<< m_execMode<<endl;
	cout <<"m_cdrCompFlag	 = "<< m_cdrCompFlag<<endl; 	
	  
	if(m_priceParamImpl == 0)
		cout <<"m_priceParamImpl = NULL"<<endl;   
	else
	{
		int order = 0;
		cout <<"m_priceParamImpl != NULL"<<endl; 
		cout <<"  {" <<endl; 
		m_priceParamImpl->dump(order);
		cout <<"  }" <<endl;
	}

	if(m_eventFeePolicy == 0)
		cout <<"m_eventFeePolicy = NULL"<<endl;   
	else
	{
		cout <<"m_eventFeePolicy != NULL"<<endl;   
		m_eventFeePolicy->dump("");
	}
	cout <<"}" <<endl;
}

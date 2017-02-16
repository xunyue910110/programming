#include "EventFeePolicy.h"
#include "Price.h"
#include "NodeBase.h"
#include "PolicyInfo.h"

PolicyInfo * EventFeePolicy::m_policyInfo = &NodeBase::m_policyInfo;


bool EventFeePolicy::execute()
{
	if (m_price != 0)	
		m_price->execute();
	
	if (m_next != 0)
		m_next->execute();
					
	return true;
}

void EventFeePolicy::dump(char *prefix) 
{
	char myPrefix[256] = "  ";
	strcat(myPrefix, prefix);	
	cout <<myPrefix<<"{" <<endl;
	cout <<myPrefix<<"---- EventFeePolicy : " <<  m_id <<" ----" <<endl;
	cout <<myPrefix<<"m_id       = "<< m_id <<endl;                        
	cout <<myPrefix<<"m_feeId    = "<< m_feeId<<endl;	
	cout <<myPrefix<<"m_rateTime = "<< m_rateTime<<endl;	                  
	cout <<myPrefix<<"m_priceId  = "<< m_priceId<<endl; 	
	  
	if(m_priceId == 0)
		cout <<myPrefix<<"m_price    = NULL"<<endl;   
	else
	{
		cout <<myPrefix<<"m_price    != NULL"<<endl;   
		m_price->dump(myPrefix);
	}
	
	if (m_next == 0)
		cout <<myPrefix<<"m_next    = NULL"<<endl; 
	else
	{
		cout <<myPrefix<<"m_next    != NULL"<<endl;   
		m_next->dump(myPrefix);
	}
	
	cout <<myPrefix<<"}" <<endl;
}

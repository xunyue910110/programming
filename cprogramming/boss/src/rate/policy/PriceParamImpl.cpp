#include "PriceParamImpl.h"
#include <iostream.h>

void PriceParamImpl::dump(int &order)
{
	++order;
	char myPrefix[] = "    ";	
	cout <<myPrefix<<"---- PriceParamImpl " <<order<<" ----" <<endl;	
	cout <<myPrefix<<"{" <<endl;	
	cout <<myPrefix<<"m_id               = "<< m_id <<endl;                        
	cout <<myPrefix<<"m_priceParamId     = "<< m_priceParamId<<endl;
	cout <<myPrefix<<"m_feePolicyParamId = "<< m_feePolicyParamId<<endl;
	cout <<myPrefix<<"m_implMethod       = "<< m_implMethod<<endl;
	cout <<myPrefix<<"m_paramValueType   = "<< m_paramValueType<<endl;
	cout <<myPrefix<<"m_fixedValue       = "<< m_fixedValue<<endl;
	cout <<myPrefix<<"}" <<endl; 
	if(m_next != NULL)                      
    	m_next->dump(order);    
}

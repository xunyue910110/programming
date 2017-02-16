#include "PriceExtend.h"

void PriceExtend::dump(char *prefix,int &order)
{
	++order;
	char myPrefix[256] = "    ";	
	strcat(myPrefix,prefix);
	cout <<myPrefix<<"---- PriceExtend " <<order<<" ----" <<endl;	
	cout <<myPrefix<<"{" <<endl;	
	cout <<myPrefix<<"m_id           = "<< m_id <<endl;                        
	cout <<myPrefix<<"m_extendId     = "<< m_extendId<<endl;
	cout <<myPrefix<<"m_extendType   = "<< m_extendType<<endl;
	cout <<myPrefix<<"m_extendValue1 = "<< m_extendValue1<<endl;
	cout <<myPrefix<<"m_extendValue2 = "<< m_extendValue2<<endl;
	cout <<myPrefix<<"m_extendValue3 = "<< m_extendValue3<<endl;
	cout <<myPrefix<<"}" <<endl; 
	if(m_next != NULL)                      
    	m_next->dump(prefix, order);
}

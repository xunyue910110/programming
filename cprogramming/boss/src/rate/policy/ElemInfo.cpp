#include "ElemInfo.h"
#include <iostream.h>

void ElemInfo::dump(char *prefix)
{
	char myPrefix[256] = "  ";
	strcat(myPrefix,prefix);
	cout <<myPrefix<<"{" <<endl;
	cout <<myPrefix<<"---- ElemInfo : " <<m_id<<" ----" <<endl;
	cout <<myPrefix<<"m_id          = "<< m_id <<endl;
	cout <<myPrefix<<"m_infoElemKey = "<< m_infoElemKey<<endl;
	cout <<myPrefix<<"}" <<endl;	
}
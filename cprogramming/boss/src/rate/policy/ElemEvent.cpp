#include "ElemEvent.h"
#include <iostream.h>

void ElemEvent::dump(char *prefix)
{
	char myPrefix[256] = "  ";
	strcat(myPrefix,prefix);
	cout <<myPrefix<<"{" <<endl;
	cout <<myPrefix<<"---- ElemEvent : " <<m_id<<" ----" <<endl;
	cout <<myPrefix<<"m_id           = "<< m_id <<endl;
	cout <<myPrefix<<"m_eventElemKey = "<< m_eventElemKey<<endl;
	cout <<myPrefix<<"}" <<endl;
}

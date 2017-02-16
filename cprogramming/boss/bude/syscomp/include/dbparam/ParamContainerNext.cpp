#include "dbparam/ParamContainerNext.h"

template <class T>
bool ParamContainerNext<T>::buildNextRelation()//zhaogc add
{	
	vector<T>::iterator curItr = m_paramVector.begin();
	for(itr=m_paramVector.begin(); itr!=m_paramVector.end(); ++itr)
	{
		++curItr;
		if(curItr != m_paramVector.end())
		{
			if((*itr).m_record.m_id == (*curItr).m_record.m_id)
				(*itr).m_record.m_next =&(*curItr).m_record;
			else
				(*itr).m_record.m_next =NULL;
		}
		else
		{
			(*itr).m_record.m_next =NULL;
			break;
		}
	}
	
	return true;
}

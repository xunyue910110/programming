#include "dbparam/ParamContainerChild.h"

template <class T>
bool ParamContainerChild<T>::buildNextRelation()//zhaogc add
{
	typename vector<T>::iterator curItr = m_paramVector.begin();
	typename vector<T>::iterator beginItr = m_paramVector.begin();
	typename vector<T>::iterator idCurItr ;

	for(itr=m_paramVector.begin(); itr!=m_paramVector.end(); ++itr)
	{		
		//建立子节点
		buildParentRelation(*itr);
		
		//建立兄弟节点
		++curItr;
		if(curItr != m_paramVector.end())
		{
			if((*itr).m_record.m_id==(*curItr).m_record.m_id)
			{
				if((*itr).m_record.m_parentNode == 
				                   (*curItr).m_record.m_parentNode )
				   (*itr).m_record.m_next = &(*curItr).m_record;
				else 
					(*itr).m_record.m_next = NULL;
			}
			else
			{
				(*itr).m_record.m_next = NULL;
				//for(idCurItr=beginItr; idCurItr!=curItr; ++idCurItr)
				//{						
				//	if( (*idCurItr).m_record.m_parentNode != 0)
				//		break;
				//		
				//	m_idSet.clear();
				//	if(!buildParentRelation(idCurItr,beginItr,curItr))
				//		return false;					
				//}
				//beginItr = curItr;
			}			
		}
		else
		{
			(*itr).m_record.m_next = NULL;
			break;
		}
	}
	return true;
}

template <class T>
bool ParamContainerChild<T>::buildParentRelation(T &record)
{
	//m_setIter = m_idSet.find(record.m_record.m_parentNode);
	//if(m_setIter != m_idSet.end())
	//{
	//	//error deal
	//	return false;
	//}
	//m_idSet.insert(record.m_record.m_parentNode);
	m_param.m_record.m_id = record.m_record.m_id;	
	m_param.m_record.m_parentNode = record.m_record.m_nodeId;
	m_param.m_record.m_nodeGroup = 0;
	m_param.m_record.m_orderNo = 0;	
	m_param.m_record.m_nodeId = 0;	
	
	vector<T>::iterator childIter;
	childIter = lower_bound(m_paramVector.begin(), 
	                        m_paramVector.end(), m_param);
	if(childIter!=m_paramVector.end() && (*childIter) == m_param)
		record.m_record.m_curNode = &(*childIter).m_record;		
	else
		record.m_record.m_curNode = NULL;		
		

	return true;
}

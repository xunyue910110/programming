#include "CompositItemMgr.h"
#include "PO_CompositItem.h"
#include "PO_Item.h"
#include "PO_DetailItem.h"

#include <vector>
#include <algorithm>

bool CompositItemMgr::initialize(
			const vector<PO_CompositItem> &compsitItemVector,
			const vector<PO_Item> &itemVector,
			const vector<PO_DetailItem> &detailItemVector,
			string &errorMsg)
{
	IntMap temItemMap;
	vector<int>::const_iterator iterInt;    
	IntMap::const_iterator iterIntMap;  	
	int itemCode;  		
	vector<PO_Item>::const_iterator iterItem;
	vector<PO_DetailItem>::const_iterator detailIter;
	
	clear();
	
	//加入-1账目(所有明细帐目集合)
	m_compsitItemMap[-1] = IntMap();
	detailIter=detailItemVector.begin();
	for(; detailIter!=detailItemVector.end(); ++detailIter)
	{
		if((*detailIter).m_record.m_itemUseType == '0')//费用明细帐目
		{	
			m_compsitItemMap[-1].
				insert(make_pair((*detailIter).m_record.m_itemId, 
				                 (*detailIter).m_record.m_itemId));
		}
	}	

	//建立 普通的组合帐目
	vector<PO_CompositItem>::const_iterator iter=compsitItemVector.begin();
	for(; iter!=compsitItemVector.end(); ++iter)
	{
		if (m_compsitItemMap.find ((*iter).m_record.m_id) == 
			m_compsitItemMap.end())
		{
			m_compsitItemMap[(*iter).m_record.m_id] = IntMap();
		}
		
		if((*iter).m_record.m_itemType == '1')//明细帐目
		{			
			m_compsitItemMap[(*iter).m_record.m_id].
				insert(make_pair((*iter).m_record.m_subItemId, 
				                 (*iter).m_record.m_orderNo));
		}
		else if((*iter).m_record.m_subItemId!=-1)
		{				
			m_idSet.clear();	
			m_idSet.insert((*iter).m_record.m_id);
			if(!addDetailItem((*iter).m_record.m_id,
				              (*iter).m_record.m_subItemId, 
				              (*iter).m_record.m_orderNo,
				              compsitItemVector))
			{			
				char buff[5];
				sprintf(buff,"%d",(*iter).m_record.m_id);
				buff[4]='\0';
				errorMsg="CompositItemMgr->initialize() fail:"
				"组合帐目可能存在循环定义:";
				errorMsg += (const char *)buff;
				errorMsg += " subItem:";
				sprintf(buff,"%d",(*iter).m_record.m_subItemId);
				errorMsg += (const char *)buff;
				return false;
			}
		}		
		else //if((*iter).m_record.m_subItemId==-1)
		{	
			m_compsitItemMap[(*iter).m_record.m_id] = m_compsitItemMap[-1];			
		}
	}	

	//建立 取反的组合帐目	
	for(iterItem=itemVector.begin();iterItem!=itemVector.end(); ++iterItem)
	{
		if((*iterItem).m_record.m_reverseItemVector.size()==0 )
			continue;

		//获取基准组合帐目的明细账目集合
		temItemMap = find ((*iterItem).m_record.m_id);
					
		//去掉指定的帐目
		for(iterInt=(*iterItem).m_record.m_reverseItemVector.begin();
			iterInt!=(*iterItem).m_record.m_reverseItemVector.end();++iterInt)
		{
			itemCode = *iterInt;
			if (itemCode>0)
			{			
				IntMap& reverseMap = find(itemCode);
				for (iterIntMap=reverseMap.begin(); iterIntMap!=reverseMap.end();
					 ++iterIntMap)
				{
					temItemMap.erase((*iterIntMap).first);
				}
			}
			else
			{
				itemCode = -itemCode;
				IntMap& reverseMap1 = find(itemCode);
				for (iterIntMap=reverseMap1.begin(); iterIntMap!=reverseMap1.end();
					 ++iterIntMap)
				{
					temItemMap.insert(make_pair((*iterIntMap).first,
					                  (*iterIntMap).second));
				}
			}
		}
		//插入到组合帐目map中
		m_compsitItemMap[(*iterItem).m_record.m_id] = temItemMap;
	}

	return true;
}

IntMap & CompositItemMgr::find (const int & compositItem) 
{		
	if ((m_itr = m_compsitItemMap.find(compositItem)) == 
		m_compsitItemMap.end())
	{
		//组合账目没有找到,则认为是明细账目
		m_detailItemMap.clear();
		m_detailItemMap[compositItem] = compositItem;
		return m_detailItemMap;
	}
	else
	{
		return (*m_itr).second;
	}
}
		
int CompositItemMgr::find (const int & compositItem, 
						   vector<DetailItem>& detailItemVector)
{
	detailItemVector.clear();	
	if ((m_itr=m_compsitItemMap.find(compositItem)) !=
		 m_compsitItemMap.end())
	{		
		DetailItem	detailItem;
		IntMap::const_iterator itrD;
		for(itrD=(*m_itr).second.begin(); itrD!=(*m_itr).second.end(); ++itrD)
		{
			detailItem.m_id = (*itrD).first;
			detailItem.m_orderNo = (*itrD).second;
			detailItemVector.push_back(detailItem);
		}
		partial_sort(detailItemVector.begin(),detailItemVector.end(),
			         detailItemVector.end());
		return detailItemVector.size();
	}
		
	return 0;
}

bool	CompositItemMgr::addDetailItem(const int &compositItemCode, 
									   const int &subCompisitItemCode,
									   const int &orderNo,
									   const vector<PO_CompositItem> &compsitItemVector)
{
	if(m_idSet.find(subCompisitItemCode) != m_idSet.end())
	{
		return false;
	}
	m_idSet.insert(subCompisitItemCode);
	
	PO_CompositItem compositItem;
	compositItem.m_record.m_id = subCompisitItemCode;
	compositItem.m_record.m_orderNo = 0;

	vector<PO_CompositItem>::const_iterator iter;
	iter=lower_bound(compsitItemVector.begin(),
		            compsitItemVector.end(),
					compositItem);

	for(;iter!=compsitItemVector.end();++iter)
	{
		if((*iter).m_record.m_id != subCompisitItemCode)
			break;
		if( (*iter).m_record.m_itemType == '1')
		{			
			m_compsitItemMap [compositItemCode].
				insert(make_pair((*iter).m_record.m_subItemId, orderNo));
		}
		else
		{
			//-1(所有账目)不能出现在底层的账目中
			if( (*iter).m_record.m_subItemId != -1)
			{
				if(!addDetailItem(compositItemCode,(*iter).m_record.m_subItemId,
					              orderNo,compsitItemVector))
				{				
					return false;
				}
				m_idSet.erase((*iter).m_record.m_subItemId);//出栈
			}
			else
				continue;
		}
	}
	return true;	
}
	
void	CompositItemMgr::clear() 
{ 
	for(m_itr=m_compsitItemMap.begin();
	    m_itr!=m_compsitItemMap.end();++m_itr)
	{
		(*m_itr).second.clear();
	}
	m_compsitItemMap.clear(); 
}

void CompositItemMgr::dump()
{
	cout <<"---------------CompositItemMgr::dump()----------------"<< endl;	
	IntMap::const_iterator iter;
	for(m_itr=m_compsitItemMap.begin();
	    m_itr!=m_compsitItemMap.end();++m_itr)
	{
		cout <<"CompositItem:"<<(*m_itr).first<< endl;
		for(iter=(*m_itr).second.begin();iter!=(*m_itr).second.end();++iter)
			cout <<"  item:"<<(*iter).first<< " order:"<<(*iter).second<< endl;
	}
}

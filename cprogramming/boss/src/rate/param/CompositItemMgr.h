#ifndef _COMPOSITITEMMGR_H_
#define _COMPOSITITEMMGR_H_

#include "CompositItem.h"

#include <set>
#include <map>

typedef map<int,int> IntMap;
typedef map<int,IntMap>	IntmapMap;

class PO_CompositItem;
class PO_DetailItem;
class PO_Item;

class CompositItemMgr
{
public:

	CompositItemMgr() {}
	~CompositItemMgr(){m_compsitItemMap.clear();}

public:
	bool initialize(const vector<PO_CompositItem> &,
	                const vector<PO_Item> &,
	                const vector<PO_DetailItem> &,
	                string &);
	IntMap& find (const int &compositItem);	
	int find (const int &compositItem, 
	          vector<DetailItem> &detailItemVector);
	int size (){ return m_compsitItemMap.size(); } 
	void	clear();
	void dump();
private:
	bool	addDetailItem (const int &compositItem,
	                       const int &subCompositItem,
	                       const int &orderNo,
		                   const vector<PO_CompositItem> &compsitItemVector);
private:	
	set<int>	m_idSet;
	IntmapMap 	m_compsitItemMap;
	IntmapMap::iterator	m_itr;
	IntMap 	    m_detailItemMap;
};

#endif

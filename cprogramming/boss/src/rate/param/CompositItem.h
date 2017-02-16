#ifndef _COMPOSITITEM_H_
#define _COMPOSITITEM_H_

#include <vector>

using namespace std;

//Item：定义帐目结构信息.
class Item 
{
    public:
        Item() {}
        ~Item() {}
    public: 
        bool operator < (const Item &right) const
        {            
			return (m_id < right.m_id);
        }        

		bool operator == (const Item &right) const
        {
            return (m_id == right.m_id);
        }
                 
    public:
        int m_id; //帐目标识
        char m_itemType;   //帐目类型 0:组合帐目 1:明细帐目        
   		vector<int> m_reverseItemVector; //需要去除的账目集合
};

//DetailItem：定义明细帐目结构信息.
class DetailItem
{
    public:
        DetailItem() {}
        ~DetailItem() {}
    public: 
		bool operator == (const DetailItem &detailItem) const
        {
            return (m_orderNo == detailItem.m_orderNo &&
                	m_id == detailItem.m_id);                    
        }

        bool operator < (const DetailItem &detailItem) const
        {
            if (m_orderNo == detailItem.m_orderNo)
            {				
				return (m_id < detailItem.m_id);
            }
            else
            {
                return (m_orderNo < detailItem.m_orderNo);
            }        
        }
                 
    public:
		int m_id;//帐目标识
		int m_orderNo;    //帐目优先级
};

class CompositItem
{
    public:
        CompositItem() {}
        ~CompositItem() {}
    public:    
        bool operator < (const CompositItem &compositItem) const
        {
        	if(m_id==compositItem.m_id)
        		return (m_orderNo < compositItem.m_orderNo);
        	else
            	return (m_id < compositItem.m_id);
        }

		bool operator == (const CompositItem &compositItem) const
        {
            return (m_id == compositItem.m_id);
        }
                 
    public:
		char m_itemType;         //子帐目类型 1:组合帐目 0:明细帐目		
		int m_id;       //帐目标识		
		int m_subItemId;//子帐目标识
		int m_orderNo;  //优先级
};

#endif

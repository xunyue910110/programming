#ifndef _PRICECOMP_H_
#define _PRICECOMP_H_

#include "NodeBase.h"

const unsigned char NODE_TYPE_COND     = '0';//条件
const unsigned char NODE_TYPE_ADDUP    = '1';//累计
const unsigned char NODE_TYPE_FEECOUNT = '2';//费用计算
const unsigned char NODE_TYPE_DISCNT   = '3';//费用优惠

//批价政策构成，用一棵树对批价政策进行展现!
//父节点下面节点排序按节点归属组编码进行排序，
//然后再按节点顺序进行排序!

class PriceComp
{
  public:
  	PriceComp() 
  	{ 
  		m_curNode = 0; 
  		m_next = 0;
  		m_nodeAction = 0; 
  	}
      ~PriceComp() {}
  public:
  	bool execute();
  	bool rateExecute(vector<CdrSeg> &r_cdrSegList);
  	void dump(char *prefix);
  public:
  	bool operator < (const PriceComp &right) const
  	{
  		if (m_id == right.m_id)
  		{				
  			if (m_parentNode == right.m_parentNode)
  			{
  				if (m_nodeGroup == right.m_nodeGroup)
  				{
  					if (m_orderNo == right.m_orderNo)
  					{
  						return (m_nodeId < right.m_nodeId);
  					}
  					else
  						return (m_orderNo < right.m_orderNo);
  				}
  				else
  					return (m_nodeGroup < right.m_nodeGroup);
  			}
  			else
  			{
  				return (m_parentNode < right.m_parentNode);
  			}
  		}
  		else
  		{
  			return (m_id < right.m_id);
  		}
  	}
  
  	bool operator == (const PriceComp& right) const
  	{
  		return ( (m_id == right.m_id) &&
  			     (m_parentNode == right.m_parentNode) );
  	}
  	
  public:		
  	char       m_nodeType;  //节点动作类型 0:条件 1:累计 2:费用计算 3:费用优惠
  	short      m_nodeGroup;//节点归属组编码
  	short      m_orderNo;  //组内节点顺序
  	int        m_parentNode; //父节点标识
  	int        m_id;         //批价政策标识(price_id)
  	int        m_nodeId;     //节点标识
  	int        m_nodeActId;  //节点动作标识
  	PriceComp *m_next;	       //下一个节点
  	PriceComp *m_curNode;      //子节点头指针(节点标志)
  	NodeBase  *m_nodeAction;   //节点动作
  private:
    static RateGlobalInfo *m_globalInfo;
};

#endif

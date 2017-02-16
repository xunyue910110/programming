#ifndef _EVENTFEEPOLICY_H_
#define _EVENTFEEPOLICY_H_

class Price;
class PolicyInfo;

class EventFeePolicy
{
public:
    EventFeePolicy() { m_price=0; m_next=0;}
    ~EventFeePolicy() {}
	bool execute();
	void dump(char *);
public:
	bool operator < (const EventFeePolicy &right) const
	{
		if( m_id == right.m_id)				
			return (m_feeId < right.m_feeId);
		else
			return (m_id < right.m_id);			
	}

	bool operator == (const EventFeePolicy &right) const
	{
		return (m_id == right.m_id);
	}      
	
public:
	char            m_rateTime; //政策批次	
	int             m_id;       //事件资费标识
	int             m_feeId;    //归属费用项
	int             m_priceId;  //批价政策标识	
	Price          *m_price;	  //批价政策标识指针 
	EventFeePolicy *m_next;	    //下一个事件资费
private:
	static PolicyInfo	*m_policyInfo;
};

#endif
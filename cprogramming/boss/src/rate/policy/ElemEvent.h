#ifndef _ELEMEVENT_H_
#define _ELEMEVENT_H_

class ElemEvent
{
  public:
  	ElemEvent(){}
  	~ElemEvent(){}
  	bool operator < (const ElemEvent &right) const
  	{
  		return (m_id < right.m_id);			
  	}
  
  	bool operator == (const ElemEvent &right) const
  	{
  		return (m_id == right.m_id);
  	}
  public:	
  	void dump(char *prefix);
  	
  
  public:
    int  m_id;               //事件元素标识
  	int  m_eventElemKey;     //事件元素关键字  	
};	

#endif

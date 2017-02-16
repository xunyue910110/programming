#ifndef _ELEMINFO_H_
#define _ELEMINFO_H_

class ElemInfo
{
  public:
  	ElemInfo(){}
  	~ElemInfo(){}
  	bool operator < (const ElemInfo &right) const
  	{
  		return (m_id < right.m_id);			
  	}
  
  	bool operator == (const ElemInfo &right) const
  	{
  		return (m_id == right.m_id);
  	}
  public:	
  	void dump(char *prefix);
  	
  
  public:
    int  m_id;               //资料元素标识
  	int  m_infoElemKey;     //资料元素关键字  	
};	

#endif

#ifndef _OBJECTCDR_H_
#define _OBJECTCDR_H_

#include "ObjectBase.h"

class PCDR ;

class ObjectCdr :public ObjectBase
{
public:
	ObjectCdr(){}
	~ObjectCdr(){}

	bool operator == (const ObjectCdr &right) const
    {
        return (m_id == right.m_id);
    }

    bool operator < (const ObjectCdr &right) const
    {
        return (m_id < right.m_id);       
    }
public:
	virtual bool getObjectValue(T_OBJECT_ATTR &value);
	virtual bool getObjectValue(long &value);
	virtual void dump(char *);
public:
	int m_key;	
private:
	static PCDR *m_cdr;	
};

#endif

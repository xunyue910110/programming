#ifndef _OBJECTVALUE_H_
#define _OBJECTVALUE_H_

#include "ObjectBase.h"

class ObjectValue :public ObjectBase
{
public:
	ObjectValue();
	~ObjectValue();
	
public:
	virtual bool getObjectValue(T_OBJECT_ATTR &value);
	virtual bool getObjectValue(long &value);
	void setValue(const long &value);
	virtual void dump(char *prefix);
	
private:
	long m_value;
};

#endif

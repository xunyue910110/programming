#ifndef _OBJECTPARAM_H_
#define _OBJECTPARAM_H_

#include "ObjectBase.h"

class ObjectParam :public ObjectBase
{
public:
	ObjectParam();
	~ObjectParam();
	
public:
	virtual bool getObjectValue(T_OBJECT_ATTR &value);
	virtual bool getObjectValue(long &value);
	void setParamId(const long &paramId);
	virtual void dump(char *prefix);
	
private:
	long m_paramId;
};

#endif

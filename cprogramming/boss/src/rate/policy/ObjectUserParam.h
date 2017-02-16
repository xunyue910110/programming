#ifndef _OBJECTUSERPARAM_H_
#define _OBJECTUSERPARAM_H_

#include "ObjectBase.h"

class ObjectUserParam :public ObjectBase
{
public:
	ObjectUserParam();
	~ObjectUserParam();
	
public:
	virtual bool getObjectValue(T_OBJECT_ATTR &value);
	virtual bool getObjectValue(long &value);
	void setParamId(const long &paramId);
	virtual void dump(char *prefix);
	
private:
	long m_paramId;
};

#endif //_OBJECTUSERPARAM_H_

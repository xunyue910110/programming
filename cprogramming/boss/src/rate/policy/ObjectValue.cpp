#include "ObjectValue.h"
#include "RateGlobalInfo.h"
#include "base/exceptions.h"

ObjectValue::ObjectValue(){}
ObjectValue::~ObjectValue(){}

void ObjectValue::dump(char *prefix)
{
	cout << " Value : " << m_value <<endl;
}

bool ObjectValue::getObjectValue(T_OBJECT_ATTR &value)
{
	value = m_value;
	return true;
}

bool ObjectValue::getObjectValue(long &value)
{
	value = m_value;
	return true;
}

void ObjectValue::setValue(const long &value)
{
	m_value = value;
}

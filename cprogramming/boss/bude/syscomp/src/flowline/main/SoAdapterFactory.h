#ifndef _SOADAPTERFACTORY_H_
#define _SOADAPTERFACTORY_H_
#include "flowline/adapter/AdapterFactory.h"
#include "base/Properties.h"

using namespace flowline::adapter;

class SoAdapterFactory : public AdapterFactory
{
public:
	SoAdapterFactory();
	virtual void init(const Properties* env);
	virtual Adapter * doCreate(const string & className);
    virtual void getClassNames(vector<string>& classNames) const;
	virtual ~SoAdapterFactory();
private:
	vector<void*> m_handles;
};

#endif //_SOADAPTERFACTORY_H_

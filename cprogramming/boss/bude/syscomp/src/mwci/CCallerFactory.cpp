#include "base/exceptions.h"
#include "mwci/CCallerFactory.h"
#include "mwci/IMWCaller.h"

//##ModelId=4190CDA6038C
IMWCaller* CCallerFactory::CreateCaller(int iCallerType)
{
	IMWCaller *theCaller;
	switch (iCallerType)
	{
		case 0 : theCaller = new CTuxCaller();break;
		case 1 : theCaller = NULL; break;
		case 2 : theCaller = NULL; break;
	}

	return theCaller;
}


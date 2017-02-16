#ifndef _DEFINITIONEXCEPTION_
#define _DEFINITIONEXCEPTION_
#include "base/exceptions.h"
namespace flowline {
	DECLARE_EXCEPTION(FlowLineException, BaseException);	
	DECLARE_EXCEPTION(DefinitionException, FlowLineException);
	DECLARE_EXCEPTION(ExecutionException, FlowLineException);
}
#endif //_DEFINITIONEXCEPTION_

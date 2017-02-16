#ifndef LCUEXCEPTION_H
#define LCUEXCEPTION_H
#include "base/config-all.h"
#include "base/exceptions.h"
#include <stdexcept>

USING_NAMESPACE(std);

/** 流程引擎内核 */
namespace ebpm4c {
   
    DECLARE_EXCEPTION(LCUException, BaseException);
    DECLARE_EXCEPTION(LCUDefinitionException, LCUException);
    DECLARE_EXCEPTION(LCUExecutionException, LCUException);
    DECLARE_EXCEPTION(LCUWrapperException, LCUException); 
    
};

#endif //LCUEXCEPTION_H

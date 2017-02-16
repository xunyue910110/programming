#ifndef MWEXCEPTION_H_HEADER_INCLUDED_BE386356
#define MWEXCEPTION_H_HEADER_INCLUDED_BE386356
#include "mwci/MWGlobal.h"
#include "base/exceptions.h"
//##ModelId=41C78CF1036B

DECLARE_EXCEPTION(MWException, BaseException);
DECLARE_EXCEPTION(CTuxException, MWException);
DECLARE_EXCEPTION(CFmlException, MWException);

#endif /* MWEXCEPTION_H_HEADER_INCLUDED_BE386356 */

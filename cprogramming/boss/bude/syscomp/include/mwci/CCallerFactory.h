#ifndef CCALLERFACTORY_H_HEADER_INCLUDED_BE678A9F
#define CCALLERFACTORY_H_HEADER_INCLUDED_BE678A9F
#include "mwci/IMWCaller.h"
#include "mwci/CTuxCaller.h"

// Caller Factory
// --------------
// Use to produce kinds of Caller.
//##ModelId=4190C9CE007A
class CCallerFactory
{
  public:
    // Return Null if failure.

    //! 创建一个符合IMWCaller接口的类实例
    /*!
		\param iCallerType 实例类型
			- 0 Tuxedo实现

		\return 一个符合IMWCaller接口的类实例指针
		\sa IMWCaller
    */
    //##ModelId=4190CDA6038C
 static  IMWCaller* CreateCaller(
        int iCallerType = 0);

};



#endif /* CCALLERFACTORY_H_HEADER_INCLUDED_BE678A9F */

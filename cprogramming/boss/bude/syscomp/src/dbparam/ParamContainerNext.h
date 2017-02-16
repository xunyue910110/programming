#ifndef PARAMCONTAINERNEXT_H_INCLUDED_C46A7A79
#define PARAMCONTAINERNEXT_H_INCLUDED_C46A7A79

#include "dbparam/ParamContainer.h"

//##ModelId=3B6275650310
template <class T>
class ParamContainerNext : public ParamContainer<T>
{
  public:    
    virtual bool buildNextRelation();//zhaogc add    
};

#include "dbparam/ParamContainerNext.cpp"

#endif /* PARAMCONTAINER_H_INCLUDED_C46A7A79 */



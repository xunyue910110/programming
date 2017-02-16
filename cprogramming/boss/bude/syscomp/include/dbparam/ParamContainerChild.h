#ifndef PARAMCONTAINERCHILD_H_INCLUDED_C46A7A79
#define PARAMCONTAINERCHILD_H_INCLUDED_C46A7A79


#include "dbparam/ParamContainer.h"

template <class T>
class ParamContainerChild : public ParamContainer<T>
{
  public:    
    virtual bool buildNextRelation();//zhaogc add
    
  protected:
     bool buildParentRelation(T &);
  protected:
  	set<int> m_idSet;
  	set<int>::iterator m_setIter; 
};

#include "dbparam/ParamContainerChild.cpp"

#endif /* PARAMCONTAINER_H_INCLUDED_C46A7A79 */



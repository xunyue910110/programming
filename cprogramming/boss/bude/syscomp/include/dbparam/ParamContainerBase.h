#ifndef PARAMCONTAINERBASE_H_INCLUDED_C46A6AE7
#define PARAMCONTAINERBASE_H_INCLUDED_C46A6AE7

#include "base/config-all.h"

#include "dbparam/PO_Common.h"

#include <string>
#include <vector>
#include <iostream>

typedef vector<string> StringVector;
typedef StringVector::iterator StringVectorIterator;

//##ModelId=3B8F4E9902E9
class ParamContainerBase
{
  public:

    //## 导入记录数据集
    virtual bool dataIn(StringVector &recordSet) = 0;

    virtual bool rewind() = 0;

    virtual bool prepareSize(long size) = 0;

    virtual bool resize(long size) = 0;

    //##ModelId=3B8F4F53007A
    virtual bool clean() = 0;

    //##ModelId=3B8F512A035C
    virtual bool sort() = 0;

    virtual bool dump(ostream& os) = 0;

    virtual bool getProperty(PO_Property &property) = 0;
    
    virtual bool buildNextRelation() = 0;//zhaogc add

    //added by shandy 20060712
    virtual void setLocalProperty(PO_Property* prop=NULL)=0;
    virtual PO_Property* getLocalProperty()=0;
};



#endif /* PARAMCONTAINERBASE_H_INCLUDED_C46A6AE7 */

#ifndef PARAMCONTAINER_H_INCLUDED_C46A7A79
#define PARAMCONTAINER_H_INCLUDED_C46A7A79

#include "base/config-all.h"

#include "dbparam/ParamContainerBase.h"

#include <algorithm>
#include <string>
#include <vector>
#include <list>
#include <set>

typedef vector<string> StringVector;
typedef StringVector::iterator StringVectorIterator;

//##ModelId=3B6275650310
template <class T>
class ParamContainer : public ParamContainerBase
{
  public:

    //## 导入记录数据集
    virtual bool dataIn(StringVector &recordSet);

    virtual bool rewind();

    virtual bool prepareSize(long size);

    virtual bool resize(long size);

    virtual bool clean();

    virtual bool sort();

    virtual bool dump(ostream &os);

    virtual bool getProperty(PO_Property &property);
    
    virtual bool buildNextRelation();//zhaogc add    

    int search(const T &record,T &result);
    int search(const T &record,T* &result);
    int searchMulti(const T &record, vector<T> &result);
    int searchMulti(const T &record, vector<T*> &result);
    int searchMulti(const T &record, list<T> &result);
    int searchMulti(const T &record, list<T*> &result);

    int scanSearch(const T &record,T &result);
    int scanSearch(const T &record,T* &result);
    int scanSearchMulti(const T &record, vector<T> &result);
    int scanSearchMulti(const T &record, vector<T*> &result);
    bool getParamVector(vector<T>  &result);
    bool getParamVector(vector<T*>  &result);
    vector<T> &getParamVector();
    
    //added by shandy 20060712
    virtual void setLocalProperty(PO_Property* prop=NULL);
    virtual PO_Property* getLocalProperty();
  protected://zhaogc modified
    bool insert(StringVector &record);
    vector<T> m_paramVector;
    typename vector<T>::iterator itr;
    T m_param;
	long m_point;
    //added by shandy 20060712
    PO_Property m_localProperty;
};

#include "dbparam/ParamContainer.cpp"

#endif /* PARAMCONTAINER_H_INCLUDED_C46A7A79 */



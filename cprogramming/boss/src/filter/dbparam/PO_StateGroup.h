#ifndef PO_STATEGROUP_H_
#define PO_STATEGROUP_H_

#include "dbparam/PO_Common.h"
#include "FieldsXX.h"

class PO_StateGroup
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_StateGroup(){}

    PO_StateGroup(const PO_StateGroup& right);

    void operator=(const PO_StateGroup& right);

    friend int operator==(const PO_StateGroup &left,
    		const PO_StateGroup& right);

    friend int operator<(const PO_StateGroup &left,
    		const PO_StateGroup& right);

	friend ostream& operator<<(ostream& os,const PO_StateGroup& po);

    struct
    {
    	T_FLAG        m_state;
    	T_FLAG        m_stategroup;
    }recordStruct;

  private:
	static PO_Property m_property;
};
#endif /* PO_STATEGROUP_H_ */
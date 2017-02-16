#ifndef PO_MMSSERVCODEBYSPOPER_H

#define PO_MMSSERVCODEBYSPOPER_H



#include "dbparam/PO_Common.h"
#include "CdrProperty.h"

class PO_MmsServCodeBySpOper

{

  public:

    bool getProperty(PO_Property &property);



    bool convert(const StringVector &argname);



    PO_MmsServCodeBySpOper(){}



    PO_MmsServCodeBySpOper(const PO_MmsServCodeBySpOper& right);



    void operator=(const PO_MmsServCodeBySpOper& right);



    friend int operator==(const PO_MmsServCodeBySpOper &left,

    		const PO_MmsServCodeBySpOper& right);



    friend int operator<(const PO_MmsServCodeBySpOper &left,

    		const PO_MmsServCodeBySpOper& right);



	friend ostream& operator<<(ostream& os,const PO_MmsServCodeBySpOper& po);



    MmsProperty  recordStruct;



  private:

	static PO_Property m_property;

};







#endif /* PO_MMSSERVCODEBYSPOPER_H */


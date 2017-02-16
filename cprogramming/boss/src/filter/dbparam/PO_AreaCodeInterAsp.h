#ifndef PO_AREACODEINTERASP_H
#define PO_AREACODEINTERASP_H

#include "dbparam/PO_Common.h"
#include "FieldsXX.h"
#include "base/StringUtil.h"
#include <vector>

class PO_AreaCodeInterAsp
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_AreaCodeInterAsp(){}

    PO_AreaCodeInterAsp(const PO_AreaCodeInterAsp& right);

    void operator=(const PO_AreaCodeInterAsp& right);

    friend int operator==(const PO_AreaCodeInterAsp &left,
    		const PO_AreaCodeInterAsp& right);

    friend int operator<(const PO_AreaCodeInterAsp &left,
    		const PO_AreaCodeInterAsp& right);

	friend ostream& operator<<(ostream& os,const PO_AreaCodeInterAsp& po);

    struct
    {
    	T_INTER_ASPCODE   m_aspCode;
    	T_DATE_TIME       m_beginTime;
    	T_DATE_TIME       m_endTime;
    	T_AREA_CODE       m_areaCode;
	    vector<string>    m_interhead;
    }recordStruct;

  private:
	static PO_Property m_property;
};



#endif /* PO_AREACODEINTERASP_H */

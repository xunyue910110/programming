#ifndef PO_AREACODEIMSII_H
#define PO_AREACODEIMSII_H

#include "dbparam/PO_Common.h"
#include "FieldsXX.h"

class PO_AreaCodeImsiI
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_AreaCodeImsiI(){}

    PO_AreaCodeImsiI(const PO_AreaCodeImsiI& right);

    void operator=(const PO_AreaCodeImsiI& right);

    friend int operator==(const PO_AreaCodeImsiI &left,
    		const PO_AreaCodeImsiI& right);

    friend int operator<(const PO_AreaCodeImsiI &left,
    		const PO_AreaCodeImsiI& right);

	friend ostream& operator<<(ostream& os,const PO_AreaCodeImsiI& po);

    struct
    {
    	T_IMSI_SEG       m_imsi;
    	T_DATE_TIME      m_beginTime;
    	T_DATE_TIME      m_endTime;
    	T_AREA_CODE      m_feeCode;
    	T_USER_TYPE      m_userType;
    	T_GROUP_ID       m_groupId;
    	T_INTER_ASPCODE  m_aspCode;                 
    	T_DATE_TIME      m_groupBeginTime;
    	T_DATE_TIME      m_groupEndTime;
    	T_INTER_ASPGROUP m_aspGroup;
    }recordStruct;

  private:
	static PO_Property m_property;
};



#endif /* PO_AREACODEIMSII_H */


#ifndef PO_HOLIDAY_H_INCLUDE_20090808_
#define PO_HOLIDAY_H_INCLUDE_20090808_

#include "dbparam/PO_Common.h"
#include "FieldsXX.h" 
#include "BillingCommon.h"

const string DATETYPE_CALENDAR = "1";
const string DATETYPE_LUNAR    = "2";
const string CYCLETYPE_MONTH   = "1";
const string CYCLETYPE_YEAR    = "2";
const string CYCLETYPE_FIX     = "3";

class PO_Holiday
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_Holiday(){}

    PO_Holiday(const PO_Holiday &right);

    void operator=(const PO_Holiday &right);

    friend int operator==(const PO_Holiday &left,const PO_Holiday &right);

    friend int operator<(const PO_Holiday &left,const PO_Holiday &right);

	  friend ostream& operator<<(ostream& os,const PO_Holiday& po);

    struct
    {
    	String<4>      m_holidayType;
    	String<8>      m_holidayBeginDate;
    	String<8>      m_holidayEndDate;
    	T_FLAG         m_dateType;
    	T_FLAG         m_cycleType;
    	T_DATE_TIME    m_beginTime;
    	T_DATE_TIME    m_endTime;
    }m_record;

  private:
	  static PO_Property m_property;
}; 

class Holiday
{
  public :
	String<4>      m_holidayType;
	String<8>      m_holidayDate;
	T_DATE_TIME    m_beginTime;
	T_DATE_TIME    m_endTime;
	void operator=(const Holiday &right);
	void operator=(const PO_Holiday &right);
    friend int operator==(const Holiday &left,const Holiday &right);
    friend int operator<(const Holiday &left,const Holiday &right);
	friend ostream& operator<<(ostream& os,const Holiday& po);
};

#endif //PO_HOLIDAY_H_INCLUDE_20090808_

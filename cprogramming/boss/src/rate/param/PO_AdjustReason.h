#ifndef PO_ADJUSTREASON_H_INCLUDE_20081014_
#define PO_ADJUSTREASON_H_INCLUDE_20081014_

#include "dbparam/PO_Common.h"
#include "FieldsXX.h" 

class PO_AdjustReason
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_AdjustReason(){}

    PO_AdjustReason(const PO_AdjustReason &right);

    void operator=(const PO_AdjustReason &right);

    friend int operator==(const PO_AdjustReason &left,const PO_AdjustReason &right);

    friend int operator<(const PO_AdjustReason &left,const PO_AdjustReason &right);

	friend ostream& operator<<(ostream& os,const PO_AdjustReason& po);

    struct
    {
       int     m_adjustReasonId; 
       char    m_adjustModeType;
       int     m_priority      ; 
       char    m_ifShowNegative;
       char    m_effectTimeTag;
       T_TP_ID m_condId;
    }  m_record;

  private:
	  static PO_Property m_property;
}; 
#endif //PO_ADJUSTREASON_H_INCLUDE_20081014_

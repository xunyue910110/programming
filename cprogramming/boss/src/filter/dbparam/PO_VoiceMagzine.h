#ifndef PO_VoiceMagzine_H
#define PO_VoiceMagzine_H

#include "dbparam/PO_Common.h"
#include "FieldsXX.h"

class PO_VoiceMagzine
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_VoiceMagzine(){}

    PO_VoiceMagzine(const PO_VoiceMagzine& right);

    void operator=(const PO_VoiceMagzine& right);

    friend int operator==(const PO_VoiceMagzine &left,
    		const PO_VoiceMagzine& right);

    friend int operator<(const PO_VoiceMagzine &left,
    		const PO_VoiceMagzine& right);

    friend ostream& operator<<(ostream& os,const PO_VoiceMagzine& po);

    struct
      {
      	T_FLAG             m_matchType;
      	string             m_prefixType;
      	T_MSISDN           m_phone;
      	T_AREA_CODE        m_areaCode;//新增
      	int                m_groupId;
      	T_DATE_TIME        m_beginTime;
      	T_DATE_TIME        m_endTime;
      }recordStruct;
  private:
	static PO_Property m_property;
};



#endif /* PO_VoiceMagzine_H */

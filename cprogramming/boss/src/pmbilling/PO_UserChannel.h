#ifndef PO_USERCHANNEL_H_
#define PO_USERCHANNEL_H_

#include "dbparam/PO_Common.h"
#include "FieldsXX.h"

class PO_UserChannel
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_UserChannel(){}

    PO_UserChannel(const PO_UserChannel& right);

    void operator=(const PO_UserChannel& right);

    friend int operator==(const PO_UserChannel &left,
    		const PO_UserChannel& right);

    friend int operator<(const PO_UserChannel &left,
    		const PO_UserChannel& right);

	friend ostream& operator<<(ostream& os,const PO_UserChannel& po);

	//limeng modified 20090812
    struct
    {
    	Integer       m_beginPartition;
    	Integer       m_endPartition;
    	T_EPARCHY_CODE   m_eparchyCode;
    	T_CHANNEL     m_channelNo;
    	int				m_dbNo;
    	string			m_ratePath;
    }recordStruct;

  private:
	static PO_Property m_property;
};
#endif /* PO_USERCHANNEL_H_ */
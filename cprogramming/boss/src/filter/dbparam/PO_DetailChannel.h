#ifndef PO_DetailChannel_H
#define PO_DetailChannel_H

#include "dbparam/PO_Common.h"
#include "DetailChannelItem.h"

class PO_DetailChannel
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_DetailChannel(){}

    PO_DetailChannel(const PO_DetailChannel& right);

    void operator=(const PO_DetailChannel& right);

    friend int operator==(const PO_DetailChannel &left,
    		const PO_DetailChannel& right);

    friend int operator<(const PO_DetailChannel &left,
    		const PO_DetailChannel& right);

	friend ostream& operator<<(ostream& os,const PO_DetailChannel& po);

	DetailChannelItem recordStruct;

  private:
	static PO_Property m_property;
};


class PO_DetailChannel_GSM:public PO_DetailChannel
{
  public:
    bool getProperty(PO_Property &property);
  private:
	static PO_Property m_property;
};

class PO_DetailChannel_SM:public PO_DetailChannel
{
  public:
    bool getProperty(PO_Property &property);
  private:
	static PO_Property m_property;
};

class PO_DetailChannel_GPRS:public PO_DetailChannel
{
  public:
    bool getProperty(PO_Property &property);
  private:
	static PO_Property m_property;
};

class PO_DetailChannel_MISC:public PO_DetailChannel
{
  public:
    bool getProperty(PO_Property &property);
  private:
	static PO_Property m_property;
};


#endif /* PO_DetailChannel_H */

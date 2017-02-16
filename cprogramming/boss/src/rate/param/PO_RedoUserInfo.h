#ifndef PO_REDOUSERINFO_H
#define PO_REDOUSERINFO_H

#include "dbparam/PO_Common.h"
#include "FieldsXX.h"

class PO_RedoUserInfo
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_RedoUserInfo(){}

    PO_RedoUserInfo(const PO_RedoUserInfo& right);

    void operator=(const PO_RedoUserInfo& right);

    friend int operator==(const PO_RedoUserInfo &left,
    		                  const PO_RedoUserInfo& right);

    friend int operator<(const PO_RedoUserInfo &left,
    		                 const PO_RedoUserInfo& right);

	  friend ostream& operator<<(ostream& os,const PO_RedoUserInfo& po);

	  struct
	  {
	    T_SERVER_ID m_userId;
	  } m_record;

  private:
	  static PO_Property m_property;
};



#endif //PO_REDOUSERINFO_H


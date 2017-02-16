#ifndef PO_FreeInfoDef_H
#define PO_FreeInfoDef_H


#include "dbparam/PO_Common.h"

class PO_FreeInfoDef
{
      public:

          bool getProperty(PO_Property &property);
          
          bool convert(const StringVector &argname);
          
          PO_FreeInfoDef(){}
          
          PO_FreeInfoDef(const PO_FreeInfoDef& right);
          
          void operator = (const PO_FreeInfoDef& right);
          
          friend int operator == (const PO_FreeInfoDef &left,
                                  const PO_FreeInfoDef& right);
          
          friend int operator < (const PO_FreeInfoDef &left,
                                 const PO_FreeInfoDef& right);
          
          friend ostream& operator << (ostream& os,const PO_FreeInfoDef& po);
          
          struct
          {
          	string		m_selectSql;
          	int			m_startMonth;
          	int			m_endMonth;
          } m_record;
      
      private:

          static PO_Property m_property;
};


#endif

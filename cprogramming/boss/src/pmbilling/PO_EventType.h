#ifndef PO_EventType_H
#define PO_EventType_H

#include "dbparam/PO_Common.h"
#include "FieldsXX.h" 

class CondBase;

class PO_EventType
{
      public:
          bool getProperty(PO_Property &property);
          
          bool convert(const StringVector &argname);
          
          PO_EventType(){}
          
          PO_EventType(const PO_EventType& right);
          
          void operator=(const PO_EventType& right);
          
          friend int operator==(const PO_EventType &left,
                                const PO_EventType& right);
          
          friend int operator<(const PO_EventType &left,
                               const PO_EventType& right);
          
          friend ostream& operator<<(ostream& os,const PO_EventType& po);
          
          struct 
          {
              T_BIZ_TYPE m_eventTypeId;
              T_BIZ_TYPE m_pEventTypeId;
              char       m_eventRank;
              int        m_servId;
              char       m_subCycType;
              int        m_condId; 
              CondBase*  m_condRule;
          } m_record;
      
      private:
            static PO_Property m_property;
};

#endif

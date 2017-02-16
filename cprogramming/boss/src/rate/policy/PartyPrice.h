#ifndef PARTYPRICE_H
#define PARTYPRICE_H


#include "dbparam/PO_Common.h"
#include "FieldsXX.h"
#include <map>

class PartyPrice
{
  public:
    
    String<12>      m_productCode;
    String<20>      m_serviceType;
    String<20>      m_partyCorpCode;
    String<21>      m_accessNo;
    String<20>      m_feePlanCode;
    String<2>       m_feePlanState;
    String<14>      m_effDate;
    String<20>      m_billingModeCode;
    String<20>      m_paramCode;
    String<512>     m_paramValue;
    
};

class ParsedPartyPrice
{
  public:
    
    String<12>          m_productCode;
    String<20>          m_billingModeCode;
    String<20>          m_paramCode;
    map<string,string>  m_prodPriceElement;

};

#endif

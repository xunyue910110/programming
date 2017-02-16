#ifndef _DETAILCHANNELITEM_H_
#define _DETAILCHANNELITEM_H_

#include "FieldsXX.h"

const int FACTORS_COUNT = 20;

class DetailChannelItem {
  public:

  DetailChannelItem & operator = (const DetailChannelItem &r)
  {
      int i;
      for (i=0;i < FACTORS_COUNT; ++i)
      {
          m_factors[i] = r.m_factors[i];
      }
      m_channel      =  r.m_channel;
      m_pri          =  r.m_pri;
      m_beginTime    =  r.m_beginTime;
      m_endTime      =  r.m_endTime;
      return *this;
  }

  T_ATTRIBUTE_VALUE   m_channel;
  T_PRI               m_pri;
  T_ATTRIBUTE_VALUE   m_factors[FACTORS_COUNT+1];
  T_DATE_TIME         m_beginTime;
  T_DATE_TIME         m_endTime;
};

#endif // _DETAILCHANNELITEM_H_


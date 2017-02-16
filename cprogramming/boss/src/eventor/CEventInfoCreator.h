#ifndef CEventInfoCreator_h
#define CEventInfoCreator_h

#include "EventCommon.h"
#include "FieldsXX.h"
//#include "PM_Rentfee.h"

#include "BillingCommon.h"
#include "IdInfo.h"

class CEventInfoCreator
{
    public:
        CEventInfoCreator();
        ~CEventInfoCreator();
        
        bool initialize(const T_CYCLE& cycId, PM_Billing *eventParamMgr);
                                   
        bool deal(ReAccountEventInfo& eventInfo);
        
        bool deal(OtpEventInfo& otpEventInfo);
        
        void outReAcct(const int chNo, ostream& os, const int biz_type);
        bool deal(vector<MonfeeSpEventInfo>& MonfeeSpEventInfo);
        void outMonfeeSp(const int chNo, ostream& os, const T_BIZ_TYPE &eventtype=BIZ_TYPE_MONFEE);
        
        void outOtp(const int chNo, ostream& os);
        void getDuration(const String<14>& endDate, const String<14>& startDate,int& dayNum); 
                                   
        PM_Billing                         *m_pEventParamMgr;
        T_CYCLE                             m_minRealCycId;
        //String<8>                           m_dealDate;
        ReAccountEventInfo                  m_eventInfo;
        
        MonfeeSpEventInfo                   m_monfeeEventInfo;
        
        OtpEventInfo                        m_otpEventInfo;
        
        //T_DATE                              m_startDate;
        //T_TIME                              m_startTime;此类型会截断000000字符串为0
        string                              m_startDate;
        string                              m_startTime;
        
        string                               m_endDate;
        string                               m_endTime;
        int                                 m_acctDay; //首长工程增加重出帐归属日期YYYYMMDD
};
#endif

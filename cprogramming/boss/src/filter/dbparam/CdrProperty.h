#ifndef PM_CDRPROPERTY_H

#define PM_CDRPROPERTY_H

#include "config-all.h"
#include <string>
#include "FieldsXX.h"

USING_NAMESPACE(std)


//点对点短信属性
class SmProperty
{
  public:
    void clear()
    {
        number         = "";
        orig_number    = "";
        match_type     = "0";
        serv_type      = "";
        serv_code_type = "";
        home_type      = "0";
        asp            = "0";
        call_type      = "*";
        begin_time     = "";
        end_time       = "";
        sm_typestat    = "";
        priority       = 0;
    }

    const SmProperty & operator =(const SmProperty &a)
    {
       if (this != &a)
       {
           number         = a.number;
           orig_number    = a.orig_number;
           match_type     = a.match_type;
           serv_type      = a.serv_type;
           serv_code_type = a.serv_code_type;
           home_type      = a.home_type;
           asp            = a.asp;
           call_type      = a.call_type;
           begin_time     = a.begin_time;
           end_time       = a.end_time;
           sm_typestat    = a.sm_typestat;
           priority       = a.priority;
       }
   	   return (*this);
    }
        T_MSISDN         number;
        T_MSISDN         orig_number;
        T_MATCH_TYPE     match_type;
        T_SERV_TYPE      serv_type;
        T_SERV_CODE_TYPE serv_code_type;
        T_HOME_TYPE      home_type;
        T_ASP            asp;
        T_CALL_TYPE      call_type;
        T_DATE_TIME      begin_time;
        T_DATE_TIME      end_time;
        T_SM_TYPESTAT    sm_typestat;
        int              priority;
};

//梦网短信属性
class SpProperty
{
  public:
    void clear()
    {
        prov_code        ="" ;
        sp_code          ="" ;
        oper_code        ="" ;
        service_code     ="" ;
        serv_type        ="0";
        serv_code_type   ="" ;
        call_type        = "*";
        home_type        ="0";
        sp_type          ="0";
        asp              ="0";
        is_verifystae    ='N';
        is_verifyorder   ='N';
        sp_serv_area     ='0';
        begin_time       ="" ;
        end_time         ="" ;
        sm_typestat      ="" ;
        priority         = 0;
        sp_name          = "";
    }
    const SpProperty & operator =(const SpProperty &a)
   {
      if (this != &a)
      {
         prov_code        = a.prov_code;
         sp_code          = a.sp_code;
         oper_code        = a.oper_code;
         service_code     = a.service_code;
         serv_type        = a.serv_type;
         serv_code_type   = a.serv_code_type;
         call_type        = a.call_type;
         home_type        = a.home_type;
         sp_type          = a.sp_type;
         asp              = a.asp;
         is_verifystae    = a.is_verifystae;
         is_verifyorder   = a.is_verifyorder;
         sp_serv_area     = a.sp_serv_area;
         begin_time       = a.begin_time;
         end_time         = a.end_time;
         sm_typestat      = a.sm_typestat;
         priority         = a.priority;
         sp_name          = a.sp_name;
      }
 	    return (*this);
   }
        T_PROVINCE_CODE  prov_code;
        T_SP_CODE        sp_code;
        T_OPER_CODE      oper_code;
        T_MSISDN         service_code;
        T_SERV_TYPE      serv_type;
        T_SERV_CODE_TYPE serv_code_type;
        T_CALL_TYPE      call_type;
        T_HOME_TYPE      home_type;
		    T_SP_TYPE        sp_type;
        T_ASP            asp;
        char             is_verifystae;
        char             is_verifyorder;
        char             sp_serv_area;
        T_DATE_TIME      begin_time;
        T_DATE_TIME      end_time;
        T_SM_TYPESTAT    sm_typestat;
        int              priority;
        T_ATTRI_VAL      sp_name; //江苏联通神奇request
};

//彩信属性
class MmsProperty
{
  public:
    void clear()
    {
        prov_code        ="" ;
        sp_code          ="" ;
        oper_code        ="" ;
        service_code     ="" ;
        call_type        ="" ;
        serv_type        ="0";
        serv_code_type   ="" ;
        home_type        ="0";
        sp_type          ="0";
        asp              ="0";
        is_verifystae    ='N';
        is_verifyorder   ='N';
        sp_serv_area     ='0';
        begin_time       ="" ;
        end_time         ="" ;
        mms_typestat     ="" ;
        priority         = 0;
    }
    const MmsProperty & operator =(const MmsProperty &a)
   {
	    prov_code      = a.prov_code;
	    sp_code        = a.sp_code;
	    oper_code      = a.oper_code;
	    service_code   = a.service_code;
	    call_type      = a.call_type;
	    serv_type      = a.serv_type;
	    serv_code_type = a.serv_code_type;
	    home_type      = a.home_type;
	    sp_type        = a.sp_type;
	    asp            = a.asp;
	    is_verifystae  = a.is_verifystae;
	    is_verifyorder = a.is_verifyorder;
	    sp_serv_area   = a.sp_serv_area;
	    begin_time     = a.begin_time;
	    end_time       = a.end_time;
	    mms_typestat   = a.mms_typestat;
	    priority       = a.priority;

	    return *this ;
   }

      T_PROVINCE_CODE  prov_code;
      T_SP_CODE        sp_code;
      T_OPER_CODE      oper_code;
      T_MSISDN         service_code;
      T_CALL_TYPE      call_type;
      T_SERV_TYPE      serv_type;
      T_SERV_CODE_TYPE serv_code_type;
      T_HOME_TYPE      home_type;
	    T_SP_TYPE        sp_type;
      T_ASP            asp;
      char             is_verifystae;
      char             is_verifyorder;
      char             sp_serv_area;
      T_DATE_TIME      begin_time;
      T_DATE_TIME      end_time;
      T_SM_TYPESTAT    mms_typestat;
      int              priority;
};

#endif /* PM_CDRPROPERTY_H */


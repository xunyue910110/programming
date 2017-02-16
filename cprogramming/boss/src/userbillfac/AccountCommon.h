#ifndef ACCOUNTCOMMON_H
#define ACCOUNTCOMMON_H


#include "config-all.h"

#include <ctime>
#include <cstdio>
#include <string.h>
#include <assert.h>

#include <memory>
#include <vector>
#include <string>
#include <map>
#include <utility>
#include <set>

#if defined(STDIOSTREAM_DISABLE)
    #include <fstream.h>
    #include <strstream.h>
    #include <iostream.h>
#else
    #include <fstream>
    #include <strstream>
    #include <iostream>
#endif

#include "base/Types.h"

//定义用户、帐户、客户、帐单标识类型
typedef Number USERID;
typedef Number ACCTID;
typedef Number CUSTID;
typedef Number BILLID;

typedef String<16>   T_SERVER_ID;

typedef String<16>   T_SERVER_ID;

//定义用户、帐户、客户ID类型 added by zhoujie 20050711
//'0':用户 '1':帐户 '2':客户
enum ID_TYPE {USER_ID_TYPE = '0', ACCT_ID_TYPE, CUST_ID_TYPE};

//定义优惠对象类型 added by zhoujie 20050711
//'0':用户 '1':帐户 '2':客户
enum DISCNT_OBJECT_TYPE {USER_OBJECT_TYPE = '0', 
                         ACCT_OBJECT_TYPE, 
                         CUST_OBJECT_TYPE};

//定义无效的用户、帐户、客户、帐单标识值
const USERID   INVALID_USERID = -1;
const ACCTID   INVALID_ACCTID = -1;
const CUSTID   INVALID_CUSTID = -1;
const BILLID   INVALID_BILLID = -1;

//系统费用帐目项最大值及相关常量
const int MAX_ITEM   = 10000;
const int MIN_ITEMID = 1;
const int MAX_ITEMID = 99999;

//系统费用来源最大值及相关常量
const int MAX_SOURCETYPE    = 3;
const int MIN_SOURCETYPETAG = 0;
const int MAX_SOURCETYPETAG = 2;

//系统地市标识最大值及相关常量
const int MAX_EPARCHY   = 20;
const int MIN_EPARCHYID = 1;
const int MAX_EPARCHYID = 19;

//文件记录各列分割符
const char FIELD_SPLIT = '\t';

//定义默认付费帐目编码
const int DEFAULT_PAYITEM_CODE = -1;

//定义数据流量转成费用后全免流量体现的优惠编码
const int REMOVEDATA_DISCNT_ID = -1;

//定义输出文件信息结构
struct OutputFileInfo
{
    //string   m_workFileName;//modify by zhouq
    vector<string> m_workFileName;
    ofstream m_workOfStream;
    //string   m_outputFileName;//modify by zhouq
    vector<string> m_outputFileName;
    //int      m_idNum;//modify by zhouq
    //int      m_recordNum;//modify by zhouq
    int      m_idTotalNum;
    int      m_recordTotalNum;
    vector<int> m_idNum;
    vector<int> m_recordNum;
    int64_t  m_feeSum;
    char     m_fileStat;
};

//定义帐务处理日志结构
struct AccountLogInfo
{
    int      m_idNum;
    int      m_errorIdNum;
    int      m_recordNum;
    int      m_errorRecordNum;
    int64_t  m_fee;
    int64_t  m_filterFee;
};

//定义帐务处理累计信息结构
struct AccountAddupInfo
{
    int     m_idNum;
    int     m_errorIdNum;
    int     m_oweIdNum;
    int     m_recordNum;
    int     m_errorRecordNum;
    int     m_feeSum;
    int     m_afeeSum;
    int     m_bfeeSum;    
    int     m_oweFeeSum;
}; 

//定义帐务处理核查信息结构
struct AccountCheckInfo
{
    string  m_checkContentCode;
    int     m_itemCode;
    int     m_factValue;
    int     m_minValue;
    int     m_maxValue;
    string  m_preValueD;
};

//定义帐期信息结构
struct AcycPara
{
   int      m_acycId;
   int      m_bcycId;
   string   m_acycStartTime;
   string   m_acycEndTime;
   string   m_discountJudgeTime;
   char     m_useTag;
   char     m_addTag;
};

//定义系统参数信息结构
struct SysParamInfo
{
   string   m_sysParamCode;
   int      m_channelNo;
   int      m_valueN;
   string   m_valueS;
   string   m_valueD;
   int      m_preValueN1;
   int      m_preValueN2;
   int      m_preValueN3;
   string   m_preValueS1;
   string   m_preValueS2;
   string   m_preValueS3;
   string   m_preValueD1;
   string   m_preValueD2;
   string   m_preValueD3;
   char     m_useTag; 
};

//定义三户优惠信息
struct IdDiscount
{
    int     m_discountId;
    string  m_startDate;
    string  m_endDate;
    string  m_eparchyCode;
};

//定义用户关系
struct UserRelation
{
    int    m_relationTypeCode;
    USERID m_relationUserId;
    string m_startDate;
  	string m_endDate; 
};

// Add by Basileus 2005/03/21
struct IdInfo
{
    Number m_id;
    char   m_idTypeCode; //0:用户标识 1:帐户标识 2:用户群标识 3:帐户群标识
    string m_openDate;
    string m_removeDate;     
    CUSTID m_custId;
    char   m_userTypeCode;
    string m_brandCode;
    string m_eparchyCode;
    string m_cityCode;
    char   m_accountTag;
    char   m_removeTag;
    string m_preDestroyTime;
    string m_lastStopTime;
    string m_userStateCodeSet;
    int    m_scoreValue;
    int    m_creditValue; 
    int    m_eparchyId;
    char   m_payModeCode;
    int    m_depositPriorityRuleId;
    int    m_itemPriorityRuleId;
    string m_contractNo;
    int    m_userGroupTypeCode;
    string m_serialNumber;
    int    m_advancePay;
    char   m_zeroCallTag;
    string m_addInfoS1;
    string m_addInfoS2;
    string m_addInfoS3;
};
// End by Basileus 2005/03/21

//定义用户信息
struct UserInfo
{
    USERID      m_userId;
    CUSTID      m_custId;
    string      m_brandCode;
    string      m_eparchyCode;
    string      m_cityCode;
    string      m_userStateCodeSet;
    char        m_userTypeCode;
    char        m_accountTag;
    char        m_removeTag;
    string      m_openDate;
    string      m_preDestroyTime;
    string      m_destroyTime;
    string      m_lastStopTime;
    int         m_scoreValue;
    int         m_creditValue;
    string      m_serialNumber;
};

struct CustInfo
{
    CUSTID  m_custId;
    string  m_openDate;
    string  m_destroyDate;
};

/**
 * PayRelation：定义付费关系.
 * 含有下面的方法:
 *   <：比较大小
 */
class PayRelation
{
    public:
        PayRelation() {}
        ~PayRelation() {}
        
        bool operator < (const PayRelation &payRelation) const
        {
            if (m_defaultTag == payRelation.m_defaultTag)
            {
                return (m_acctPriority < payRelation.m_acctPriority);
            }
            else
            {
                return (m_defaultTag < payRelation.m_defaultTag);
            }        
        } 
                               
    public:
        ACCTID    m_acctId;
        int       m_acctPriority;
        int       m_userPriority;
        int       m_payItemCode;
        int       m_limitValue;
        char      m_limitType;
        char      m_complementTag;
        char      m_bindType;
        char      m_defaultTag;
};

//定义帐前调整信息.
struct AdjustBBill
{
    BILLID  m_billId;
    int     m_effectItemCode;
    char    m_effectMode;
    char    m_addupMode;
    char    m_actMode;
    char    m_effectValueType;
    int     m_effectValue;
    int     m_referFee;
    int     m_startAcycId;
    int     m_endAcycId;
    int     m_remainFee;
    int     m_limitFee;
    int     m_adjustReasonId;
};

//定义优惠累计信息.
struct AddupInfo
{
    int    m_discountId;    
    int    m_startAcycId;
    int    m_acycId;       //zhaogc add 20070425
    int    m_effectId;
    int    m_priority;
    int    m_effectItemCode;
    int    m_addupValue;
    int    m_clearTime;
    char   m_addupModeTag;
    char   m_addupValueType;
    char   m_clearModeType;
    char   m_clearAreaType;
    
    //zhaogc add begin 20070425
    bool operator < (const AddupInfo &right) const
    {
        if (m_discountId == right.m_discountId)
        {
            if (m_startAcycId == right.m_startAcycId)
            {
                if (m_effectId == right.m_effectId)
                {                        
                    if (m_priority == right.m_priority)
                    {
                        if(m_addupModeTag == right.m_addupModeTag)
                        {
                        	return (m_addupValueType < right.m_addupValueType);
                        }   
                        else
                        {
                        	return (m_addupModeTag < right.m_addupModeTag);                        	
                        }   
                    }       
                    else
                    {
                        return (m_priority < right.m_priority);
                    }    
                }
                else
                {
                    return (m_effectId < right.m_effectId);
                }       
            }
            else
            {    
                return (m_startAcycId < right.m_startAcycId);
            }    
        }
        else
        {
            return (m_discountId < right.m_discountId);
        }        
    }
    
    bool operator == (const AddupInfo &right) const
    {
        return (m_discountId == right.m_discountId &&
                m_startAcycId == right.m_startAcycId &&
                m_effectId == right.m_effectId &&
                m_priority == right.m_priority &&
                m_addupModeTag == right.m_addupModeTag &&
                m_addupValueType == right.m_addupValueType );
    } 
    //zhaogc add end 20070425       
};

//定义优惠信息
class Discount
{
    public:
        Discount() {}
        ~Discount() {}
                        
        bool operator < (const Discount &discount) const
        {
            if (m_adjustModeType == discount.m_adjustModeType)
            {
                if (m_priority == discount.m_priority)
                {
                    if (m_idTypeCode == discount.m_idTypeCode)
                    {                        
                        if ((m_idTypeCode == '0')|| (m_idTypeCode == '2'))
                        {
                            return (m_discountId < discount.m_discountId); 
                        }       
                        else
                        {
                            return (m_adjustBBillId < discount.m_adjustBBillId);
                        }    
                    }
                    else
                    {
                        return (m_idTypeCode < discount.m_idTypeCode);
                    }       
                }
                else
                {    
                    return (m_priority < discount.m_priority);
                }    
            }
            else
            {
                return (m_adjustModeType < discount.m_adjustModeType);
            }        
        }
                    
    public:
        int     m_discountId;
        BILLID  m_adjustBBillId;
        int     m_startAcycId;
        string  m_startDate;
        string  m_endDate;
        int     m_tariffId;
        int     m_invalidStateId;
        char    m_idTypeCode;
        char    m_adjustModeType;
        int     m_priority;
        char    m_ifShowNegative;
};

//定义优惠明细帐单结构
struct DiscountBill 
{
    USERID  m_subId;
    int     m_detailItemCode;
    int     m_discountFee;
    int     m_discountId;
    char    m_discountType;
};

//定义优惠赠送话费结构
struct PresentMoney
{
    int     m_discountId;
    int     m_depositTypeCode;
    int     m_presentMoney;
};

//定义帐前调整已使用结构
struct AdjustBBillUseInfo
{
    BILLID  m_billId;
    int     m_useFee;
}; 

//定义三户错误信息结构
struct IdErrorInfo
{
    string  m_errorType;
    int     m_fValue;
    int     m_sValue;
};       

//定义明细帐目信息
struct DetailItemInfo
{
    char m_itemType;
    char m_outputType;
};

/**
 * DetailItem：定义明细帐目结构信息.
 * 含有下面的方法:
 *   <：比较大小
 */        
class DetailItem
{
    public:
        DetailItem() {}
        ~DetailItem() {}
        
        bool operator < (const DetailItem& detailItem) const
        {
            if (m_priority == detailItem.m_priority)
            {
                return (m_detailItemCode < detailItem.m_detailItemCode);                
            }
            else
            {
                return (m_priority < detailItem.m_priority);
            }        
        }
                 
    public:
        int  m_detailItemCode;
        int  m_priority;
        
};

//定义明细帐目费用信息.
struct DetailItemFeeInfo
{
    int     m_detailItemCode;
    int     m_ofee;
    int     m_nfee;
    int     m_afee;
    int     m_bfee;
}; 

//定义用户的帐户帐单格式
struct UserAccountBill
{
    ACCTID                      m_acctId;
    int                         m_userPriority;
    char                        m_defaultTag;
    vector<DetailItemFeeInfo>   m_detailItemFeeInfoVector;
};

/**
 * DetailBill：定义帐务优惠使用的明细帐单信息.
 * 含有下面的方法:
 *   <：比较大小
 */ 
class DetailBill
{
    public:
        DetailBill() {}
        ~DetailBill() 
        { 
            m_detailItemFeeInfoVector.clear();
        }
        
        bool operator < (const DetailBill& detailBill) const
        {
            if (m_priority == detailBill.m_priority)
            {
                return (m_subId < detailBill.m_subId); 
            }    
            else
            {
                return (m_priority < detailBill.m_priority);
            }    
        }         
    
    public:
        USERID                      m_subId;      
        int                         m_priority;
        vector<DetailItemFeeInfo>   m_detailItemFeeInfoVector;
};

/**
 * TariffDetailInfo：定义帐务费率明细信息.
 * 含有下面的方法:
 *   <：比较大小
 */         
class TariffDetailInfo
{
    public:
        TariffDetailInfo() {}
        ~TariffDetailInfo() {}
        
        bool operator < (const TariffDetailInfo& tariffDetailInfo) const
        {
            return (m_orderNo < tariffDetailInfo.m_orderNo);
        }              
    
    public:
        int     m_orderNo;
        int     m_effectId;
        int     m_condId;
};

/**
 * TariffEffect：定义帐务费率作用信息.
 * 含有下面的方法:
 *   <：比较大小
 */
class TariffEffect
{
    public:
        TariffEffect() {}
        ~TariffEffect() {}
        
        bool operator < (const TariffEffect& tariffEffect) const
        {
            return (m_priority < tariffEffect.m_priority);
        }
              
    public:
        int     m_priority;
  		int     m_referItemCode;
  		int     m_effectItemCode;
  		char    m_feeSourceTag;
  		char    m_effectMode;
  		char    m_effectValueType;
  		int     m_effectValue;
  		int     m_referValue;
  		char    m_dayFeeRelation;
  		char    m_discountMode;
  		int     m_limitId;
  		int     m_addupId;
  		char    m_addupActType;
  		char    m_effectTimeTag;
  		char    m_negativeShowTag;
  		char    m_effectValidType;
};    

/**
 * TariffEffectLimit：定义帐务费率作用限定信息.
 * 含有下面的方法:
 *   <：比较大小
 */
class TariffEffectLimit
{
    public:
        TariffEffectLimit() {}
        ~TariffEffectLimit() {}
        
        bool operator < (const TariffEffectLimit& tariffEffectLimit) const
        {
            return (m_orderNo < tariffEffectLimit.m_orderNo);
        }                           
// Add by Basileus 2005/02/22         
        bool isValidEffectLimit()
        {
            if ( m_limitModeTag>'7' || m_limitModeTag<'0' || m_limitJudgeValue<0 ||
                 m_limitElemType<'0'||m_limitElemType>'2' ||
                 m_andOrTag>'1' || m_andOrTag<'0' || 
                 m_valueSourceTag>'1' || m_valueSourceTag<'0' )
                return false;
            return true;
        };
        
        bool IfPassJudge(const int& addupValue) const
        {
            if (m_limitModeTag=='0') //限定方式为等于
            {
                if (m_limitJudgeValue!=addupValue)
                    return false;
            }
            if (m_limitModeTag=='1') //限定方式为不等于
            {
                if (m_limitJudgeValue==addupValue)
                    return false;
            }
            if (m_limitModeTag=='2') //限定方式为大于
            {
                if (m_limitJudgeValue>=addupValue)
                    return false;
            }
            if (m_limitModeTag=='3') //限定方式为大于等于
            {
               if (m_limitJudgeValue>addupValue)
                    return false;
            }
            if (m_limitModeTag=='4') //限定方式为小于
            {
                if (m_limitJudgeValue<=addupValue)
                    return false;
            }
            if (m_limitModeTag=='5') //限定方式为小于等于
            {
                if (m_limitJudgeValue<addupValue)
                    return false;
            }                        
            return true;
        };
// End by Basileus 2005/02/22    
    public:
        int     m_orderNo;
        char    m_andOrTag;
        char    m_valueSourceTag;
        int     m_limitItemCode;        
        char    m_limitElemType;
        char    m_limitModeTag;
        int     m_limitJudgeValue;
};

/**
 * TariffEffectAddup：定义帐务费率作用累计信息.
 * 含有下面的方法:
 *   <：比较大小
 */
class TariffEffectAddup
{
    public:
        TariffEffectAddup() {}
        ~TariffEffectAddup() {}        

        bool operator < (const TariffEffectAddup& tariffEffectAddup) const
        {
            return (m_orderNo < tariffEffectAddup.m_orderNo);
        }     

    public:
        int     m_orderNo;
  	    char    m_addupModeTag;
  	    char    m_addupElemType;
  	    char    m_clearModeType;
  	    int     m_clearTime;
  	    char    m_clearAreaType;              
};

/**
 * TariffCondition：定义帐务费率条件信息.
 * 含有下面的方法:
 *   <：比较大小
 */
class TariffCondition
{
    public:
        TariffCondition() {}
        ~TariffCondition() {}
        
        bool operator < (const TariffCondition& tariffCondition) const
        {
            return (m_priority < tariffCondition.m_priority);
        }             
    
    public:
        int     m_priority;
        char    m_andOrTag;
        int     m_memberCondId;
        char    m_memberCondType;
};

//优惠无效的状态信息
class InvalidState
{
    public:
        InvalidState() {}
        ~InvalidState() {}
        
        bool operator < (const InvalidState& invalidState) const
        {
            return (m_priority < invalidState.m_priority);
        }
        
    public:
        int     m_priority;
        string  m_stateCodeSet;
        int     m_acycNum;
        string  m_eparchyCode;
};

//定义帐务费率基本条件信息
struct TariffBaseCondition
{
    char    m_firstValueType;
    int     m_firstValue;
    char    m_judgeMode;
    char    m_secondValueType;
    string  m_secondValue;      
};
// Add by Basileus 2005/03/17
struct MarginOfLimitValue
{
    int     m_limitItemCode;
    char    m_limitElemType;
    int     m_marginValue;
};

struct UserSVCState
{
    USERID  m_userId;
    int     m_serviceId;
    char    m_mainTag;
    char    m_stateCode;
    string  m_startDate;
    string  m_endDate;
};

//定义特殊优惠信息  zhaogc add 
struct SpecificDiscount
{
    Number m_id;
    char   m_idTypeCode; //0:用户标识 1:帐户标识 2:客户标志
    char   m_discntMode;
    int    m_tradeTypeCode;        
    int    m_startAcycId;
    int    m_endAcycId;
    int    m_referItemCode;
    int    m_effectItemCode;
    int    m_valueN1;
    int    m_valueN2;
    int    m_valueN3;
    int    m_valueN4;
    int    m_valueN5;    
};

// End by Basileus 2005/03/17   


// added by lgj for binding one user's fee to another 
class BindRelation
{
  public:
    USERID   m_masterUserId;      //主用户
    USERID   m_subUserId;         //附用户
    char     m_bindTag;           //绑定标志
    //int      m_payItemCode;//预留，按账目项绑定
    //int      m_replaceItemCode; //预留，绑定到主卡指定的账目项上
    int      m_orderNo;           //优先级      
    
    BindRelation()
    {
      m_masterUserId    = 0;
      m_subUserId       = 0;
      m_bindTag         = '0';
      m_orderNo         = 0;
    };
    ~BindRelation()
    {
    };
         
    bool operator <(const BindRelation& obindrelation) const
    {
        if (m_masterUserId==obindrelation.m_masterUserId)
        {
            if (m_orderNo==obindrelation.m_orderNo)
            {
                return (m_subUserId<obindrelation.m_subUserId);
            }
            else
            {
            	  return (m_orderNo<obindrelation.m_orderNo);
            }
        }
        else
        {
            return (m_masterUserId<obindrelation.m_masterUserId);
        }
    };
           
    friend ostream& operator << (ostream& out, const BindRelation& r)
	{
	  out << r.m_masterUserId          << FIELD_SPLIT  //主用户
	      << r.m_subUserId             << FIELD_SPLIT  //附用户
	      << r.m_bindTag               << FIELD_SPLIT
	      << r.m_orderNo               << endl ;       //优先级
	
	  return out ;
	};

};


//end of addition of lgj

//Added by Malone 20060609 start
//定义集团统一付费的明细帐目费用信息.
struct unifypayDetailItemFeeInfo
{
    int     m_detailItemCode;
    int     m_nfee;
    int     m_afee;
    int     m_bfee;
}; 

//Added by Malone 20060609 end

#endif //!ACCOUNTCOMMON_H

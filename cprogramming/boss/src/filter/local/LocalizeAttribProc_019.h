#ifndef __LOCALIZEATTRIBPROC_019_H__
#define __LOCALIZEATTRIBPROC_019_H__

#include <algorithm>
#include "config-all.h"
#include "FilterError.h"
#include "AttribUNProc.h"
#include "LocalizeParamManager_019.h"
#include "LocalConstDef_019.h"

/////////////////////////////////////////////////////////////////
class LocalizeAttribProc019:public AttribUNProc
{
  public:
	virtual int execute();
};

/////////////////////////////////////////////////////////////////
class LocalizeAttribSet:public AttribUNProc
{
  public:
	virtual int execute();
};

/////////////////////////////////////////////////////////////////
class CdrAttribInit:public AttribUNProc
{
  public :
	virtual int execute();
};

/////////////////////////////////////////////////////////////////
class BeforeOtherPartyStand:public AttribUNProc
{
  public :
	virtual int execute();
};

/////////////////////////////////////////////////////////////////
class AfterSetMocLongType:public AttribUNProc
{
  public:
	virtual int execute();
};

////////////////////////////////////////////////////////////////
class AfterForwLongType:public AttribUNProc
{
  public:
	virtual int execute();
};

/////////////////////////////////////////////////////////////////
class BeforeGetVisitAreaCodeA:public AttribProc
{
  public:
	virtual int execute();
};

/////////////////////////////////////////////////////////////////
class AfterGetVisitAreaCodeA:public AttribProc
{
  public:
	virtual int execute();
};

////////////////////////////////////////////////////////////////
class BeforeGetVisitAreaCodeB:public AttribProc
{
  public:
	virtual int execute();
};

////////////////////////////////////////////////////////////////
class AfterGetVisitAreaCodeB:public AttribProc
{
  public:
	virtual int execute();
};

/////////////////////////////////////////////////////////////////
class AfterSetRoamType:public AttribUNProc
{
  public:
	virtual int execute();
};

/////////////////////////////////////////////////////////////////
class SetOriginFeeFlag:public AttribUNProc
{
  public:
	virtual int execute();
};

/////////////////////////////////////////////////////////////////
class CdrAttribEnd:public AttribUNProc
{
  public:
	virtual int execute();
};

/////////////////////////////////////////////////////////////////
class LocalizeErrorCdrProc:public AttribUNProc
{
  public:
	virtual int execute();
};

/////////////////////////////////////////////////////////////////
class IsNoNeedTPSpecialProc:public AttribUNProc
{
  public:
	virtual int execute();
};

/////////////////////////////////////////////////////////////////
class LocalEdgeRoamProc:public AttribUNProc
{
  public:
	virtual int execute();
};


/////////////////////////////////////////////////////////////////
//IP话单类型求取
class LocalizeIPCdrSetProc:public AttribUNProc
{
  public:
	virtual int execute();
    bool otherpartyStanding(); //对端号码标准化
    //获取中继用户类型 TD_TRUNKUSER
    bool getTypeByTrunkUser(const T_ATTRI_VAL &r_phone,
                        const T_DATE_TIME &r_beginTime,
                        T_ATTRI_VAL &r_usertype,
                        T_ATTRI_VAL &r_areacode);
  private :
      ParamContainer<PO_TrunkUser>       *m_PO_TrunkUserContainer;
};

/////////////////////////////////////////////////////////////////
//IP话单区内区间判断
class LocalizeIPPAAreaSetProc:public AttribUNProc
{
  public:
	virtual int execute();
	   bool setSpecialIPAreaType();
     bool setSameAreaType();
     
    //根据用户号段获取中继用户区码 TD_BPS_PSTN_NUMSEG
    bool getBusinessAreaByphone(const T_ATTRI_VAL &r_phone,
            const T_ATTRI_VAL &r_areacode,
            T_ATTRI_VAL &r_businessareacode);
    //根据AB用户区码和交换机归属获取获取用户之间的网络类型 TD_BPS_BUSINESS_AREA_REL
    bool getconnectTypeByABBusiness(const T_ATTRI_VAL &r_areacode,
            const T_ATTRI_VAL &r_businessareacode_a,
            const T_ATTRI_VAL &r_businessareacode_b,
            T_ATTRI_VAL &r_connecttype);
  private :
      ParamContainer<PO_BpsPstnNumseg>   *m_PO_BpsPstnNumsegContainer;
      ParamContainer<PO_BpsRelation>     *m_PO_BpsRelationContainer;
};

/////////////////////////////////////////////////////////////////
class LocalizeIPSwitchSetProc:public AttribUNProc
{
  public:
	virtual int execute();
};

/////////////////////////////////////////////////////////////////
class LocalizeGprsSpecialProc:public AttribUNProc
{
  public:
	virtual int execute();
};

/////////////////////////////////////////////////////////////////
//联通在线求SP名
class LocalizeSpSpecialProc:public AttribUNProc
{
  public:
	virtual int execute();

	bool getSpServCodeName(const T_ATTRI_VAL &r_call_type,
				  const T_ATTRI_VAL &r_sp_code,
				  const T_ATTRI_VAL &r_oper_code,
				  const T_ATTRI_VAL &r_service_code,
				  SpProperty &r_spProperty);
  
  //pps联通在信通过接入号匹配，找到即返回
	bool getSpServCodeName(const T_ATTRI_VAL &r_call_type,
				  const T_ATTRI_VAL &r_service_code,
				  SpProperty &r_spProperty);

  private :
  	  SpProperty m_spProperty;
      ParamContainer<PO_SpServCodeSpname>     *m_PO_SpServCodeSpnameContainer;
      ParamContainer<PO_ServiceCodeSpname>    *m_PO_ServiceCodeSpnameContainer;

  private :
  	class SelfSort
    {
			public:
				bool operator()(const PO_SpServCodeSpname* left,const PO_SpServCodeSpname* right)   
				{
          if(left->recordStruct.priority==right->recordStruct.priority)
          {
						if(left->recordStruct.oper_code=="*" && right->recordStruct.oper_code=="*")
							return false;
						else if(left->recordStruct.oper_code!="*" && right->recordStruct.oper_code=="*")
							return true;
						else if(left->recordStruct.oper_code=="*" && right->recordStruct.oper_code!="*")
							return false;
						else
							return (left->recordStruct.oper_code > right->recordStruct.oper_code);
          }
          else
          {
          	  return (left->recordStruct.priority > right->recordStruct.priority);
          }
				}
    };

};

/////////////////////////////////////////////////////////////////
//增值业务话单校验
class LocalizeSpBillRuleProc:public AttribUNProc
{
  public:
	virtual int execute();
     
    //增值业务话单是否符合规则 TD_SP_BILLRULE
    bool SpCdrValid(const T_ATTRI_VAL &r_source,
            const T_ATTRI_VAL &r_spcode,
            const T_ATTRI_VAL &r_sp_srvtype,
            const T_DATE_TIME &r_begintime);
  private :
      ParamContainer<PO_SpBillRule>   *m_PO_PO_SpBillRuleContainer;
};

/////////////////////////////////////////////////////////////////
//联通在信费用校验
class LocalizeSpOperFeeProc:public AttribUNProc
{
  public:
	virtual int execute();

	//根据SP代码、业务代码和接入码得到信息费定义 TD_SP_MON_OPER
    int getInfoFeeBySpOper(const T_ATTRI_VAL &r_sp_code,
			 const T_ATTRI_VAL &r_oper_code,
			 const T_ATTRI_VAL &r_service_code,
			 const T_ATTRI_VAL &r_fee_type,
			 const T_DATE_TIME &r_begintime,
       int   &r_infofee,
       T_ATTRI_VAL &r_sp_serv_area);

  private :
      ParamContainer<PO_LocalSpOperFee>     *m_PO_LocalSpOperFeeContainer;
};

/////////////////////////////////////////////////////////////////
class BeforeGetFixHomereaCodeA:public AttribUNProc
{
  public:
	virtual int execute();
};

/////////////////////////////////////////////////////////////////
class GetFixHomereaCodeAByTrunkReason:public AttribUNProc
{
  public:
	virtual int execute();
};


/////////////////////////////////////////////////////////////////
class BeforeFixOtherPartyStand:public AttribUNProc
{
  public:
	virtual int execute();
};

/////////////////////////////////////////////////////////////////
class LocalizeFixAttribSet:public AttribUNProc
{
  public:
	virtual int execute();
};

/////////////////////////////////////////////////////////////////
class LocalizeSubAreaTypeSet:public AttribUNProc
{
  public:
	virtual int execute();
};

#endif //__LOCALIZEATTRIBPROC_019_H__

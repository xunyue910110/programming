#ifndef __LOCALIZEATTRIBPROC_034_H__
#define __LOCALIZEATTRIBPROC_034_H__

#include <algorithm>
#include "config-all.h"
#include "FilterError.h"
#include "AttribUNProc.h"
#include "LocalizeParamManager_010.h"
#include "LocalConstDef_010.h"

/////////////////////////////////////////////////////////////////
class LocalizeAttribProc034:public AttribUNProc
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
// Begin add by xiezq 20101207 for 集团BSSV4.1  一卡双号需求
private :
	ParamContainer<PO_MsisdnHk> *m_PO_MsisdnHkContainer;	
	ParamContainer<PO_ImsiHk> *m_PO_ImsiHkContainer;	    
	bool msisdn_hk();
	bool imsi_hk();
// End add by xiezq 20101207 for 集团BSSV4.1  一卡双号需求	
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
};

/////////////////////////////////////////////////////////////////
//IP话单区内区间判断
class LocalizeIPPAAreaSetProc:public AttribUNProc
{
public:
	virtual int execute();
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
private:
    //剔除特定被叫前缀号码话单
    bool trashprefixCalled();
	//规整对端号码，匹配上就去掉对端号码前缀 TD_REGULAR_FIXCALLED
    bool RegularFixCalledPhone(const T_ATTRI_VAL &r_calledareaCode,
		const T_ATTRI_VAL &r_calledPhone,
		const T_ATTRI_VAL &r_msc,
		const T_DATE_TIME &r_beginTime,
		T_ATTRI_VAL &r_dealedPhone);
    //号码互打剔除
    bool trashfixPhonePtp(const T_ATTRI_VAL &r_msc,
		const T_ATTRI_VAL &r_callingPhone,
		const T_ATTRI_VAL &r_calledPhone,
		const T_DATE_TIME &r_beginTime);

	//处理各地市过滤号码，将含有过滤号头的话单trash
	bool FilterFixCalledPhone(const T_ATTRI_VAL &r_area_code,
		const T_ATTRI_VAL &r_msc,
		const T_ATTRI_VAL &r_phone_a,
		const T_ATTRI_VAL &r_phone_b,
		const T_ATTRI_VAL &r_area_code_b,
		const T_DATE_TIME &r_begintime,
		T_ATTRI_VAL &r_serv_type);
private:
	ParamContainer<PO_RegularFixCalled>   *m_PO_RegularFixCalledContainer;
	ParamContainer<PO_NmFixTrashPtp>      *m_PO_NmFixTrashPtpContainer;
};

/////////////////////////////////////////////////////////////////
class LocalizeSubAreaTypeSet:public AttribUNProc
{
public:
	virtual int execute();
};

#endif //__LOCALIZEATTRIBPROC_010_H__

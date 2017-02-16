#ifndef _PM_FILTER_H_200800813_
#define _PM_FILTER_H_200800813_

#include "dbparam/ParamManager.h"
#include "cdr/CdrBase.h"
#include "BillingConstDef.h"

#include "FieldsXX.h"
#include "FilterError.h"
#include "ConstDef.h"
#include "CdrProperty.h"
#include "DetailChannelItem.h"

#include "PO_TimeMethodBind.h"
#include "PO_NetTypeCodeBind.h"
#include "PO_SmServCode.h"
#include "PO_SpServCode.h"
#include "PO_AreaCodeImsiI.h"
#include "PO_HomeAreaCodeI.h"
#include "PO_HomeAreaCodeN.h"
#include "PO_HomeAreaCodeO.h"
#include "PO_HomeAreaCodeP.h"
#include "PO_HomeAreaCodeByPhoneN.h"
#include "PO_HomeAreaCodeBySSP.h"
#include "PO_SpOperFee.h"
#include "PO_UssdServCode.h"
#include "PO_MmsServCodeBySpOper.h"
#include "PO_AspByMsisdnHeader.h"
#include "PO_NrChannelByProvCode.h"
#include "PO_DetailChannel.h"
#include "PO_UserChannel.h"
#include "PO_VirtualArea.h"
#include "PO_HomeAreaCodeC.h"
#include "PO_HomeAreaCodeImsiN.h"
#include "PO_HomeAreaCodeImsiP.h"
#include "PO_MscByAlias.h"
#include "PO_VisitAreaCodeLac.h"
#include "PO_AVByMscCellid.h"
#include "PO_AVByMscLac.h"
#include "PO_AVByMscLacCellid.h"
#include "PO_AVByMsc.h"
#include "PO_AVByLac.h"
#include "PO_AVByCellid.h"
#include "PO_AVByLacCellid.h"
#include "PO_HomeAreaCode.h"
#include "PO_FixTelephone.h"
#include "PO_HomeAreaCodeBySpecialMsisdn.h"
#include "PO_TransNumber.h"
#include "PO_SpecialPhoneGroup.h"
#include "PO_VoiceMagzine.h"
#include "PO_ProvInnerCode.h"
#include "PO_ProvFixTelephone.h"
#include "PO_EdgeCellId.h"
#include "PO_EdgeLacCellId.h"
#include "PO_EdgeMscCellId.h"
#include "PO_EdgeMscLacCellId.h"
#include "PO_EdgeVisitCodeLacCellId.h"
#include "PO_AreaCodeInterAsp.h"
#include "PO_SpecialPhone.h"
#include "PO_B_Object.h"
#include "PO_B_CompCond.h"
#include "PO_B_SimpleCond.h"
#include "PO_B_Eventelem.h"

#include "PO_GprsSgsnAddr.h"
#include "PO_GprsApn.h"
#include "PO_GprsApnniType.h"
#include "PO_SatelCar.h"
#include "PO_Trunk.h"
#include "PO_StateGroup.h"
#include "PO_VistitAreaRoam.h"

#include "PO_HomeAreaCodeFix.h"
#include "PO_ExchangeToSectionFix.h"
#include "PO_FixTrunk.h"
#include "PO_MsisdnSectionFix.h"
#include "PO_SectionRelFix.h"
#include "PO_NbrGroup.h"
#include "PO_NumberConvert.h"
#include "PO_ExchangeDiscount.h"
#include "PO_FixSpecialPhoneSection.h"
#include "PO_JudgeFemtoByLacCellid.h"  //FEMTO

#include "PO_HomeAreaCodeByBureau.h"

#include "MdbParamManager.h"
#include "UserProperty.h"
#include "Mdb_UserInfo_Crm.h"
#include "Mdb_UserImInfobyPhone.h"
#include "Mdb_UserImInfobyImsi.h"
#include "Mdb_UserServState.h"
#include "Mdb_UserOtherInfo.h"
#include "Mdb_VpnInfobyPhone.h"
#include "Mdb_User_Param.h"   //固网中获取市县乡村编码

class PM_Filter:public ParamManager
{
public:
	
	PM_Filter();
	virtual ~PM_Filter();
	
	
	void setHostNameFilter(const string hostName);
	void setAppNameFilter(const string appName);
	void setChannelNoFilter(const int channelNo);
	
	virtual bool addParamObject();
	virtual bool getData();
	bool afterGetData();           //取参数后,初始化一些内部成员
	
private:
	void brCondRuleRela();         //建立外部关联-组合条件
	bool isLoopDefineCondition();  //判断组合条件是否存在循环定义
	bool addCondRule(int &r_ruleId, vector<PO_B_CompCond> &r_compCondList);
	set<int> m_idSet;
	
public:
	//短信中心号码属性求取 TD_SM_SERV_CODE
	bool getSmServCode(const T_MSISDN &r_number,SmProperty &r_smProperty);
	
	//梦网短信参数表 TD_SP_SERV_CODE
	bool getSpServCode(const T_SP_CODE &r_sp_code,const T_OPER_CODE &r_oper_code,SpProperty &r_spProperty);
	
	bool getSpServCode(const T_ATTRI_VAL &r_call_type,
		const T_ATTRI_VAL &r_sp_code,
		const T_ATTRI_VAL &r_oper_code,
		const T_ATTRI_VAL &r_service_code,
		SpProperty &r_spProperty);
	
	//判断用户访问sp是否需要进行稽核 TD_SP_SERV_CODE
	bool getSpOperVerify(const T_ATTRI_VAL &r_sp_code,
		const T_ATTRI_VAL &r_oper_code,
		const T_ATTRI_VAL &r_service_code,
		const T_ATTRI_VAL &r_call_type,
		const T_DATE_TIME &r_begintime,
		char	&r_isVerifyState,
		char	&r_isVerifyOrder);
	
	//根据IMSI判是否国际出访 TD_IMSII
	bool isImsiI(const T_ATTRI_VAL &r_imsi,const T_DATE_TIME &r_begintime,T_ATTRI_VAL &r_homeType);
	
	//根据国际区号获取归属信息 TD_INTER_FEE_CODE
	bool getHomeAreaCodeByMsisdnI(const T_ATTRI_VAL &msisdn,
		const T_DATE_TIME &begintime,
		T_ATTRI_VAL &areaCode,
		T_ATTRI_VAL &groupId1,
		T_ATTRI_VAL &groupIdEx,
		T_ATTRI_VAL &groupIdEx1,
		T_ATTRI_VAL &groupIdEx2,
		T_ATTRI_VAL &groupIdGprs,
		T_ATTRI_VAL &userType,
		T_ATTRI_VAL &nationCode);
	
	//根据国际区号获得长途分组 TD_INTER_FEE_CODE
	bool getGroupIdByAreaCodeI(const T_ATTRI_VAL &area_codeI,
		const T_DATE_TIME &begintime,
		T_ATTRI_VAL &group_id);
	
	//根据国内区号获取归属信息 TD_NATIONAL_FEE_CODE
	bool getHomeAreaCodeByMsisdnN(const T_ATTRI_VAL &msisdn,
		const T_DATE_TIME &begintime,
		T_ATTRI_VAL &areaCode,
		T_ATTRI_VAL &groupId,
		T_ATTRI_VAL &userType);
	
	//根据国内区号找长途分组 TD_NATIONAL_FEE_CODE
	bool getGroupIdByAreaCodeN(const T_ATTRI_VAL &area_codeN,
		const T_DATE_TIME &begintime,
		T_ATTRI_VAL &home_type,
		T_ATTRI_VAL &group_id);
	
	//根据长途区号获取归属信息（城市代码） TD_NATIONAL_FEE_CODE
	bool getCitycodeByAreaCode(T_ATTRI_VAL& citycode,
		const T_DATE_TIME &begintime,
		const T_ATTRI_VAL &areacode,
		T_ATTRI_VAL &provcode,
		T_ATTRI_VAL &groupid,
		T_ATTRI_VAL &usertype);
	
	//根据城市代码获取归属信息（长途区号） TD_NATIONAL_FEE_CODE
	bool getAreacodeByCityCode(const T_ATTRI_VAL& citycode,
		const T_DATE_TIME &begintime,
		T_ATTRI_VAL &areacode,
		T_ATTRI_VAL &provcode,
		T_ATTRI_VAL &groupid,
		T_ATTRI_VAL &usertype);
	
	//根据城市代码获取归属信息（长途区号）联通漫游出访话单会下一个区号多个城市代码 TD_NATIONAL_CODE_ROAMOUT
	bool getAreacodeByCityCodeRoamOut(const T_ATTRI_VAL& citycode,
		const T_DATE_TIME &begintime,
		T_ATTRI_VAL &areacode,
		T_ATTRI_VAL &provcode,
		T_ATTRI_VAL &groupid,
		T_ATTRI_VAL &usertype);
	
	//根据移动非运营商号码获取归属信息 TD_OTHER_MSISDN
	bool getHomeAreaCodeByMsisdnO(const T_ATTRI_VAL &msisdn,
		const T_DATE_TIME &begintime,
		T_ATTRI_VAL &homeAreaCode,
		T_ATTRI_VAL &groupId,
		T_ATTRI_VAL &userType,
		T_ATTRI_VAL &calledType);
	
	//根据本省移动运营商号码获取归属信息 TD_PROV_MSISDN
	bool getHomeAreaCodeByMsisdnP(const T_ATTRI_VAL &msisdn,
		const T_DATE_TIME &begintime,
		T_ATTRI_VAL &homeAreaCode,
		T_ATTRI_VAL &groupId,
		T_ATTRI_VAL &userType,
		T_ATTRI_VAL &officeCode,
		T_ATTRI_VAL &calledType);
	
	//根据移动运营商号码获取归属信息 TD_MSISDN
	bool getHomeAreaCodeByPhoneN(const T_ATTRI_VAL &msisdn,
		const T_DATE_TIME &begintime,
		T_ATTRI_VAL &areaCode,
		T_ATTRI_VAL &groupId,
		T_ATTRI_VAL &userType,
		T_ATTRI_VAL &provinceCode,
		T_ATTRI_VAL &calledType);
	
	//根据国内智能网手机号码获取归属信息 TD_SSP
	bool getHomeAreaCodeBySSP(const T_ATTRI_VAL &msisdn,
		const T_DATE_TIME &begintime,
		T_ATTRI_VAL &areaCode,
		T_ATTRI_VAL &groupId,
		T_ATTRI_VAL &userType,
		T_ATTRI_VAL &officeCode,
		T_ATTRI_VAL &calledType);
	
	//根据SP代码和业务代码得到信息费定义 TD_SP_MON_OPER
    int getInfoFeeBySpOper(const T_ATTRI_VAL &r_sp_code,
		const T_ATTRI_VAL &r_oper_code,
		const T_ATTRI_VAL &r_fee_type,
		const T_DATE_TIME &r_begintime,
		int   &r_infofee);
	
	//根据SP代码、业务代码和接入码得到信息费定义 TD_SP_MON_OPER
    int getInfoFeeBySpOper(const T_ATTRI_VAL &r_sp_code,
		const T_ATTRI_VAL &r_oper_code,
		const T_ATTRI_VAL &r_service_code,
		const T_ATTRI_VAL &r_fee_type,
		const T_DATE_TIME &r_begintime,
		int   &r_infofee);
	
	//根据手机号码头求运营商类型 add by liux 2006-4-6 21:36 TD_MOBILE_SEG
	bool getAspByMsisdnHeader(const T_ATTRI_VAL &r_msisdn,
		const T_DATE_TIME &r_begintime,
		T_ASP &r_asp);
	
	//根据省代码求省内漫游分发的通道号 Added by Sunam 2007-5-28 10:18 TD_PROV_CODE
	bool getNrChannelByProvCode(const T_ATTRI_VAL &provCdoe, int &channelNo);
	
	//从分拣规则表中获取通道 TD_FILTER
	bool getDetailChannelItem(const DetailChannelItem &r_detailChannelItem,
		const int &r_cout,
		T_ATTRI_VAL &r_filterchannel);
	//根据不同数据源直接获取通道 TD_FILTER
	bool getDetailChannelItem(const T_BIZ_TYPE &r_biz_type,
		const DetailChannelItem &r_detailChannelItem,
		const int &r_cout,
		T_ATTRI_VAL &r_filterchannel);
	
	//根据用户USER_ID后四位确定用户通道号 TD_USER_CHANNEL
	bool getChannelNoByUserId(const T_USER_ID &r_userId,
		const T_EPARCHY_CODE &r_eparchyCode,
		int  &r_channelNo);
	
	//判断这两个区号的AREA_GROUP是否相同 TD_VIRTUAL_AREA
	bool isVirtualArea(const T_ATTRI_VAL areaCode1,
		const T_ATTRI_VAL areaCode2,
		const T_ATTRI_VAL  hometype,
		const T_ATTRI_VAL &asp,
		const T_DATE_TIME &begintime);
	
	//根据国际IMSI号获得国际长途区号 TD_IMSII
	bool getAreaCodeByImsiI(const T_ATTRI_VAL &imsi,
		const T_DATE_TIME &begintime,
		T_ATTRI_VAL &areaCode,
		T_ATTRI_VAL &groupId,
		T_ATTRI_VAL &userType,
		T_ATTRI_VAL &interAspCode,
		T_ATTRI_VAL &interAspGroup);
	
	//根据国内IMSI号获得归属长途区号 TD_IMSIN
	bool getHomeAreaCodeByImsiN(const T_ATTRI_VAL &imsi,
		const T_DATE_TIME &begintime,
		T_ATTRI_VAL &homeAreaCode,
		T_ATTRI_VAL &groupId,
		T_ATTRI_VAL &userType,
		T_ATTRI_VAL &officeCode);
	
	//根据省内IMSI号获得归属长途区号 TD_IMSIP
	bool getHomeAreaCodeByImsiP(const T_ATTRI_VAL &imsi,
		const T_DATE_TIME &begintime,
		T_ATTRI_VAL &homeAreaCode,
		T_ATTRI_VAL &groupId,
		T_ATTRI_VAL &userType,
		T_ATTRI_VAL &officeCode);
	
	
	//根据交换机别名获取交换机号 TD_PROV_MSC
	bool getMscByAlias(const T_ATTRI_VAL &msc_alias,
		const T_DATE_TIME &begintime, 
		T_ATTRI_VAL &msc);
	
	//根据LAC求取到访地 TD_LAC
	bool getVisitAreaCodeByLac(const T_ATTRI_VAL &lac,
		const T_DATE_TIME &begintime,
		T_ATTRI_VAL &visitAreaCode,
		T_ATTRI_VAL &officeCode);
	
	//根据MSC和CELL_ID求取到访地 TD_LAC
	bool getAVByMscCellid(const T_ATTRI_VAL &msc,
		const T_ATTRI_VAL &cellid,
		const T_DATE_TIME begintime,
		T_ATTRI_VAL &areacode);
	
	//根据MSC和LAC求取到访地 TD_LAC
	bool getAVByMscLac(const T_ATTRI_VAL &msc,
		const T_ATTRI_VAL &lac,
		const T_DATE_TIME &begintime,
		T_ATTRI_VAL &visitAreaCode);
	
	//根据MSC,LAC和CELL_ID求取到访地 TD_LAC
	bool getAVByMscLacCellid(const T_ATTRI_VAL &msc,
		const T_ATTRI_VAL &lac, const T_ATTRI_VAL &cellId,
		const T_DATE_TIME &begintime,
		T_ATTRI_VAL &visitAreaCode);
	
	//根据MSC求取到访地 TD_PROV_MSC
	bool getAVByMsc(const T_ATTRI_VAL &msc,
		const MSC_TYPE &mscType,
		const T_DATE_TIME &begintime,
		T_ATTRI_VAL &visitAreaCode);
	
	//根据LAC求取到访地 TD_LAC
	bool getAVByLac(const T_ATTRI_VAL &lac,
		const T_DATE_TIME &begintime,
		T_ATTRI_VAL &visitAreaCode);
	
	//根据CELL_ID求取到访地 TD_LAC
	bool getAVByCellid(const T_ATTRI_VAL &cellId,
		const T_DATE_TIME &begintime,
		T_ATTRI_VAL &visitAreaCode);
	
	//根据LAC和CELL_ID求取到访地 TD_LAC
	bool getAVByLacCellid(const T_ATTRI_VAL &lac,
		const T_ATTRI_VAL &cellId,
		const T_DATE_TIME &begintime,
		T_ATTRI_VAL &visitAreaCode);
	
	
	//根据交换机号求取归属地（到访地） TD_MSC
	bool getHomeAreaCodeByM123(const T_ATTRI_VAL &m123,
		const T_DATE_TIME &begintime,
		T_ATTRI_VAL &homeAreaCode,
		T_ATTRI_VAL &groupId,
		T_ATTRI_VAL &userType);
	
	
	//根据号码和长途区号求取对方号码为固定电话的属性（网通、铁通、小灵通) TD_FIX_TELEPHONE
	bool getFixAttrByOtherParty(const T_ATTRI_VAL &msisdn,
		const T_ATTRI_VAL &areaCode,
		const T_DATE_TIME &beginTime,
		T_ATTRI_VAL &userType,
		T_ATTRI_VAL& called_type);
	
	//根据特殊手机号码求取属性 TD_SPECIAL_MSISDN
	bool getHomeAreaCodeBySpecialMsisdn(const T_ATTRI_VAL &msisdn,
		const T_DATE_TIME &begintime,
		T_ATTRI_VAL &areaCode,
		T_ATTRI_VAL &groupId,
		T_ATTRI_VAL &userType,
		T_ATTRI_VAL &provinceCode,
		T_ATTRI_VAL &calledType);
	
	//根据对方号码获得对应的接入号码信息 TD_TRANS_NUMBER
	bool getTransNumber(const T_ATTRI_VAL &msisdn,
		int &length,T_ATTRI_VAL &carrier_type,
		T_ATTRI_VAL &tran_type,
		T_ATTRI_VAL &asp,
		T_ATTRI_VAL &dialType);
	
	//根据对方号码获得对应的接入号码信息 TD_TRANS_NUMBER
	bool getTransNumber(const T_ATTRI_VAL &msisdn,
		int &length,T_ATTRI_VAL &carrier_type,
		T_ATTRI_VAL &tran_type,
		T_ATTRI_VAL &asp,
		T_ATTRI_VAL &dialType,
		T_ATTRI_VAL &isNoNeedTp);
	
	//根据对方号码获得对应的接入号码信息 TD_TRANS_NUMBER FOR CNC SYSTEM add by shenglj 2009-03-26 10:43:00
	bool getTransNumberCNC(const T_ATTRI_VAL &msisdn,
		int &length,T_ATTRI_VAL &carrier_type,
		T_ATTRI_VAL &tran_type,
		T_ATTRI_VAL &asp,
		T_ATTRI_VAL &dialType);
	
	//根据对方号码获得对应的接入号码信息 TD_TRANS_NUMBER FOR CNC SYSTEM add by shenglj 2009-03-26 10:43:00
	bool getTransNumberCNC(const T_ATTRI_VAL &msisdn,
		int &length,T_ATTRI_VAL &carrier_type,
		T_ATTRI_VAL &tran_type,
		T_ATTRI_VAL &asp,
		T_ATTRI_VAL &dialType,
		T_ATTRI_VAL &isNoNeedTp);
	
	//特殊局项 未使用
	bool specialBureau(const T_ATTRI_VAL &msisdn,
		T_ATTRI_VAL &homeAreaCode);
	
	
	//根据号码从特殊号码组表中获得特殊号码组号 TD_SPECIAL_PHONE_GROUP
	bool getSpecialPhoneGroup(const T_ATTRI_VAL &msisdn,
		const T_ATTRI_VAL &area_code,
		const T_DATE_TIME &begintime,
		int &group_id);
	
	//根据号码从特殊号码组表中获得特殊号码组号 TD_SPECIAL_PHONE_GROUP
	//FOR CNC SYSTEM add by shenglj 2009-08-11 21:40:01
	bool getSpecialPhoneGroupCNC(const T_ATTRI_VAL &msisdn,
		const T_ATTRI_VAL &area_code,
		const T_DATE_TIME &begintime,
		int &group_id);
	
	//根据号码从语音杂志表中获得特殊号码组号
	bool getVoiceMagzineAttr(const T_ATTRI_VAL &msisdn,
		const T_ATTRI_VAL &area_code,
		const T_DATE_TIME &begintime,
		int &group_id);
	
	//判断areaCode是否为省内 TD_PROV_INNER_AREA
	bool isProvInnerCode(const T_ATTRI_VAL &areaCode);
	
	//根据省内长途区号求省内地市通道号 TD_PROV_INNER_AREA
	bool getProvChannelByCitYCode(const T_ATTRI_VAL &areaCode, int &channelNo);
	
	//判断otherParty是否是省内固定号码 TD_PROV_FIX_TELEPHONE
	bool isProvFixTelephone(const T_ATTRI_VAL &otherParty,
		const T_DATE_TIME &dateTime);
	
	//根据CELL_ID获取边界漫游标志 TD_EDGE_ROAM
	bool getEdgeByCell(const T_ATTRI_VAL &cellId,
		const T_ATTRI_VAL &homeAreaCode,
		const T_DATE_TIME &begintime,
		T_ATTRI_VAL &edgeFlag);
	
	//根据CELL_ID和到访地区号获取边界漫游标志 TD_EDGE_ROAM
	bool getEdgeByCell(const T_ATTRI_VAL &cellId,
		const T_ATTRI_VAL &homeAreaCode,
		const T_ATTRI_VAL &visitAreaCode,
		const T_DATE_TIME &begintime,
		T_ATTRI_VAL &edgeFlag);
	
	//根据CELL_ID获取边界漫游标志 联通 TD_EDGE_ROAM
	bool getEdgeByCell(const T_ATTRI_VAL &cellId,
		const T_ATTRI_VAL &homeAreaCode,
		const T_DATE_TIME &begintime,
		T_ATTRI_VAL &edgeFlag,
		T_ATTRI_VAL &visit_code);
	
	//根据LAC和CELL_ID获取边界漫游标志 TD_EDGE_ROAM
	bool getEdgeByLacCell(const T_ATTRI_VAL &lac,
		const T_ATTRI_VAL &cellId,
		const T_ATTRI_VAL &homeAreaCode,
		const T_DATE_TIME &begintime,
		T_ATTRI_VAL &edgeFlag);
	
	//根据LAC和CELL_ID获取边界漫游标志 联通 TD_EDGE_ROAM
	bool getEdgeByLacCell(const T_ATTRI_VAL &lac,
		const T_ATTRI_VAL &cellId,
		const T_ATTRI_VAL &homeAreaCode,
		const T_DATE_TIME &begintime,
		T_ATTRI_VAL &edgeFlag,
		T_ATTRI_VAL &visit_code);
	
	//根据MSC和CELL_ID获取边界漫游标志 TD_EDGE_ROAM
	bool getEdgeByMscCell(const T_ATTRI_VAL &msc,
		const T_ATTRI_VAL &cellId,
		const T_ATTRI_VAL &homeAreaCode,
		const T_DATE_TIME &begintime,
		T_ATTRI_VAL &edgeFlag);
	
	//根据MSC和CELL_ID获取边界漫游标志 联通 TD_EDGE_ROAM
	bool getEdgeByMscCell(const T_ATTRI_VAL &msc,
		const T_ATTRI_VAL &cellId,
		const T_ATTRI_VAL &homeAreaCode,
		const T_DATE_TIME &begintime,
		T_ATTRI_VAL &edgeFlag,
		T_ATTRI_VAL &visit_code);
	
	//根据MSC,LAC和CELL_ID获取边界漫游标志 TD_EDGE_ROAM
	bool getEdgeByMscLacCell(const T_ATTRI_VAL &msc,
		const T_ATTRI_VAL &lac,
		const T_ATTRI_VAL &cellId,
		const T_ATTRI_VAL &homeAreaCode,
		const T_DATE_TIME &begintime,
		T_ATTRI_VAL &edgeFlag);
	
	//根据MSC,LAC和CELL_ID获取边界漫游标志 联通 TD_EDGE_ROAM
	bool getEdgeByMscLacCell(const T_ATTRI_VAL &msc,
		const T_ATTRI_VAL &lac,
		const T_ATTRI_VAL &cellId,
		const T_ATTRI_VAL &homeAreaCode,
		const T_DATE_TIME &begintime,
		T_ATTRI_VAL &edgeFlag,
		T_ATTRI_VAL &visit_code);
	
	//特殊边漫处理 根据CELL_ID获取边界漫游标志，如果实际归属和表归属不一致，置特殊边漫标志 TD_EDGE_ROAM
	bool getEdgeByCellRoamOut(const T_ATTRI_VAL &cellId,
		const T_ATTRI_VAL &homeAreaCode,
		T_ATTRI_VAL &visitAreaCode,
		const T_DATE_TIME &begintime,
		T_ATTRI_VAL &edgeFlag);
	
	//特殊边漫处理 根据MSC和CELL_ID获取边界漫游标志，如果实际归属和表归属不一致，置特殊边漫标志 TD_EDGE_ROAM
	bool getEdgeByMscCellRoamOut(const T_ATTRI_VAL &msc,
		const T_ATTRI_VAL &cellId,
		const T_ATTRI_VAL &homeAreaCode,
		T_ATTRI_VAL &visitAreaCode,
		const T_DATE_TIME &begintime,
		T_ATTRI_VAL &edgeFlag);
	
	//特殊边漫处理 根据MSC,LAC和CELL_ID获取边界漫游标志，如果实际归属和表归属不一致，置特殊边漫标志 TD_EDGE_ROAM
	bool getEdgeByMscLacCellRoamOut(const T_ATTRI_VAL &msc,
		const T_ATTRI_VAL &lac,
		const T_ATTRI_VAL &cellId,
		const T_ATTRI_VAL &homeAreaCode,
		T_ATTRI_VAL &visitAreaCode,
		const T_DATE_TIME &begintime,
		T_ATTRI_VAL &edgeFlag);
	
	//特殊边漫处理 根据LAC和CELL_ID获取边界漫游标志，如果实际归属和表归属不一致，置特殊边漫标志 TD_EDGE_ROAM
	bool getEdgeByLacCellRoamOut(const T_ATTRI_VAL &lac,
		const T_ATTRI_VAL &cellId,
		const T_ATTRI_VAL &homeAreaCode,
		T_ATTRI_VAL &visitAreaCode,
		const T_DATE_TIME &begintime,
		T_ATTRI_VAL &edgeFlag);
	
	//Added by Sunam 2006-11-15 10:37
	//省际出访根据A到访地区号,LAC和CELL_ID判断是否边界漫游 TD_EDGE_ROAM (湖南SCP特殊需求)
	bool getEdgeByVisitCodeLacCell(const T_ATTRI_VAL &visitAreaCode,
		const T_ATTRI_VAL &lac,
		const T_ATTRI_VAL &cellId,
		const T_ATTRI_VAL &homeAreaCode,
		const T_DATE_TIME &begintime,
		T_ATTRI_VAL &edgeFlag);
	
	//根据国际运营商代码ASPCODE获取国际长途区号以及国际长途接入字冠组 TD_IMSII
	bool getAVAspCode(const T_ATTRI_VAL &aspCode,
		const T_DATE_TIME &begintime,
		T_ATTRI_VAL &areacode,
		vector<string> &interHead);
	
	
	//根据号码从特殊号码表中求取属性 TD_SPECIAL_PHONE
	bool getSpecialPhone(const T_ATTRI_VAL &msisdn,
	       const T_ATTRI_VAL &area_code_a,
		   const T_ATTRI_VAL &area_code,
		   const T_DATE_TIME &begintime,
		   const PCDR &r_pcdr,
		   T_ATTRI_VAL &user_type,
		   T_ATTRI_VAL &isNoNeedTp,
		   T_ATTRI_VAL &called_type,
		   T_ATTRI_VAL &groupId,
		   T_ATTRI_VAL &telitem,
		   int &length,
		   int &error);
	
	//根据号码从特殊号码表中求取属性 TD_SPECIAL_PHONE FOR CNC SYSTEM add by shenglj 2009-03-26 10:43:00
	bool getSpecialPhoneCNC(const T_ATTRI_VAL &msisdn,
	       const T_ATTRI_VAL &area_code_a,
		   T_ATTRI_VAL &area_code,
		   const T_DATE_TIME &begintime,
		   const PCDR &r_pcdr,
		   T_ATTRI_VAL &user_type,
		   T_ATTRI_VAL &isNoNeedTp,
		   T_ATTRI_VAL &called_type,
		   T_ATTRI_VAL &groupId,
		   T_ATTRI_VAL &telitem,
		   int &length,
		   int &error);
	
	
	//GPRS参数表
	//根据国际长途区号获取国际GPRS分组  TD_INTER_FEE_CODE
	bool getGroupIdGprsByVisitAreaCode(const T_ATTRI_VAL &areaCode,
		const T_DATE_TIME &begintime,
		T_ATTRI_VAL &groupIdGprs);
	
	//获取APNNI分组信息 TD_GPRS_APN
	bool getGprsApnniInfo(const T_GPRS_APNNI &apnni,
		const T_DATE_TIME &begintime,
		T_GPRS_APNGROUP &apnGroup,
		char &servRange);
	
	//获取APNNI分组信息 顺序查找 TD_GPRS_APN
	bool getGprsApnniInfoMatchMode(const T_GPRS_APNNI &apnni,
		const T_DATE_TIME &begintime,
		T_GPRS_APNGROUP &apnGroup,
		char &servRange);
	
	//获取APNNI分类信息 TD_APNNI_GROUP_TYPE
	bool getApnniType(const T_GPRS_APNGROUP &apnGroup,
		T_GPRS_APNTYPE &apnType,
		int &priority);
	
	//根据SGSN地址求用户地区类型 TD_GPRS_SGSN_SEGMENT
	bool getSgsnAddrInfo(const T_DATE_TIME  &begintime,
		const T_GPRS_SGSN  &sgsnAddr,
		T_ATTRI_VAL  &areaCode,
		T_HOME_TYPE  &homeType);
	
	//根据MSC，LAC和CELLID获取用户到访地 TD_SATELCAR
	bool getAVBySatelCar(const T_ATTRI_VAL &msc,
		const T_ATTRI_VAL &lac,
		const T_ATTRI_VAL &cellId, 
		const T_DATE_TIME &beginTime, 
		T_ATTRI_VAL &visitAreaCode);
	
    //获取中继类型 TD_TRUNK
    bool getTypeByTrunk(const T_ATTRI_VAL &msc,
		const T_ATTRI_VAL &trunkid,
		const T_DATE_TIME &begintime,
		const T_ATTRI_VAL &trunkdirection,
		T_ATTRI_VAL &trunktype,
		T_ATTRI_VAL &busitype,
		T_ATTRI_VAL &usertype,
		T_ATTRI_VAL &filterflag);
	
    //获取用户状态分组 TD_STATE_GROUP
    bool getStateGroup(const T_FLAG &r_state,
		T_FLAG &r_stategroup);
	
	
	//根据固网号码获取归属信息 TD_FIXPHONE_SEG
	int getHomeAreaCodeByFixPhone(const T_ATTRI_VAL &phone,
		const T_DATE_TIME &begintime,
		T_ATTRI_VAL &exchangeCode,
		T_ATTRI_VAL &sectionCode,
		T_ATTRI_VAL &homeAreaCode,
		T_ATTRI_VAL &groupId,
		T_ATTRI_VAL &userType,
		T_ATTRI_VAL &officeCode,
		T_ATTRI_VAL &calledType);
	
	//根据局向信息获取营业区信息 TD_EXCHANGE_SECTION_REL
	bool getSectionByExchangeCode(const T_ATTRI_VAL &exchangeCode,
		T_ATTRI_VAL &sectionCode);
	
	//获取固网中继类型 TD_FIX_TRUNK
	bool getTypeByFixTrunk(const T_ATTRI_VAL &r_area_code,
		const T_ATTRI_VAL &r_msc,
		const T_ATTRI_VAL &r_trunkdirection,
		const T_ATTRI_VAL &r_trunkid,
		const T_DATE_TIME &r_begintime,
		T_ATTRI_VAL &r_bill_type,
		T_ATTRI_VAL &r_bill_number,
		T_ATTRI_VAL &r_serv_type);
	
	//获取固网中继类型 TD_FIX_TRUNK
	bool getTypeByFixTrunk(const T_ATTRI_VAL &r_area_code,
		const T_ATTRI_VAL &r_msc,
		const T_ATTRI_VAL &r_trunkdirection,
		const T_ATTRI_VAL &r_trunkid,
		const T_ATTRI_VAL &r_phone_a,
		const T_DATE_TIME &r_begintime,
		T_ATTRI_VAL &r_bill_type,
		T_ATTRI_VAL &r_bill_number,
		T_ATTRI_VAL &r_serv_type);
	
	//根据数据源匹配网别编码 TD_NETTYPE_BINDMETHOD
	bool isCorrectNetBySourceType(const T_ATTRI_VAL &r_source_type,
		const T_ATTRI_VAL &r_net_type_code,
		const T_DATE_TIME &r_begintime);
	
	//根据数据源获取绑定资料时间限制 TD_TIME_BINDMETHOD
	bool getTimeMethodBySourceType(const T_ATTRI_VAL &r_source_type,
		T_FLAG &r_timeMethod);
	
	//add by dph,增加客户资料时间前后偏移量
	//根据数据源获取绑定资料时间限制 TD_TIME_BINDMETHOD
	bool getTimeMethodOffsetBySourceType(const T_ATTRI_VAL &r_source_type,
		T_FLAG &r_timeMethod, int &r_preoffset, int &r_postoffset);
	//end of add by dph,增加客户资料时间前后偏移量
	
	//根据移动号码段获取营业区，一般针对本省移动号码 TD_MSISDN_SECTION
	bool getSectionByMsisdn(const T_ATTRI_VAL &r_area_code,
		const T_ATTRI_VAL &r_msisdn,
		const T_DATE_TIME &r_begintime,
		T_ATTRI_VAL &r_sectionCode);
	
	//根据计费号码的局向、营业区和对端号码的局向、营业区，获取区内区间定义关系 TD_SECTION_REL
	bool getRelationByPhoneandSection(const T_ATTRI_VAL &r_phone_a,
		const T_ATTRI_VAL &r_section_a,
		const T_ATTRI_VAL &r_phone_b,
		const T_ATTRI_VAL &r_section_b,
		const T_ATTRI_VAL &r_area_code_b,
		const T_DATE_TIME &r_begintime,
		T_ATTRI_VAL &r_area_type);
	
	//千群百号,根据号码段获取实际计费号码 TD_NBR_GROUP
	bool getNbrGroupByPhone(const T_ATTRI_VAL &r_phone,
		const T_ATTRI_VAL &r_msc,
		const T_DATE_TIME &r_begintime,
		T_ATTRI_VAL &r_bill_type,
		T_ATTRI_VAL &r_bill_number,
		T_ATTRI_VAL &r_serv_type);
	
	//根据号码获取实际计费号码 TD_NUMBER_CONVERT
	bool getBillNumberByPhone(const T_ATTRI_VAL &r_phone,
		const T_ATTRI_VAL &r_msc,
		const T_DATE_TIME &r_begintime,
		T_ATTRI_VAL &r_bill_type,
		T_ATTRI_VAL &r_bill_number,
		T_ATTRI_VAL &r_serv_type);
	
	//根据主叫号码和被叫号码获取计费类别 TD_EXCHANGE_DISCOUNT
	bool getServTypeByDoublePhone(const T_ATTRI_VAL &r_area_code,
		const T_ATTRI_VAL &r_msc,
		const T_ATTRI_VAL &r_phone_a,
		const T_ATTRI_VAL &r_phone_b,
		const T_DATE_TIME &r_begintime,
		T_ATTRI_VAL &r_serv_type);
	
	//根据主叫号码和被叫号码获取计费类别 TD_EXCHANGE_DISCOUNT
	bool getServTypeByDoublePhone(const T_ATTRI_VAL &r_area_code,
		const T_ATTRI_VAL &r_msc,
		const T_ATTRI_VAL &r_phone_a,
		const T_ATTRI_VAL &r_area_code_b,
		const T_ATTRI_VAL &r_phone_b,
		const T_DATE_TIME &r_begintime,
		T_ATTRI_VAL &r_serv_type);
	
	//根据号码获取所对应的营业区，固网专有 TD_FIX_SPECIALPHONE_SECTION
	bool getSpecialPhoneSection(const T_ATTRI_VAL &phone,
		const T_ATTRI_VAL &area_code_a,
		const T_ATTRI_VAL &area_code,
		const T_DATE_TIME &begintime,
		T_ATTRI_VAL &section_code);
	
	//根据lac和cellid信息求取femto信息 TD_LAC_FEMTO
	bool getFemtoInfoByLacCellid(const T_ATTRI_VAL &lac, const T_ATTRI_VAL &cellId,
		const T_DATE_TIME &begintime,
		T_ATTRI_VAL &r_visitAreaCode,
		T_ATTRI_VAL &r_serviceType);
	
	//protected: 供其他模块用
	//从特殊号码表中找到记录并进行匹配（为了规整）
	bool isSpecialPhoneValid(const SpecialPhoneInfo &r_specialPhoneInfo,
		const PCDR  &r_tpcdr,
		T_ATTRI_VAL &r_user_type,
		T_ATTRI_VAL &r_isNoNeedTp,
		T_ATTRI_VAL &r_called_type,
		T_ATTRI_VAL &r_groupId,
		T_ATTRI_VAL &r_telitem,
		int &r_length,
		int &r_terror);
	
	//根据特殊号码表提供的条件ID去组合条件表以及简单条件表中匹配 TD_B_COMPCOND TD_B_OBJECT TD_B_SIMPLECOND
	bool isConditionValid(const PCDR &r_cdr, const int &r_cond_id, int &r_error);
	
	//递归调用组合条件 TD_B_COMPCOND
	bool getCondRule(const PCDR &r_cdr, const int &r_ruleId, vector<PO_B_CompCond> &r_compCondList, bool &r_result, int &r_error);
	
	//测试条件ID是否成立
	bool test_isConditionValid(const PCDR &r_cdr, const int &r_cond_id, int &r_error);
	
	//根据简单条件表中的判断对象标识ID获取对象定义表中的属性ID TD_B_OBJECT
	bool getAttrIdByObjectId(const int &r_objectId, int &r_AttrId);
	
	//根据对象定义表中的属性ID获取事件元素表中的事件元素关键字(话单下标) TD_B_EVENTELEM
	bool getElementKeyByElementId(const int &r_ElementId, int &r_ElementKey);
	
	//根据特殊号码表中的条件ID从组合条件表中获取最后真正起作用的条件ID TD_B_COMPCOND
	bool getActIDGroupByConID(const int &r_conditionId, vector<int> &v_actionId);
	
	bool getActIDGroupByConID(const int &r_conditionId);
	
	//根据真正起作用的条件ID获取简单条件表的相关信息进行判断 TD_B_SIMPLECOND
	bool getSimpleCondition(const int &r_cond_id,
        T_FLAG &r_yesOrNo,
        int &r_attriId,
        T_FLAG &r_judgeMethod,
        T_FLAG &r_valueType,
        T_ATTRI_VAL &r_minValue,
        T_ATTRI_VAL &r_maxValue,
        vector<string> &r_enumValue);
	
	//根据真正起作用的条件ID获取简单条件表的相关信息进行判断 TD_B_SIMPLECOND
	bool getSimpleCondition(const PCDR &r_cdr, const int &r_cond_id, bool &r_result, int &r_error);
	
	//根据当前月份和基准值推算其他月份
	void addMonth(const long &r_addMonth,long &r_month) const;

  //根据对方号码获得被叫号码归属地 TD_SPECIAL_DIRECTION
  bool getHomeAreaCodeByBureau(const T_ATTRI_VAL& msisdn, const T_DATE_TIME& begintime, 	T_ATTRI_VAL& areaCode);
	
	//modify by dph,将m_PO_ExchangeDiscountContainer变量修改为共有变量
public:
	ParamContainer<PO_ExchangeDiscount>     m_PO_ExchangeDiscountContainer;     //add by shenglj 2009-08-14 11:06:00
	
protected:
	ParamContainer<PO_TimeMethodBind>       m_PO_TimeMethodBindContainer;   //add by shenglj 2009-10-16 11:28:00
	ParamContainer<PO_NetTypeCodeBind>      m_PO_NetTypeCodeBindContainer;  //add by shenglj 2009-04-06 22:33:00
	ParamContainer<PO_SmServCode>           m_PO_SmServCodeContainer;
	ParamContainer<PO_SpServCode>           m_PO_SpServCodeContainer;
	ParamContainer<PO_AreaCodeImsiI>        m_PO_AreaCodeImsiIContainer;
	ParamContainer<PO_HomeAreaCodeI>        m_PO_HomeAreaCodeIContainer;
	ParamContainer<PO_HomeAreaCodeN>        m_PO_HomeAreaCodeNContainer;
	ParamContainer<PO_HomeAreaCodeO>        m_PO_HomeAreaCodeOContainer;
	ParamContainer<PO_HomeAreaCodeP>        m_PO_HomeAreaCodePContainer;
	ParamContainer<PO_HomeAreaCodeByPhoneN> m_PO_HomeAreaCodeByPhoneNContainer;
	ParamContainer<PO_HomeAreaCodeBySSP>    m_PO_HomeAreaCodeBySSPContainer;
	ParamContainer<PO_SpOperFee>            m_PO_SpOperFeeContainer;
	ParamContainer<PO_UssdServCode>         m_PO_UssdServCodeContainer;
	ParamContainer<PO_MmsServCodeBySpOper>  m_PO_MmsServCodeBySpOperContainer;
	ParamContainer<PO_AspByMsisdnHeader>    m_PO_AspByMsisdnHeaderContainer;
	ParamContainer<PO_NrChannelByProvCode>  m_PO_NrChannelByProvCodeContainer;
	ParamContainer<PO_DetailChannel>        m_PO_DetailChannelContainer;
	//分通道继承子模块
	ParamContainer<PO_DetailChannel_GSM>    m_PO_DetailChannelGsmContainer;
	ParamContainer<PO_DetailChannel_SM>     m_PO_DetailChannelSmContainer;
	ParamContainer<PO_DetailChannel_GPRS>   m_PO_DetailChannelGprsContainer;
	ParamContainer<PO_DetailChannel_MISC>   m_PO_DetailChannelMiscContainer;
	ParamContainer<PO_UserChannel>          m_PO_UserChannelContainer;
	ParamContainer<PO_VirtualArea>          m_PO_VirtualAreaContainer;
	ParamContainer<PO_HomeAreaCodeC>        m_PO_HomeAreaCodeCContainer;
	ParamContainer<PO_HomeAreaCodeImsiN>    m_PO_HomeAreaCodeImsiNContainer;
	ParamContainer<PO_HomeAreaCodeImsiP>    m_PO_HomeAreaCodeImsiPContainer;
	
	ParamContainer<PO_MscByAlias>           m_PO_MscByAliasContainer;
	ParamContainer<PO_VisitAreaCodeLac>     m_PO_VisitAreaCodeLacContainer;
	ParamContainer<PO_AVByMscCellid>        m_PO_AVByMscCellidContainer;
	ParamContainer<PO_AVByMscLac>           m_PO_AVByMscLacContainer;
	ParamContainer<PO_AVByMscLacCellid>     m_PO_AVByMscLacCellidContainer;
	ParamContainer<PO_AVByMsc>              m_PO_AVByMscContainer;
	ParamContainer<PO_AVByLac>              m_PO_AVByLacContainer;
	ParamContainer<PO_AVByCellid>           m_PO_AVByCellidContainer;
	ParamContainer<PO_AVByLacCellid>        m_PO_AVByLacCellidContainer;
	ParamContainer<PO_HomeAreaCode>         m_PO_HomeAreaCodeContainer;
	ParamContainer<PO_FixTelephone>         m_PO_FixTelephoneContainer;
	ParamContainer<PO_HomeAreaCodeBySpecialMsisdn> m_PO_HomeAreaCodeBySpecialMsisdnContainer;
	ParamContainer<PO_TransNumber>          m_PO_TransNumberContainer;
	ParamContainer<PO_TransNumber_CNC>      m_PO_TransNumberCNCContainer;  //add by shenglj 2009-03-26 10:43:00
	ParamContainer<PO_SpecialPhone>         m_PO_SpecialPhoneContainer;
	ParamContainer<PO_SpecialPhone_CNC>     m_PO_SpecialPhoneCNCContainer; //add by shenglj 2009-03-26 10:43:00
	ParamContainer<PO_SpecialPhoneGroup>    m_PO_SpecialPhoneGroupContainer;
	ParamContainer<PO_SpecialPhoneGroup_CNC>    m_PO_SpecialPhoneGroupCNCContainer; //add by shenglj 2009-08-11 22:04:00
	ParamContainer<PO_VoiceMagzine>         m_PO_VoiceMagzineContainer;
	ParamContainer<PO_ProvInnerCode>        m_PO_ProvInnerCodeContainer;
	ParamContainer<PO_ProvFixTelephone>     m_PO_ProvFixTelephoneContainer;
	ParamContainer<PO_EdgeCellId>           m_PO_EdgeCellIdContainer;
	ParamContainer<PO_EdgeLacCellId>        m_PO_EdgeLacCellIdContainer;
	ParamContainer<PO_EdgeMscCellId>        m_PO_EdgeMscCellIdContainer;
	ParamContainer<PO_EdgeMscLacCellId>     m_PO_EdgeMscLacCellIdContainer;
	ParamContainer<PO_EdgeVisitCodeLacCellId> m_PO_EdgeVisitCodeLacCellIdContainer;
	ParamContainer<PO_AreaCodeInterAsp>     m_PO_AreaCodeInterAspContainer;
	
	ParamContainer<PO_B_Object>             m_PO_B_ObjectContainer;
	ParamContainer<PO_B_CompCond>           m_PO_B_CompCondContainer;
	ParamContainer<PO_B_SimpleCond>         m_PO_B_SimpleCondContainer;
	ParamContainer<PO_B_Eventelem>          m_PO_B_EventelemContainer;
	
	ParamContainer<PO_GprsSgsnAddr>         m_PO_GprsSgsnAddrContainer;
	ParamContainer<PO_GprsApn>              m_PO_GprsApnContainer;
	ParamContainer<PO_GprsApnniType>        m_PO_GprsApnniTypeContainer;
	ParamContainer<PO_SatelCar>             m_PO_SatelCarContainer;
	ParamContainer<PO_Trunk>                m_PO_TrunkContainer;
	ParamContainer<PO_StateGroup>           m_PO_StateGroupContainer;
	ParamContainer<PO_VistitAreaRoam>       m_PO_VistitAreaRoamContainer;
	
	ParamContainer<PO_HomeAreaCodeFix>      m_PO_HomeAreaCodeFixContainer; //add by shenglj 2009-03-26 10:43:00
	ParamContainer<PO_FixTrunk>             m_PO_FixTrunkContainer;        //add by shenglj 2009-03-
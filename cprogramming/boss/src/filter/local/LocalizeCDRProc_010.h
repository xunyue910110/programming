#ifndef __LOCALIZECDRPROC_034__
#define __LOCALIZECDRPROC_034__

#include "config-all.h"
#include "FilterError.h"
#include "CDRProc.h"

#include "LocalizeParamManager_010.h"
#include "MdbParamManager.h"
#include "LocalConstDef_010.h"

/////////////////////////////////////////////////////////////////
class LocalizeGsmTrashGet:public CDRProc
{
public:
	virtual int execute();
};

/////////////////////////////////////////////////////////////////
class LocalizeSmTrashGet:public CDRProc
{
public:
	virtual int execute();
};

/////////////////////////////////////////////////////////////////
class LocalizeGprsTrashGet:public CDRProc
{
public:
	virtual int execute();
};

/////////////////////////////////////////////////////////////////
class LocalizeMiscTrashGet:public CDRProc
{
public:
	virtual int execute();
};

/////////////////////////////////////////////////////////////////
class LocalizeIPTrashGet:public CDRProc
{
public:
	virtual int execute();
};

/////////////////////////////////////////////////////////////////
class LocalizeSmCdrGet:public CDRProc
{
public:
	virtual int execute();
};

/////////////////////////////////////////////////////////////////
class LocalizeFixSmCdrJudge:public CDRProc
{
public:
	virtual int execute();
};

/////////////////////////////////////////////////////////////////
class LocalizeProvCdrGet:public CDRProc
{
public:
	virtual int execute();
};

/////////////////////////////////////////////////////////////////
class LocalizeMiscNormalSet:public CDRProc
{
public:
	virtual int execute();
};

/////////////////////////////////////////////////////////////////
class LocalizeProvTrashGet:public CDRProc
{
public:
	virtual int execute();
};

//add by shenglj 2009-04-13 11:11:01
/////////////////////////////////////////////////////////////////
class LocalizeProvScpTrashGet:public CDRProc
{
public:
	virtual int execute();
};

/////////////////////////////////////////////////////////////////
class LocalizeChannelReset:public CDRProc
{
public:
	virtual int execute();
};

/////////////////////////////////////////////////////////////////
//根据资料中网别编码挑单
class LocalizeProvByNetType:public CDRProc
{
public:
	virtual int execute();
};

/////////////////////////////////////////////////////////////////
class LocalizeProvRegionInfoGet:public CDRProc
{
public:
	virtual int execute();
private:
	bool ScdmatrashfixPhone(const T_ATTRI_VAL &r_msc,
		const T_ATTRI_VAL &r_callingPhone,
		const T_ATTRI_VAL &r_calledPhone,
		const T_DATE_TIME &r_beginTime);
	ParamContainer<PO_NmFixTrashPtp>      *m_PO_ScdmaFixTrashContainer;
};

/////////////////////////////////////////////////////////////////
class LocalizeFixGsmTrashGet:public CDRProc
{
public:
	virtual int execute();
};

/////////////////////////////////////////////////////////////////
class LocalizeFixGsmNormalSet:public CDRProc
{
public:
	virtual int execute();
};

/////////////////////////////////////////////////////////////////
class LocalizeFixGsmgetTrunkProperty:public CDRProc
{
public:
	virtual int execute();
};

/////////////////////////////////////////////////////////////////
class LocalizeFixGsmSameMscCheck:public CDRProc
{
public:
	virtual int execute();
};

/////////////////////////////////////////////////////////////////
class LocalizeFixGsmSetBillNumber:public CDRProc
{
public:
	virtual int execute();
};

/////////////////////////////////////////////////////////////////
class LocalizeFixGsmTrashNumberNbr:public CDRProc
{
public:
	virtual int execute();
private:
	//根据号码选择端局话单 TD_TJ_SELECT_NBR
    bool chooseSourceByPhone(const T_ATTRI_VAL &r_callingPhone,
		const T_DATE_TIME &r_beginTime);
	//根据号码剔除汇接局话单 TD_TJ_TRASH_NBR
    bool trashSourceByPhone(const T_ATTRI_VAL &r_callingPhone,
		const T_DATE_TIME &r_beginTime);
private:
	//ParamContainer<PO_TjSelectNbr>   *m_PO_TjSelectNbrContainer;
	//ParamContainer<PO_TjTrashNbr>    *m_PO_TjTrashNbrContainer;
};
#endif //__LOCALIZECDRPROC_034__
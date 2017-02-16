#ifndef __LOCALIZECDRPROC_019__
#define __LOCALIZECDRPROC_019__

#include "config-all.h"
#include "FilterError.h"
#include "CDRProc.h"

#include "LocalizeParamManager_019.h"
#include "MdbParamManager.h"
#include "LocalConstDef_019.h"

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
  	MdbParamManager m_mdbMgr;
	bool isIvpnUser();  	
	bool userHaveNetTypeCode(const char * r_net_type_code);
	//wujg add 20090531
	bool userHaveUserTypeCode(const char * r_user_type_code);
  virtual int execute();
};

/////////////////////////////////////////////////////////////////
class LocalizeProvRegionInfoGet:public CDRProc
{
  public:
  virtual int execute();
  bool getShanxiFixCdrType(const T_ATTRI_VAL &r_m_bizType,
          const T_ATTRI_VAL &r_sourceType,
          const T_ATTRI_VAL &r_m_msc,
          const T_ATTRI_VAL &m_trunkGroupIn,
          const T_ATTRI_VAL &r_callType,
          const T_ATTRI_VAL &r_userAspB,
          const T_ATTRI_VAL &r_UserServiceTypeB,
          const T_ATTRI_VAL &r_paType,
          const T_ATTRI_VAL &r_longType,
          const T_ATTRI_VAL &r_longGroup,
          const T_ATTRI_VAL &r_carrierType,
          const T_ATTRI_VAL &r_homeCodeA,
          const T_ATTRI_VAL &r_homeCodeB,
          T_ATTRI_VAL &r_recordType);
  private:
   ParamContainer<PO_LocalCdrType>    *m_PO_LocalCdrTypeContainer;
  private:
  	class LocalCdrTypeFixSort
    {
			public:
				bool operator()(const PO_LocalCdrType* left,const PO_LocalCdrType* right)   
				{
          if(left->recordStruct.m_priority==right->recordStruct.m_priority)
          {
						return (left->recordStruct.m_UserServiceTypeB > right->recordStruct.m_UserServiceTypeB);
          }
          else
          {
            return (left->recordStruct.m_priority > right->recordStruct.m_priority);
          }
				}
    };

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

#endif //__LOCALIZECDRPROC_019__


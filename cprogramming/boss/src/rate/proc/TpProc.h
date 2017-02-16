#ifndef __TPPROC_H__20080729_
#define __TPPROC_H__20080729_

#include "config-all.h"
#include "RateGlobalInfo.h"
#include "Procedure.h"
#include "FieldsXX.h"   
#include "RateConstDef.h"   
#include "TpInfo.h"

// 费率政策属性求解过程
class TpProc:public Procedure 
{
  public:
  	virtual int execute();
  	virtual int reset();
  	
    virtual bool setTpParamInfo();    
    //取基本资费政策
  	virtual bool getBaseTpList();
  	//取用户所有优惠套餐
  	virtual bool getUserTpList();
    //取基本优惠政策
  	virtual bool getAllBaseTariff();
  	//判断是否需要做计费政策(套餐)
  	virtual bool isNoNeedTP();
  	
  //#ifdef __PROV019__  //如果是山西	
    //获取默认系统资费
  	virtual bool getDefaultTariff();//add by cuiyw for shanxi At 2009-12-04 20:36:50
  //#endif	
    //virtual void setInNetFlag();
  public:
  	//取基站基本优惠政策
    bool getBaseCellTp();

  protected:
    void setFreeFlag();
    void setGsmFreeFlag();
    void setFixVFreeFlag();
  protected:
    vector<T_TP_ID>  m_baseTpList; //计费基本政策列表
    
  	bool m_hasTp[FEEFIELDS_MAX];//该项是否有政策
};          
   
#endif //__TPPROC_H__20080729_

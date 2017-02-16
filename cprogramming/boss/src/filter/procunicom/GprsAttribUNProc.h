#ifndef __GPRSATTRIBUNPROC_H__
#define __GPRSATTRIBUNPROC_H__

#include "config-all.h"

#include "AttribUNProc.h"


class GprsAttribUNProc:public AttribUNProc
{
  public:
    virtual int execute();
    
  protected:
	void setRoamType(void);
	bool getGprsVisitAreaCodeA(void);

	//added by gaojf  10/17/2006 8:37AM 为内容计费
	void setRoamTypeGcdr(void);       //设置GCDR的漫游类型 
	void setRoamTypeScdr(void);       //设置GCDR的漫游类型 
	bool getGcdrVisitAreaCodeA(void); //求取GCDR的到访地    
	bool getScdrVisitAreaCodeA(void); //求取SCDR的到访地   
	bool isValidGprsCdr();            //判断是否有效的GPRS话单
	//检验是否有效的APN(国内来访检验APNNI，国际来访检验APNOI)
	//省内用户校验APNNI(包括国际出访)
	bool isValidApn(); 
	T_HOME_TYPE m_sgsnHomeType;  //SGSN归属类型
	T_ATTRI_VAL m_visitProvCode; //SGSN归属省代码
  protected:
};


#endif


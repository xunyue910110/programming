#include "RateCdr.h"
#include "RateError.h"
#include "RateConstDef.h"
#include "Procedure.h"

RateCdr::RateCdr()
{
}
RateCdr::~RateCdr()
{

}
//读取话单前一些属性初始化
bool RateCdr::reset() 
{
  bool t_bRet = true;
  if(CdrBase::reset()==false)
  {
    t_bRet = false;
  }
  m_tatolFee = 0;
  m_totalDuration = 0;
  m_totalData = 0;
  m_baseFee   = 0;
  return t_bRet;
}
//读取话单后规整
bool RateCdr::canonicalize()
{
  bool t_bRet = true;
  if(CdrBase::canonicalize()==false)
  {
    t_bRet = false;
  }
  
  m_attrList[ATTRI_NEED_BASETP]= FLAG_YES;
  m_attrList[ATTRI_NEED_USERTP]= FLAG_YES;
  m_attrList[ATTRI_FREE_FEE1]  = FLAG_NO ;
  m_attrList[ATTRI_FREE_FEE2]  = FLAG_NO ;
  m_attrList[ATTRI_FREE_FEE3]  = FLAG_NO ;
  m_attrList[ATTRI_FREE_FEE4]  = FLAG_NO ;
  m_attrList[ATTRI_FREE_FEE5]  = FLAG_NO ;

	//3.本地化的话单初始化过程
  ProcedureHelper::ExecuteProcedure("RateCdrAttribInit");
  
  
  return true;
}
bool RateCdr::finalize()
{
  bool t_bRet = true;
  if(CdrBase::finalize()==false)
  {
    t_bRet = false;
  }
  
	ProcedureHelper::ExecuteProcedure("RateCdrAttribEnd");
	
  m_baseFee       = m_attrList[ATTRI_FEE_ALL].value();
  m_tatolFee      = m_attrList[ATTRI_DISC_ALL].value();
  m_totalDuration = m_attrList[ATTRI_DURATION].value();
  m_totalData     = m_attrList[ATTRI_SP_TOTALDATA].value();
  
  return true;
}


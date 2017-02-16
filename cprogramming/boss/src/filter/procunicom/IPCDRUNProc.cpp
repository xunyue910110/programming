#include "IPCDRUNProc.h"

#include "FilterConfig.h"
#include "PMGlobalManager.h"

INSTALL_PROCESSBASE_INSTANCE(IPCDRUNProc);

bool IPCDRUNProc::InitCdrProperty()
{
	reset();
	//保留原始主叫号码
	m_pcdr[ATTRI_MSISDN_A_ORG] = m_pcdr[ATTRI_MSISDN_A];
	return true;
}

bool IPCDRUNProc::getTrash()
{
	 int t_iRet;
	 //本地化ip trash判断
	 t_iRet=ProcessBaseHelper::ExecuteProcedure("LocalizeIPTrashGet");
	 if(t_iRet==0)
	 {
	 	  return false;
	 }
	 else if(t_iRet==1)
	 {
      m_pcdr[ATTRI_TRASH_FLAG]=ATTRI_TRASH_Y;
      return true;
	 }

   return false;
}

bool IPCDRUNProc::getDisChannelProperty()
{
	//1. 无效话单
	if(getTrash())
    return true;

	//2. 错误话单
	if(getError())
		 return true;

  //3. 求话单类型
  ProcessBaseHelper::ExecuteProcedure("LocalizeIPCdrSetProc");
 
	return true;
}



int IPCDRUNProc::execute()
{
 #ifdef FP_DEBUG
   FP_BEGIN(IPCDRUNProc)
 #endif

 InitCdrProperty();

 //求分通道必要属性
 getDisChannelProperty();

 #ifdef FP_DEBUG
   FP_END(IPCDRUNProc)
 #endif
  return 0;
}

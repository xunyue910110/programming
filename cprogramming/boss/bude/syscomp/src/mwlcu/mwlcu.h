#ifndef EBPM4C_MW_LCU_H
#define EBPM4C_MW_LCU_H
#include "mwci/mwci.h"
#include "lcu/lcu.h"
#include "mwlcu/lcuplugins.h"
#include "mwlcu/fmlcontext.h"
#include "log4cpp/Logger.h"

#define LCU_FML_PROCESS_NAME    "X_TRANS_CODE"
#define LCU_FML_RESULTCODE      "X_RESULTCODE"
#define LCU_FML_RESULTERRINFO   "X_RESULTINFO"
#define LCU_FML_LAST_RESULTCODE      "X_LAST_RESULTCODE"
#define LCU_FML_LAST_RESULTERRINFO   "X_LAST_RESULTINFO"


//员工号
#define LCU_FML_DEF_STAFFID        "TRADE_STAFF_ID"  
//部门号
#define LCU_FML_DEF_DEPATID        "TRADE_DEPART_ID" 

//地市号
#define LCU_FML_DEF_EPARCHY_CODE   "TRADE_EPARCHY_CODE"  
//city号
#define LCU_FML_DEF_CITY_CODE      "TRADE_CITY_CODE" 


//流水号
#define LCU_FML_DEF_FLOWID       "TRADE_ID"  
//回退方法名的前缀
#define LCU_METHOD_UNDO_PREF     "Undo"
//回退用的Buff
#define LCU_FML_UNDOBUFFER_NAME  "undobuffer"

//当inBuffer中包含UNDOFLAG且等于UNDOFLAG_VALUE 则这是一个Undo流程 "CALLFLAG"
#define LCU_FML_UNDOFLAG         "X_LCU_UNDOFLAG"  
                            

//当UNDOFLAG的值等于UNDOFLAG_VALUE的时候为执行回退流程
#define LCU_FML_UNDOFLAG_VALUE   "UNDO"

//每一步的步骤序号step值为整型从1~n  "TAG"
#define LCU_FML_LOGSTEP_NAME     "X_LCU_LOGSTEP"   

//每进程间用XA方式提交事务,XA_FLD记录当前进程的深度,调一个进程加一,
//离开一个进程减一,离开时发现深度是0且在仍然事务中，就提交事务  "UPDATESERVICCOUNT"
#define LCU_TXDCALL_COUNT        "X_LCU_TXDSERV_COUNT"  
#define LCU_LOG_TRADE            "gboss.crm.trade."
#define LCU_LOG_TRACE            "gboss.lcu.trace"


using namespace log4cpp; 

struct TPSVRINFO {	 
    string  SRVNAME;  //-C mwservercrm1   
	string  DOMAINID; //-g dom=gbossapp
    string  GROUPID;  //-i 140
    string  SRVID ;   //-u 402
    string  HOSTID;   //-U sun220  
    vector <string>  CONFINFO; //配置信息
    int     SRVPID;   //进程pid
};
//class MWLCUImpl;

class MWLCU {

protected: 	
   MWLCU(){
   }
   MWLCU(const MWLCU&){
   }  
   virtual ~MWLCU(){
   }
private:
   MWLCU& operator = (const MWLCU&){
      return *this;
   }
   
public:
  
   static MWLCU& getInstance();
   
   //服务进程启动代码，加载资源
   virtual int tpsvrinit(int argc,char ** argv)=0;
      //初始化引擎
   virtual void initEngine()=0; 
   virtual PDefinitionLoaderDao getDefinitionsLoader()=0;
    //请求Buffer名
   virtual string getRequestName(const string&  processname)=0;  
   //回复Buffer名 
   virtual string getResponseName(const string&  processname)=0;
    /**
   * 执行系统定义的流程
   * @param inBuf 输入PCFmlBuf参数作为请求Buffer
   * @return PCFmlBuf 回复Buffer
   */   
   virtual PCFmlBuf execute(PCFmlBuf inBuf)=0;
   //执行子流程,子流程不需要开事务
   virtual PCFmlBuf subProcessExecute(const string& processname,FMLContext& ctx,PCFmlBuf inBuf)=0;
  
   //执行Undo  
   virtual PCFmlBuf undoExecute(const string& processname,FMLContext& ctx)=0;
   //执行Undo流程 processindex 为0是主流程
   virtual void undoProcessExecute(const string& processname,int processindex,FMLContext& ctx)=0;
public:
   
   //是否在事务中 
   virtual bool isInTransXA()=0;
   //是否在本地事务中
   virtual bool isInTransLocal()=0;
   //已经在事务中，就不开事务	
   virtual void transBegin(int transLevel)=0;
   //iscommit = false 则 rallback
   virtual void transCommit(bool iscommit,FMLContext& fmlctx)=0;
    //0=不要事务,1=本地事务,2=XA事务
   virtual int getTransLevel(const string& processname)=0;   
};



#endif //EBPM4C_MW_LCU_H


//---------------------------------------
//Title  : sotest for MWLCU 
//Author : juyf@lianchuang.com  
//Date   : 2005/01/25
//---------------------------------------

#ifndef  SOTEST_MW_LCU_H
#define  SOTEST_MW_LCU_H
#include "mwci.h"
#include "udbi.h"
#include "config-all.h"
#include "smartptr.h"
#include <memory>
#include <string>
#include "DynamicLibrary.h"
#include <stdexcept>

#define LCU_FML_PROCESS_NAME    "TRANSCODE"
#define LCU_FML_RESULTCODE      "RESULTCODE"
#define LCU_FML_RESULTERRINFO   "RESULTERRINFO"

//流水号
#define LCU_FML_DEF_FLOWID       "TRADEID"
//回退方法名的前缀
#define LCU_METHOD_UNDO_PREF     "Undo"
//回退用的Buff
#define LCU_FML_UNDOBUFFER_NAME  "undobuffer"

//当inBuffer中包含UNDOFLAG且等于UNDOFLAG_VALUE 则这是一个Undo流程 "CALLFLAG"
#define LCU_FML_UNDOFLAG         "LCU_UNDOFLAG"  
                            
//当UNDOFLAG的值等于UNDOFLAG_VALUE的时候为执行回退流程
#define LCU_FML_UNDOFLAG_VALUE   "UNDO"

//每一步的步骤序号step值为整型从1~n  "TAG"
#define LCU_FML_LOGSTEP_NAME     "LCU_LOGSTEP"

//每进程间用XA方式提交事务,XA_FLD记录当前进程的深度,调一个进程加一,
//离开一个进程减一,离开时发现深度是0且在仍然事务中，就提交事务
#define LCU_TXDCALL_COUNT        "LCU_TXDSERV_COUNT"  

//此定义是借用
#define LCU_TRANSABLE_NAME        "SERVTYPE"  


    /** @stereotype exception */
    class definition_error : public runtime_error {
    public:
        explicit definition_error(const string & msg);
        virtual ~definition_error() throw();
    };
   
    /** @stereotype exception */
    class execution_error : public runtime_error {
    public:
        explicit execution_error(const string & msg);
        virtual ~execution_error() throw();
    };


   
   typedef counted_ptr < CFmlBuf >  PCFmlBuf;
   
   class MWSOTEST {   
   private :    
      static MWSOTEST MWSOTESTInstance;  
      MWSOTEST(){};
      MWSOTEST(const MWSOTEST&){};
      MWSOTEST& operator = (const MWSOTEST&){
         return *this;
      };
   
      //是否在事务中 
      bool isInTransXA(); 
      //是否在本地事务中
      bool isInTransLocal();   
      //已经在事务中，就不开事务	
      void transBegin(int transLevel);
      //iscommit = false 则 rallback
      void transCommit(bool iscommit,int ctx);
   
      DynamicLibraryManager manager;     
   
   public:   
    
      static MWSOTEST& getInstance(){     
         return MWSOTEST::MWSOTESTInstance;
      }
      /**
      * 执行系统定义的流程
      * @param inBuf 输入PCFmlBuf参数作为请求Buffer
      * @return PCFmlBuf 回复Buffer
      */   
      PCFmlBuf testExecute(PCFmlBuf inBuf);
      //0=不要事务,1=本地事务,2=XA事务
      int getTransLevel(const string& processname);
      //是否需要事务   
      bool isTransable(const string& processname);    
      //是否需要回退   
      bool isUndoable(const string&  processname);
   
   };
   
   
   typedef void(* Invoker) (CFmlBuf& request, CFmlBuf& response);
   typedef void(* UndoInvoker) (CFmlBuf& undobuffer);
   
   //动态连接库调用处理
   class SOActionHandle {
   private:
          DynamicLibraryManager * manager;
          string libname;
          string symbolname;
          string requestparam;
          string responseparam;
          
   public:  
          SOActionHandle(DynamicLibraryManager * manager_) ;
          ~SOActionHandle();  
          PCFmlBuf execute(PCFmlBuf inBuf, bool isUndo = false);
          void configure(const string & configuration);
   };

#endif 


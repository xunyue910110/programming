#ifndef EBPM4C_LCU_PLUGINS_HOME_H
#define EBPM4C_LCU_PLUGINS_HOME_H
#include "mwci/mwci.h" 
#include <iostream>
#include <memory>
#include "dynlib/DynamicLibrary.h"
#include "lcu/lcu.h"
#include "mwlcu/fmlcontext.h"
#include "mwlcu/udbidbloader.h"
#include "mwlcu/udbidblog.h"
#include "lcu/decidetoolkit.h"
#include "mwlcu/fmloperator.h"

#include <unistd.h>
#include <string>

USING_NAMESPACE(std);

using namespace ebpm4c;
//using namespace log4cpp;

class DefaultHandleFactory;
class DLActionHandle;
class TuxedoCallActionHandle;
class DefaultDecisionHandle;

typedef  counted_ptr < DefaultHandleFactory > PDefaultHandleFactory;
typedef  counted_ptr < UDBIDBDefinitionsLoader > PUDBIDBDefinitionsLoader;

typedef void(* Invoker) (CFmlBuf& request, CFmlBuf& response);
typedef void(* UndoInvoker) (CFmlBuf& undobuffer);

//动态连接库调用处理
class LCUAdminActionHandle : public ActionHandle {
private:
       string cmd;
       string paramtype;
       string param; 
public:   
       LCUAdminActionHandle();    
       ~LCUAdminActionHandle();    
       void execute(Context& ctx);
       void configure(const string & configuration);
};

//动态连接库调用处理
class DLActionHandle : public ActionHandle {
private:	
       DynamicLibraryManager * manager;
       string libname;
       string symbolname;
       string requestparam;
       string responseparam;
       bool ignoreerror;
       int undoother;
       string undootherserver;
public:
       DLActionHandle(DynamicLibraryManager * manager_) ;
       ~DLActionHandle();    
       void execute(Context& ctx);
       void configure(const string & configuration);
};



//TuxedoCaller 本地服务处理
class TuxedoCallerActionHandle : public ActionHandle {
private :	
   	   string symbolname;
       string requestparam;
       string responseparam;
       bool ignoreerror;
       int undoother;
       string undootherserver;	
public :
       TuxedoCallerActionHandle();    
       ~TuxedoCallerActionHandle();    
   	   void execute(Context& ctx);		
       void configure(const string & configuration);	
};

class DefaultDecisionHandle : public DecisionHandle {	
	private :
      string     configuration; 
      TransList  translist;
      
	public:
		virtual string decide(Context & ctx);
		void configure(const string & configuration_);
};

class ErrorDecisionHandle : public DecisionHandle {	
	private :
      string     configuration; 
      TransList  translist;
      
	public:
		virtual string decide(Context & ctx);
		void configure(const string & configuration_);
};


class SubProcessInvocationHandle : public ProcessInvocationHandle {
   private :	
      string subprocessname;
      string requestparam;
      string responseparam; 
      bool ignoreerror;
      int undoother;
      string undootherserver;
   public :
      SubProcessInvocationHandle();    
      ~SubProcessInvocationHandle();
      void startSubProcess(Context& ctx);		
      void configure(const string & configuration);
};	

//对FMLContext操作
class FmlOperatorActionHandle : public ActionHandle {
private:
	  string  configuration; 	
      vector<OperItem> operlist;
public:
       FmlOperatorActionHandle();
       ~FmlOperatorActionHandle();    
       void execute(Context& ctx);
       void configure(const string & configuration);
};


/**
 * 句柄工厂
 */ 
class DefaultHandleFactory : public  HandleFactory{

private:
      DynamicLibraryManager manager;

public:
      DefaultHandleFactory(const string & workPath) {
         manager.setWorkPath(workPath);
      };
   	/**
   	 * 创建一个句柄, 生命周期有客户负责(客户程序负责delete该对象)
   	 * @param 句柄的类型名称
   	 * @return 句柄
   	 * @exception execution_error 执行期间出现异常
   	 */ 
   	PHandle create(const string& className);
};


#endif //EBPM4C_LCU_HOME_H

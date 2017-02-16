#ifndef FMLCONTEXT_H 
#define FMLCONTEXT_H
#include "lcu/lcu.h"
#include "lcu/plugins.h"
#include "mwci/CFmlBuf.h"
#include "log4cpp/Logger.h"
using namespace log4cpp;

namespace ebpm4c{
typedef counted_ptr < CFmlBuf >  PCFmlBuf;
typedef counted_ptr < Exception >  PException;
class FMLContext: public Context{
    
private:
		map<string, PCFmlBuf> buffers;
		string flowid;
		bool undoflag;	   
		int servidx;//服务进程序号 默认=0主进程
		int servcount;
		PException baseExcep;
		Logger* log;
		EndState* endnode;
public:
      FMLContext() : Context() {
         undoflag = false;
         flowid   = "";        
         servidx = 0; 
         servcount = 0;
         log = 0;   
         endnode = 0;   
      }     
      
      FMLContext(FMLContext& ctx) : Context(ctx) { 
         servidx = ctx.servidx;
         servcount = ctx.servcount;
         undoflag = false;
         flowid   = "";
         setLogger(&ctx.getLogger());
         endnode = 0;
      }
      
      ~FMLContext();
      
      void setLogger(Logger * logger){
         log = logger;
      }
      Logger& getLogger(){
      	ASSERT(log!=0);
      	return *log;
      }
      
      void setException(Exception& exp);
      
      Exception* getException();
      
      void setServIdx(const int& servindex){this->servidx = servindex;}
      int getServIdx(){return this->servidx;}  
      
      void setServCount(const int& count_){this->servcount = count_;}
      int getServCount(){return this->servcount;}       
                 
      //是否要为回退 
      void setUndoFlag(bool flag); 
      bool getUndoFlag();
      
      void setFlowId(const string& flowId);
      string getFlowId();
          
      void addCFmlBuf(const string& bufname,const PCFmlBuf& cfmlbuf);
      
      PCFmlBuf getCFmlBuf(const string& bufname);
      
      //得到Buff对象中字段的值    
      string getFMLFldValue(const string& varname);  
       //得到Buff对象中字段的值 needstr=true需要在string上加''
      string getFMLFldValue(const string& varname,bool needstr);
      
      //此函数暂时没有用到
      string getBufferedValue(const string& bufname,const string& fldname,int index=0);  
      
      string getVariant(const string& vname){
          return getFMLFldValue(vname);
      }	 
      void setEndState(EndState* end){
         endnode = end;	
      }
      EndState*  getEndState(){
         return endnode;	
      }
};
}
#endif // FMLCONTEXT_H

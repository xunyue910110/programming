#include "mwlcu/mwlcu.h"
#include "lcu/lcu.h"
#include "mwlcu/tradestateagent.h"
#include "base/StringUtil.h"
#include "dynlib/DynamicLibrary.h"
#include "sys/stat.h"

using namespace std;
using namespace ebpm4c;



class MWLCUImpl : public MWLCU  {
private :
      
   //static MWLCUImpl mwlcuInstance;   
   PDefaultHandleFactory factory;
   PUDBIDBDefinitionsLoader loader;
   Logger& lcutrace;
   int xaPID;
 
   //服务配置信息
   struct TPSVRINFO lcusvrconf;
   int trans_timeout;
    //请求Buffer名
   int tpTransTimeout(const string&  processname);
   int trans_manager;
   int tpTransManager(const string& processname);
   bool undoforward;
   bool isUndoForward(const string& processname);
     
   MWLCUImpl& operator = (const MWLCUImpl&){
      return *this;
   }   
public:   
   MWLCUImpl():lcutrace(Logger::getLogger(LCU_LOG_TRACE)){
   }
   MWLCUImpl(const MWLCUImpl&):lcutrace(Logger::getLogger(LCU_LOG_TRACE)){
   }
   
   ~MWLCUImpl(){      
   } 
   
   //static MWLCUImpl& getInstance(){     
   //   return MWLCUImpl::mwlcuInstance;
   //}
   //初始化引擎
   void initEngine(); 
   
   //服务进程启动代码，加载资源
   int tpsvrinit(int argc,char ** argv);
   
   PDefinitionLoaderDao getDefinitionsLoader(){
       return loader;
   }
   /**
   * 执行系统定义的流程
   * @param inBuf 输入PCFmlBuf参数作为请求Buffer
   * @return PCFmlBuf 回复Buffer
   */   
   PCFmlBuf execute(PCFmlBuf inBuf); 
   //0=不要事务,1=本地事务,2=XA事务
   int getTransLevel(const string& processname);
   //是否需要事务   
   bool isTransable(const string& processname);    
   //是否需要回退   
   bool isUndoable(const string&  processname);
   //是否需要监视
   bool isMonitor(const string&  processname);
   //请求Buffer名
   string getRequestName(const string&  processname);  
   //回复Buffer名 
   string getResponseName(const string&  processname);
   //得到流水号字段名
   string getFlowIDName(const string&  processname);  
   //执行子流程,子流程不需要开事务
   PCFmlBuf subProcessExecute(const string& processname,FMLContext& ctx,PCFmlBuf inBuf);
 
   //执行Undo  
   PCFmlBuf undoExecute(const string& processname,FMLContext& ctx);
   //执行Undo流程 processindex 为0是主流程
   void undoProcessExecute(const string& processname,int processindex,FMLContext& ctx);
   
public:
   
   //是否在事务中 
   bool isInTransXA(); 
   //是否在本地事务中
   bool isInTransLocal();   
   //已经在事务中，就不开事务	
   void transBegin(int transLevel);
   //iscommit = false 则 rallback
   void transCommit(bool iscommit,FMLContext& fmlctx);
private: 
   //写回退日志
   void writeUndoLog(PProcessDefinition pdef,FMLContext& ctx); 
   //执行Undo调用子流程节点
   void undoSubProcessNodeHandle(PNode pnode,int index,FMLContext& ctx);
   //执行Undo调用普通节点
   void undoNodeHandle(PNode pnode,FMLContext& ctx);
   
	time_t his_patchlevel_time;
	time_t his_lcu_patch_time;
	time_t lastpatchleveltime;
    string his_lcu_patch_info;
    string g_ProcessName;
    map<int,string> errcode_table; 
    FMLContext * rootctx;
public:	
		
	void init_patch_time();
	
	void check_update();
	
	bool check_patchlevel();	
	
	void fireUpdate_LCU_Patch();	
		
	void fireUpdate_SO_Patch();	
	
	void fireUpdate_DAO_Patch();
	
	void initCurrentPatchInfo();
		
	string getErrorCodeInfo(int errcode);
	//监视员工
	void monitorStaff(PCFmlBuf inBuf, const string&  processname = "" );
};

//初始化引擎
	
MWLCU& MWLCU::getInstance(){ 
	static MWLCUImpl mwlcuInstance;    
	return mwlcuInstance;
}

//服务进程启动代码，加载资源
int MWLCUImpl::tpsvrinit(int argc,char ** argv){
	vector<string> tpsvr;
	for(int i = 0; i < argc; i++){
	    tpsvr.push_back(string(argv[i]));
	}
	char key;	
	for ( int i = 0; i < tpsvr.size() -1; i += 2){
		key = tpsvr[i + 1].at(1);
		switch( key ){			
			case 'C':
				lcusvrconf.SRVNAME  = tpsvr[i];
				break;
			case 'g':
				lcusvrconf.DOMAINID = tpsvr[i];
				break;
			case 'i':
				lcusvrconf.GROUPID  = tpsvr[i];
				break;
			case 'u':
				lcusvrconf.SRVID    = tpsvr[i];
				break;
			case 'U':
				lcusvrconf.HOSTID   = tpsvr[i];
				break;				
			case 'm'://日志文件					
			default: //原样保留
				lcusvrconf.CONFINFO.push_back( tpsvr[ i ]  );
				lcusvrconf.CONFINFO.push_back( tpsvr[ i + 1 ] );				
		}		
	}
	lcusvrconf.SRVPID = getpid();
	TradeStateAgent::getInstance().initsvr(&lcusvrconf);
		
    return 0;
}


void MWLCUImpl::initEngine(){
   if(!loader){
      string workPath = getenv("BOSS_LIB");

      LOG_TRACE_P2(lcutrace,"[tmboot][%s][workpath:%s]",lcusvrconf.SRVNAME.c_str(),workPath.c_str());

      factory = PDefaultHandleFactory(new DefaultHandleFactory(workPath));
      loader  = PUDBIDBDefinitionsLoader(new UDBIDBDefinitionsLoader());
      loader -> setHandleFactory(factory);
      if(string("false") != getenv("BOSS_LCU_LOAD_ALL")){
      		loader -> refresh();     
      }   
      //当环境变量BOSS_LCU_LOAD_ALL=false时 启动服务不加载LCU流程
      loader -> loadErrorCodeTable(errcode_table);
      init_patch_time();
      LOG_TRACE(lcutrace,"[tmboot][success]");
      //ebpmEngine = ebpm4c::PEngine(new ebpm4c::Engine(factory, loader));
   }
}
int MWLCUImpl::tpTransTimeout(const string&  processname){
   initEngine();   
   string req = loader->getDefinitionByName(processname)->getExtendedAttribute("timeout");
   trans_timeout = (req.empty()) ? 100 : atoi(req.c_str()); 
   return trans_timeout;
}
bool MWLCUImpl::isMonitor(const string&  processname) {
   initEngine();
   string req = loader->getDefinitionByName(processname)->getExtendedAttribute("monitor");
   return ("true" == req);
}

bool MWLCUImpl::isUndoable(const string&  processname) {
   initEngine();
   string req = loader->getDefinitionByName(processname)->getExtendedAttribute("undoable");
   return ("false" != req && "" != req );
}
 bool MWLCUImpl::isUndoForward(const string& processname){
   initEngine();
   string req = loader->getDefinitionByName(processname)->getExtendedAttribute("undoable");
   return ("true_forward" == req);
 }

string MWLCUImpl::getRequestName(const string&  processname) {
   initEngine();
   string req = loader->getDefinitionByName(processname)->getExtendedAttribute("requestName");
   return ("" != req ) ? req : "InBuf";
}

string MWLCUImpl::getResponseName(const string&  processname) {
   initEngine();
   string res = loader->getDefinitionByName(processname)->getExtendedAttribute("responseName");
   return ("" != res ) ? res : "OutBuf";
}

string MWLCUImpl::getFlowIDName(const string&  processname) {
   initEngine();
   string res = loader->getDefinitionByName(processname)->getExtendedAttribute("flowidName");
   return ("" != res ) ? res : "TRADE_ID";//string(LCU_FML_DEF_FLOWID);
}

bool MWLCUImpl::isTransable(const string&  processname) {
   initEngine();
   string req = loader->getDefinitionByName(processname)->getExtendedAttribute("transaction");
   return ("notrans" != req );  //不为false就是需要
}

int MWLCUImpl::tpTransManager(const string& processname){
	trans_manager = getTransLevel(processname);
	return trans_manager ;
}
	
//0=不需要事务 1=本地事务 2=没有了 3=XA事务 4=多路本地事务  -1= 外部管理事务 
int MWLCUImpl::getTransLevel(const string& process){
   initEngine();
   string processname = process;
   if( processname == ""){
		processname = this->g_ProcessName;
   }
   string req = loader->getDefinitionByName(processname)->getExtendedAttribute("transaction");
   int v = 0;
   if (req == "notrans")        { v = 0;}
   else if (req == "false")     { v = 0;}
   else if (req == "true")      { v = 1;}
   else if (req == "translocal"){ v = 1;}   
   else if (req == "transXA")   { v = 3;}  
   else if (req == "transMuti") { v = 4;}   
   else if (req == "outermanager")   { v = -1;}    
   return v;
}

bool MWLCUImpl::isInTransXA(){
    int v = tpgetlev();
    switch(v){
      case 0: return false;
      case 1: return true;
      default: return false;
    }
}

bool MWLCUImpl::isInTransLocal(){
     JConnect* lcon = jdb::getConnect();
     return lcon->isTrans();
}

//根据需要开事务
void MWLCUImpl::transBegin(int transLevel){
   if(trans_manager < 0){
		LOG_TRACE(lcutrace,"[LCU]由LCU的外部管理事务，所以不管事务启动"); 
		return;
   }
  
   Logger& tradelog = Logger::getLogger(LCU_LOG_TRADE + g_ProcessName);
   switch(transLevel){
      case -1:
           break;     
      case 0:
           break;
      case 1: {//1=本地事务    
      	    JConnect* lcon = jdb::getConnect();       
            if(lcon->isTrans()){
               LOG_TRACE(tradelog,"[LCU]已经在本地事务中不需要再开事务");
            } else if(isInTransXA()){              
               LOG_TRACE(tradelog,"[LCU]已经开了主流程XA事务,本地事务不启动");
            } else  {
               ASSERT(rootctx);
               if(rootctx->getServIdx() != 0 && rootctx->getProcessId() != 0){
               	   THROW_C_P1(LCUException,-1,"开本地事务异常:%s","被tuxedo调用的服务进程里不允许再调用子流程启动本地事务,只有跨库独立事务时允许被调主流程中开本地事务");
               }               
               //不能把隐藏事务清掉，否则会掩盖程序逻辑错误
               lcon->TransRollback();//把隐藏事务清掉
               lcon->TransBegin();
               LOG_TRACE(tradelog,"[LCU]开主流程本地事务");
            }
            break;
         } 
      case 2:  // 没有2
           break;    
      case 3: {// 3=XA事务
      	
           if(isInTransLocal()){
               THROW_C(LCUException,-1,"[LCU]已经在本地事务中不允许再开XA事务");
           } else  if(isInTransXA()){
               LOG_TRACE(tradelog,"[LCU]已经在XA事务中不需要再开事务");
           } else {
               tpabort(0);
               LOG_TRACE(tradelog,"[LCU]开主流程XA事务");
               if( tpbegin(trans_timeout ,0) == -1){               	    
                    THROW_C_P1(LCUException,tperrno,"tpbegin()异常:%s",tpstrerror(tperrno));
               }else{
                    xaPID = lcusvrconf.SRVPID;
               }
           }
           break;
         }
      case 4:
           break;
      default:
		break;
   }
}
//iscommit=true提交事务，= false回滚事务
void MWLCUImpl::transCommit(bool iscommit,FMLContext& fmlctx){
   LOG_TRACE_P3(lcutrace,"[LCU]事务处理，iscommit:%d 进程idx:%d 子流程idx:%d",iscommit,fmlctx.getServIdx(),fmlctx.getProcessId()); 
   if(trans_manager < 0){
		LOG_TRACE(lcutrace,"[LCU]由LCU的外部管理事务，所以不管事务提交"); 
		return;
   }
   Logger& lcutrace = fmlctx.getLogger();//用函数内部的log
   JConnect* lcon = jdb::getConnect();
   if(lcon->isTrans()){
      if(iscommit){
         if(fmlctx.getProcessId() == 0){
            lcon->TransCommit();
            LOG_TRACE(lcutrace,"[LCU]提交主流程本地事务");
         } else {
            //id不等于0的子流程不能提交本地事务
            LOG_TRACE(lcutrace,"[LCU]是子流程，暂不提交本地事务");
         }
      } else {
         lcon->TransRollback();
         LOG_TRACE(lcutrace,"[LCU]主流程回滚本地事务");
      }
   } else if(isInTransXA()){
      if( iscommit ){         
         if(fmlctx.getServIdx() == 0 && fmlctx.getProcessId() == 0){
            //只有主进程才能提交事务
            LOG_TRACE(lcutrace,"[LCU]提交主流程XA事务");
            if(tpcommit(0) == -1){
               Logger& tradelog = fmlctx.getLogger();
               tradelog.alert(Grade::MAJOR,tperrno,
               	  	 (string("运行在") + lcusvrconf.SRVNAME + "中,发生tpcommit()异常" + tpstrerror(tperrno)).c_str()) ;
               THROW_C_P1(LCUException,tperrno,"tpcommit()异常:%s",tpstrerror(tperrno));
            }
         } else {
            LOG_TRACE(lcutrace,"[LCU]不是主进程的主流程，暂不提交XA事务");
         }
      } else {
         if(lcusvrconf.SRVPID == xaPID){
         	 LOG_TRACE(lcutrace,"[LCU]流程回滚XA事务");
	         if(tpabort(0)==-1) {
				Logger& tradelog = fmlctx.getLogger();
				tradelog.alert(Grade::MAJOR,tperrno,
	              	(string("运行在") + lcusvrconf.SRVNAME + "中,发生tpabort()异常" + tpstrerror(tperrno)).c_str()) ;
				LOG_TRACE_P1(lcutrace,"[LCU]主流程tpabort()异常:%s",tpstrerror(tperrno));               
	            //THROW_C_P1(LCUException,tperrno,"tpabort()异常:%s",tpstrerror(tperrno));
	         }
         } else {
            LOG_TRACE(lcutrace,"[LCU]不是启动XA进程，暂不回滚XA事务");
         }
      }
   } else {
   		 lcon->TransRollback();
   		 LOG_TRACE(lcutrace,"[LCU]不在事务中,Rollback 清除隐含的事务");
   }    
}

//执行流程
PCFmlBuf MWLCUImpl::execute(PCFmlBuf inBuf){
          
      //cout << "[LCU][tpcall begin]--["<< lcusvrconf.SRVNAME << "]--" << endl;
      initEngine();
      TradeStateAgent& tsAgent = TradeStateAgent::getInstance();
      TradeStateItem tsItem = tsAgent.beginTradeState(); //得到代理项
      
      FMLContext ctx;
      string processname;
      string errinfo;
      int errcode = 0;      
      bool tranflag = false;
      bool logflag = false;
      PCFmlBuf outBuf;      
      xaPID = 0;  
      try {
      	 check_update();       
         if(inBuf->IsFldExist(LCU_FML_PROCESS_NAME)){
				processname = inBuf->GetString(LCU_FML_PROCESS_NAME);
         } else {
				THROW(LCUExecutionException,"[LCU]X_TRANS_CODE交易编码字段不存在");
         }        
        
		Logger& tradelog = Logger::getLogger(LCU_LOG_TRADE + processname);
		ctx.setLogger(&tradelog);
		
		this->g_ProcessName = processname;
		
		LOG_TRACE_P2(lcutrace,"主流程执行开始:%s,%s",lcusvrconf.SRVNAME.c_str(),processname.c_str());
		LOG_TRACE_P2(tradelog,"主流程执行开始:%s,%s",lcusvrconf.SRVNAME.c_str(),processname.c_str());
		
		this->monitorStaff(inBuf,processname);//此时事务还没有被启动
				
		if(tradelog.isTraceEnabled()){
			const string& logtrace = inBuf->ToString();
			LOG_TRACE_P1(tradelog,"主流程InBuf:\n%s",logtrace.c_str());
		}
         
		if(inBuf->IsFldExist(LCU_TXDCALL_COUNT)){//进程深度
			int idx = inBuf->GetInt(LCU_TXDCALL_COUNT);
			ctx.setServIdx(idx + 1);
			ctx.setServCount(idx + 1);
			LOG_TRACE_P1(tradelog,"[LCU]当前进程深度为:%d",ctx.getServIdx());
		} else {
			ctx.setServIdx(0);//当前进程时主叫进程
			ctx.setServCount(0);
		}
        rootctx = &ctx;//设置进程根ctx
		//是否执行Undo
		if(inBuf->IsFldExist(LCU_FML_UNDOFLAG)){            
            string inflag = inBuf->GetString(LCU_FML_UNDOFLAG);
            if(strcmp(LCU_FML_UNDOFLAG_VALUE, inflag.c_str()) == 0){
               LOG_TRACE(lcutrace,"[LCU]流程执行回退开始");
               ctx.addCFmlBuf(LCU_FML_UNDOBUFFER_NAME, inBuf);
               return undoExecute(processname,ctx);
            }
         }
         string flowidname;
         if(isUndoable(processname)){
            flowidname = getFlowIDName(processname);
            if(inBuf->IsFldExist(flowidname.c_str())){
               ctx.setFlowId(inBuf->GetString(flowidname.c_str()));
               logflag = true;
               //cout << "[LCU]流程将记录回退日志,因为需要回退："<< flowidname << endl;
            } else {                      
               tradelog.alert(Grade::WARNING, 0, string("[LCU]流程将无法回退,因为流水号没有值:"+flowidname).c_str());
            }
         } 

         ctx.addCFmlBuf(getRequestName(processname), inBuf);
                    
         
         int v = tpTransManager(processname);
         if( v == 3) tpTransTimeout(processname);//需要事务才设置超时间
         this->transBegin(v);

         PProcessDefinition pdef = loader->getDefinitionByName(processname);
         //执行主流程
         pdef->execute(ctx);

         //只有需要回退且 流程需要事务或已经在事务中
         if(logflag && ( v > 0 || this->isInTransLocal() || this->isInTransXA())){
            writeUndoLog(pdef,ctx);
         } 
                 
         EndState* endnode = ctx.getEndState();
         if(endnode == 0) {            	
         	LOG_TRACE_P1(lcutrace,"[LCU]流程%s上下文中没有结束节点:",processname.c_str());
         	this->transCommit(false,ctx);
         } else { 
			this->transCommit(endnode->isCommit(),ctx);
			int grade = endnode->getAlertCode();
			if( grade > -1){
				string info(processname);
				info += "流程告警:" ;
				info += endnode->getName();
				ctx.getLogger().alert((Grade::Type)grade, -1, info.c_str());
			}            
		}
		//LOG_TRACE_P1(lcutrace, "主流程执行结束:%s",processname.c_str());
        LOG_TRACE_P1(tradelog,"主流程执行结束:%s",processname.c_str());     
	}catch(LCUException & e) {
         ctx.setException(e);
         errcode  =  e.getCode();
         errinfo  =  e.getMessage();
         transCommit(false, ctx); 
	}catch(Exception & e) {
         ctx.setException(e);
         errcode = e.getCode();
         errinfo = e.toString();        
         transCommit(false, ctx); 
	}catch(const exception& ex) {             
         errcode = -1;
         errinfo = ex.what();
         Exception e(errcode,errinfo);  
         ctx.setException(e);
         transCommit(false, ctx);
	}catch(...) {
         errcode = -1;
         errinfo = "未知异常";
         Exception e(errcode,errinfo);
         ctx.setException(e);
         transCommit(false, ctx);
	}
	
	Logger& tradelog = Logger::getLogger(LCU_LOG_TRADE + processname);
	if(errcode != 0){
          LOG_TRACE(lcutrace, errinfo.c_str());
          if(ctx.getException()){
          	if(! dynamic_cast< BusinessException* >( ctx.getException() ) 
          	   && ! dynamic_cast< LCUWrapperException* >( ctx.getException() ) )
                tradelog.alert(*ctx.getException());
            //else if(ctx.getException()->getGrade()!= Grade::INDETERMINATE)
            // 	tradelog.alert(*ctx.getException());
          } //else {
             	//tradelog.alert(Grade::MINOR,ctx.getErrorCode(), ctx.getErrorInfo().c_str());
          //}
	}
      
	string respons;
	try {
		respons = getResponseName(processname);
		if(ctx.getUndoFlag()){
			respons = string(LCU_FML_UNDOBUFFER_NAME);
		}
	}catch(...){
		LOG_TRACE(lcutrace, "异常退出，流程定义不存在");
	}
	outBuf = ctx.getCFmlBuf(respons);
	string configerrinfo;
	try {
         //进程call计数器
         outBuf->SetInt(LCU_TXDCALL_COUNT,ctx.getServCount());//把调过的子服务数一起输出去
         if (errcode != 0){           
               outBuf->SetInt(LCU_FML_RESULTCODE,errcode);
               configerrinfo = getErrorCodeInfo(errcode); 
               if(configerrinfo.empty())          
               		outBuf->SetString(LCU_FML_RESULTERRINFO, processname + "执行异常:\n"+errinfo);
               else {               	  
                    outBuf->SetString(LCU_FML_RESULTERRINFO, configerrinfo);
                    LOG_TRACE_P1(tradelog,"主流程输出原始错误信息:%s", errinfo.c_str());
               }
               ctx.setErrorCode(errcode);
               ctx.setErrorInfo(errinfo);
         } else {
	         outBuf->SetInt(LCU_FML_RESULTCODE, ctx.getErrorCode());	       
	         if(ctx.getErrorCode()!=0 ){
	         	 configerrinfo = getErrorCodeInfo(ctx.getErrorCode()); 
	         	 if(configerrinfo.empty())       
	             	outBuf->SetString(LCU_FML_RESULTERRINFO, ctx.getErrorInfo());
	             else {
	                outBuf->SetString(LCU_FML_RESULTERRINFO, configerrinfo);
	                LOG_TRACE_P1(tradelog,"主流程输出原始错误信息:%s", ctx.getErrorInfo().c_str());
	             }
	         } else {	             
	             if(!outBuf->IsFldExist(LCU_FML_RESULTERRINFO)){
	             	  outBuf->SetString(LCU_FML_RESULTERRINFO, "OK!");
	             }
	         }
         }
	}catch(...){        
         LOG_TRACE(lcutrace,"MWCI异常,返回空集");
         outBuf = ctx.getCFmlBuf("ERROR");
	}

	if(tradelog.isTraceEnabled()){
		const string& logtrace = outBuf->ToString();
		LOG_TRACE_P1(tradelog,"主流程OutBuf:\n%s",logtrace.c_str());
	}
	
	tsItem.trade = processname;
	tsItem.resultcode = ctx.getErrorCode();
	tsAgent.submitTradeState(tsItem);
	//cout << "[LCU][tpcall end]----------------" << endl;
	return outBuf;
}

//执行子流程,子流程不需要开事务
PCFmlBuf MWLCUImpl::subProcessExecute(const string& processname,FMLContext& ctx,PCFmlBuf inBuf){
   int errcode = 0;
  
   bool logflag = false;
   try {
		
		ctx.addCFmlBuf(getRequestName(processname), inBuf);
		//子流程借用父流程的logger来输出日志
		LOG_TRACE_P2(ctx.getLogger(),"[LCU]子流程执行开始:%s  Index:%d" ,processname.c_str(),ctx.getProcessId());
		
		int v = getTransLevel(processname); //只取level不管manager
		if( v == 3) tpTransTimeout(processname);//需要事务才设置超时间 
		this->transBegin(v);

		if(isUndoable(processname)){
			string flowidname = getFlowIDName(processname);
			if(inBuf->IsFldExist(flowidname.c_str())){
				ctx.setFlowId(inBuf->GetString(flowidname.c_str()));
				logflag = true;
				//如果主流程不需要，所以也不要记录
				if(isUndoable(g_ProcessName)==false){
					logflag = false;
				}
				//cout << "[LCU]流程将需要回退,因为流水号没有值:" << flowidname << endl;
			} else {				
				LOG_TRACE_P1(ctx.getLogger(),"[LCU][worning]流程将无法回退,因为流水号没有值:%s" ,flowidname.c_str());
			}
		}
	
		PProcessDefinition pdef = loader->getDefinitionByName(processname);
		pdef->execute(ctx);
		
		//只有需要回退且 流程需要事务或已经在事务中
		if(logflag && ( v > 0 || this->isInTransLocal() || this->isInTransXA())){
			writeUndoLog(pdef,ctx);
		}
	
		//this->transCommit(true,ctx);
		EndState* endnode = ctx.getEndState();
		if( endnode == 0) {  
			//cout << "[LCU]上下文中没有结束节点" << endl; 
			LOG_TRACE_P1(lcutrace,"[LCU]子流程%s上下文中没有结束节点:",processname.c_str());    
			this->transCommit(false,ctx);
		} else { 
			this->transCommit(endnode->isCommit() , ctx);
			int grade = endnode->getAlertCode();
			if( grade > -1){
				string info(processname);
				info += "流程告警:" ;
				info += endnode->getName();
				ctx.getLogger().alert((Grade::Type)grade, -1, info.c_str());
			}            
		}
   } catch (LCUException & ex){
      transCommit(false, ctx);
      throw;
   }catch(BusinessException & e) {
      transCommit(false, ctx);
      //cout << "subprocess:BusinessException:"<< e << endl;
      throw;
   }catch(exception & e) {
      transCommit(false, ctx);
      //cout << "subprocess:exception:"<< e << endl;
      throw;
   }catch(...) {
      transCommit(false, ctx);
      //cout << "subprocess:exception" << endl;
      throw;
   }
   PCFmlBuf outBuf = ctx.getCFmlBuf(getResponseName(processname));
   //设置结束信息
    outBuf->SetInt(LCU_FML_RESULTCODE, ctx.getErrorCode());
   
    if(ctx.getErrorCode()!=0 ){
		outBuf->SetString(LCU_FML_RESULTERRINFO, ctx.getErrorInfo());
    } else {
		if(!outBuf->IsFldExist(LCU_FML_RESULTERRINFO)){
		  	outBuf->SetString(LCU_FML_RESULTERRINFO, "OK!");
		}
	}      
  
   LOG_TRACE_P1(ctx.getLogger(),"[LCU]子流程执行结束:%s" ,processname.c_str());
   return outBuf;
}

void MWLCUImpl::writeUndoLog(PProcessDefinition pdef,FMLContext& ctx){
   int64_t definitionid  = pdef->getId();
   string definitionname = pdef->getName();
   //cout << "写回退日志开始:" << definitionname << endl;
   UDBIUndoLogDao logdao;
   logdao.writelog( definitionid, definitionname,ctx);  
   LOG_TRACE_P1(ctx.getLogger(),"[LCU]写回退日志结束:%s" ,definitionname.c_str());
}


PCFmlBuf MWLCUImpl::undoExecute(const string& processname,FMLContext& ctx){
   //开始UNDO
   UDBIUndoLogDao logdao;
   ctx.setUndoFlag(true);
   this->undoforward = isUndoForward(processname);
   PCFmlBuf inBuf = ctx.getCFmlBuf(LCU_FML_UNDOBUFFER_NAME);
   if(isUndoable(processname)){
      string flowidname = getFlowIDName(processname);
      if(inBuf->IsFldExist(flowidname.c_str())){
         ctx.setFlowId(inBuf->GetString(flowidname.c_str()));
      } else {        
         LOG_TRACE_P1(ctx.getLogger(),"[LCU][ERROR]流程无法回退,因为流水号没有值:%s" ,flowidname.c_str());
      }
   }
   string flowid = ctx.getFlowId();
   if(flowid.empty()){     
      THROW(LCUExecutionException,"回退流程失败:没有输入流水号");
   }

   if(inBuf->IsFldExist(LCU_TXDCALL_COUNT)){//进程深度
      int idx = inBuf->GetInt(LCU_TXDCALL_COUNT);
      ctx.setServIdx(idx - 1);
      ctx.setServCount(idx - 1);    
      LOG_TRACE_P1(ctx.getLogger(),"[LCU]当前进程回退深度为:%d",ctx.getServIdx());
   } else {
      ctx.setServIdx(0);//当前进程时主叫进程
      int prec = logdao.getLogServCount(flowid);
      ctx.setServCount(prec);
   }

   inBuf->SetInt(LCU_TXDCALL_COUNT,ctx.getServCount());//把调过的子服务数一起输出去

   int servidx = ctx.getServIdx();

   try {
      //cout << "[LCU]开始运行回退,Undo:"<< processname << endl;
      LOG_TRACE_P1(lcutrace,"[LCU]开始运行回退,Undo:",processname.c_str());    
      //cout << "[LCU]回退开主流程事务" <<endl;
      int v = tpTransManager(processname);
      if( v == 3) tpTransTimeout(processname);//需要事务才设置超时间 
      this->transBegin(v);

      int count = logdao.getLogSteps(flowid,servidx);
      ctx.setLogStep(count);
      if(count <= 0 ){        
         THROW(LCUExecutionException,"回退日志中记录回退步骤为0,不需要回退 流水号为:" +flowid);
      }

      this->undoProcessExecute(processname,0,ctx);
      //cout << "[LCU]提交主回退流程事务"<<endl;
      ctx.setProcessId(0);
      this->transCommit(true,ctx);

   } catch(exception& e) {
      //cout << "[LCU]回退发生异常" << endl;
      LOG_TRACE_P1(lcutrace,"[LCU]回退发生异常,e:%s",e.what());  
      this->transCommit(false,ctx);
      throw;
   }

   PCFmlBuf outBuf = ctx.getCFmlBuf(LCU_FML_UNDOBUFFER_NAME);
   //cout << "[LCU]主回退流程执行结束." << endl;
   LOG_TRACE_P1(lcutrace,"[LCU]主回退流程执行结束%s","");  
   //cout << "[LCU][end]--------------------------------" << endl;
   return outBuf;
}

void MWLCUImpl::undoProcessExecute(const string & processname,int index,FMLContext& ctx){
      //开始UNDO
      //cout << "[LCU]运行流程回退UNDO:"<< processname <<""<<index << endl;
      LOG_TRACE_P2(lcutrace,"[LCU]运行流程回退UNDO:%s,IDX:%d",processname.c_str(),index); 
      if(isUndoable(processname) == false){
           LOG_TRACE_P1(lcutrace,"[LCU]被调用UNDO子流程:%s,没有设置成需要回退,所以log中不会有轨迹.请在父流程图中把调用此子流程的节点的属性:选择Undo方式改成:不执行Undo.",processname.c_str()); 
           return ;
      }     
       
      string flowid = ctx.getFlowId();
      int servidx = ctx.getServIdx();
      this->undoforward = isUndoForward(processname);
      UDBIUndoLogDao logdao;
      if(logdao.isUndoFinished(flowid,servidx,index,processname)){        
         THROW(LCUExecutionException,"不允许重复执行回退");
      } else {
         //cout << "[LCU]以前没有运行过流程回退："<< processname <<" TRADE_ID="<<flowid << endl;         
      }
      vector<string>  strlist;
      int64_t log_def_id = logdao.readlog(strlist,flowid,servidx,index,processname);
      PProcessDefinition pdef = loader->getDefinitionByName(processname);
      int64_t def_id = pdef->getId();
      if(def_id != log_def_id ){
         //cout << "[LCU]获得历史版本的流程定义."<< endl;
         LOG_TRACE(lcutrace,"[LCU]获得历史版本的流程定义."); 
         pdef = loader->getDefinitionById(log_def_id);
         //throw execution_error("[LCU]回退流程版本与当前流程版本不一致.");
      }
      string nodeid;
      string nodetmp;
      PNode pnode;
      int pos;
      if(this->undoforward == false){
	      vector<string>::reverse_iterator it = strlist.rbegin();
	      for(; it != strlist.rend(); it++){
	         nodeid = (*it);
	         //cout << "[LCU]回退 Node:"<< nodeid <<endl;
	         LOG_TRACE_P1(lcutrace,"[LCU]回退 Node:%s",nodeid.c_str());  
	         pos = nodeid.find("sub(");
	         if(pos>=0){
	            pos     = nodeid.find(",");
	            nodetmp = nodeid.substr(4,pos-4);
	            pos     = atoi(nodeid.substr(pos+1 , nodeid.size()-pos-2).c_str());
	            pnode   = pdef->getNodeByID(logdao.str2int64(nodetmp));
	            undoSubProcessNodeHandle(pnode,pos,ctx);
	         }else {
	            pnode = pdef->getNodeByID(logdao.str2int64(nodeid));
	            undoNodeHandle(pnode,ctx);
	         }
	      }
	   } else {//上下代码是拷贝的
		      vector<string>::iterator it = strlist.begin();
		      for(; it != strlist.end(); it++){
		         nodeid = (*it);
		         //cout << "[LCU]回退 Node:"<< nodeid <<endl;
		         LOG_TRACE_P1(lcutrace,"[LCU]forward回退 Node:%s",nodeid.c_str());  
		         pos = nodeid.find("sub(");
		         if(pos>=0){
		            pos     = nodeid.find(",");
		            nodetmp = nodeid.substr(4,pos-4);
		            pos     = atoi(nodeid.substr(pos+1 , nodeid.size()-pos-2).c_str());
		            pnode   = pdef->getNodeByID(logdao.str2int64(nodetmp));
		            undoSubProcessNodeHandle(pnode,pos,ctx);
		         }else {
		            pnode = pdef->getNodeByID(logdao.str2int64(nodeid));
		            undoNodeHandle(pnode,ctx);
		         }
		      }	   
	   }
	      
      logdao.setUndoSucess(flowid,servidx,index,pdef->getId());      
      LOG_TRACE(lcutrace,"[LCU]流程回退执行结束.");  
}

void MWLCUImpl::undoSubProcessNodeHandle(PNode pnode,int index,FMLContext& ctx){
      PProcessState pstate = dynamic_pointer_cast< ProcessState > (pnode);
      ctx.setProcessId(index);
      PDelegation delegation = pstate->getDelegation();
      PHandle h = delegation->getHandle();
      PProcessInvocationHandle handle = dynamic_pointer_cast < ProcessInvocationHandle > (h);
      handle->startSubProcess(ctx);
}

void MWLCUImpl::undoNodeHandle(PNode pnode,FMLContext& ctx){
      PState pstate =  dynamic_pointer_cast < State >  (pnode);
      const vector< PAction >& actions = pstate->getActions();
      vector < PAction >::const_iterator it = actions.begin();
      for ( ; it != actions.end(); it++) {
        (* it)->execute(ctx);
      }
      ctx.setLogStep(ctx.getLogStep()-1);//做完后再减
}



//---------与同步更新有关系--------------------------------------------------

void MWLCUImpl::init_patch_time(){
	char* pf = getenv("MW_PATCHLEVEL");   
    if(pf) {    	
		struct stat patchstat;		
		if(stat(pf, &patchstat) == 0) {				
				his_patchlevel_time = patchstat.st_mtime;				
		} else {
		        his_patchlevel_time = time(0);
                lcutrace.alert("检查patchlevel文件失败:%s", strerror(errno));		
		}
	} else {
	    his_patchlevel_time = time(0);
		cout << "[LCU]NOT getenv MW_PATCHLEVEL." <<endl;
	}	
	char* patchfile = getenv("LCU_PATCHINFO");;
	if(patchfile) {    	
		struct stat patchstat;		
		if(stat(patchfile, &patchstat) == 0) {				
			his_lcu_patch_time = patchstat.st_mtime;
			initCurrentPatchInfo();				
		} else {	
		    his_lcu_patch_time = time(0);
			lcutrace.alert("[LCU]检查patchlevel文件失败:%s", strerror(errno));
		}
	} else {
	    his_lcu_patch_time = time(0);
		cout << "[LCU]NOT getenv LCU_PATCHINFO." <<endl;
	}	
}


void MWLCUImpl::initCurrentPatchInfo(){	
	char* patchfile = getenv("LCU_PATCHINFO");;
	if(patchfile) {  
		ifstream fin(patchfile,ios_base::in);
		if(!fin.is_open()) {
	    	cout << "LCU_PATCHINFO 文件打不开." <<endl;
	    	return;
	    }
		string linedata;
		while(!fin.eof() && !fin.fail())
		{       
			getline(fin, linedata, '\n');
			StringUtil::trim(linedata);
			if(linedata.size() > 0 && linedata.at(0)=='!'){
					his_lcu_patch_info = linedata;		
					break;			  
			}		    		
		} 		
		fin.close();
	}
}


bool MWLCUImpl::check_patchlevel(){	
    char* pf = getenv("MW_PATCHLEVEL");   
    time_t lastpatchtime = time(0);
    if(pf) {    	
		struct stat patchstat;		
		if(stat(pf, &patchstat) == 0) {				
			lastpatchtime = patchstat.st_mtime;			
		} else {		      
			lcutrace.alert("[LCU]检查MW_PATCHLEVEL文件失败:%s", strerror(errno));
		}
	}
	if(lastpatchtime != his_patchlevel_time){
		lastpatchleveltime = lastpatchtime;
		return true;
	} 
	return false;		
}

void MW patchstat;		
		if(stat(patchfile, &patchstat) == 0) {				
			his_lcu_patch_time = patchstat.st_mtime;
			initCurrentPatchInfo();				
		} else {	
		    his_lcu_patch_time = time(0);
			lcutrace.alert("[LCU]妫
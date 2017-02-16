#include "mwlcu/lcuplugins.h"

#include "mwlcu/mwlcu.h"
#include "base/Exception.h"
#include <iostream>
#include "log4cpp/Logger.h"


using namespace log4cpp;
/**
#if defined(HPUX_OS_CONFIG)
#define MWLCU_HPUX
#include <unistd.h>
#include <sys/param.h>
#include <sys/pstat.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

	void mem_rpt_trace(Logger& tradelog)
	{
		if ( "1" != getenv("MWLCU_MEM_TRACE")) return;
		pid_t pid = getpid();
		struct pst_status pst;		
		memset((void*)&pst,0,sizeof(struct pst_status));		
		
		//获取进程信息
		pstat_getproc(&pst,sizeof(pst),0,pid);
		//printf("rtext:%ld,rdata:%ld,rstack:%ld,vtext:%ld,vdata:%ld,vstack:%ld\n",
		//			 pst.getrtext(),pst.getrdata(),pst.getrstack(),v.getvtext(),pst.getvdata(),pst.getvstack());
		
        long pagesize = getpagesize()/1024;   
		char info[200];
		sprintf(info,"rtext:%ld,rdata:%ld,rstack:%ld,vtext:%ld,vdata:%ld,vstack:%ld\n",
					 pst.pst_tsize*pagesize,pst.pst_dsize*pagesize,pst.pst_ssize*pagesize,pst.pst_vtsize*pagesize,pst.pst_vdsize*pagesize,pst.pst_vssize*pagesize);
	    LOG_TRACE_P2(tradelog,"HPUX pid:%d %s\n",pid,info);
	};

#elif defined(IBMAIX_OS_CONFIG)
#define MWLCU_IBMAIX
#include <stdlib.h>
#include <stdio.h>
#include <sys/procfs.h>
	void mem_rpt_trace(Logger& tradelog)
	{ 
		if ( "1" != getenv("MWLCU_MEM_TRACE")) return;
		pid_t pid = getpid();
		FILE* fd = 0;
		char filename[50];	
		struct psinfo sinfo;
		
		//读取的文件名
		sprintf(filename,"/proc/%ld/psinfo",pid);
		fd = fopen(filename,"r");
		if (fd)
		{
			fread((char*)&sinfo,sizeof(struct psinfo),1,fd);		
			//printf("The size : %ld , resident size : %ld\n",sinfo.pr_size,sinfo.pr_rssize);
		    char info[100];
		    sprintf(info,"The size : %ld , resident size : %ld\n",sinfo.pr_size,sinfo.pr_rssize);
	       
	        LOG_TRACE_P2(tradelog ,"IBMAIX pid:%d %s\n",pid,info);
			
			fclose(fd);
		}else{
		     LOG_TRACE_P2(tradelog ,"IBMAIX pid:%d  cant open file:%s\n",pid,filename);
		}
	};
#else
#define MWLCU_OTHER_OS
#include <stdlib.h>
#include <stdio.h>
#include <sys/procfs.h>
	void mem_rpt_trace(Logger& tradelog)
	{   
		if ( "1" != getenv("MWLCU_MEM_TRACE")) return;
		pid_t pid = getpid();
		FILE* fd = 0;
		char filename[50];	
		struct pstatus status;
		
		//读取的文件名
		sprintf(filename,"/proc/%ld/status",pid);
		fd = fopen(filename,"r");
		if (fd)
		{
			fread((char*)&status,sizeof(struct pstatus),1,fd);		
			//printf("The heapsize : %ld , stack size : %ld\n",status.pr_brksize,status.pr_stksize);
			char info[100];
		    sprintf(info,"The heapsize : %ld , stack size : %ld\n",status.pr_brksize,status.pr_stksize);
	        LOG_TRACE_P2(tradelog,"IBMAIX pid:%d %s\n",pid,info);
			fclose(fd);
		}else{
		     LOG_TRACE_P2(tradelog ,"IBMAIX pid:%d  cant open file:%s\n",pid,filename);
		}
	};
#endif

*/




DLActionHandle::DLActionHandle(DynamicLibraryManager * _manager) {
   this->manager = _manager;
   ignoreerror = false ;
}
DLActionHandle::~DLActionHandle(){
}

/**
* 现在执行动态库就比较复杂了
* 一般运行：invoker(inbuf,outbuf)
* undo  0:  Undoinvoker(undobuf,undobuf)
*       1:  undooterserver(undobuf,undobuf);
	    2:  不执行;
*/
void DLActionHandle::execute(Context& ctx) {
  
      //构建新的buffer，还是直接传递
	  FMLContext& fmlctx =(FMLContext&) ctx;
	  //cout << "[LCU]动态库运行开始"<< endl;
	  //cout << "[LCU]libname:"<<libname << endl;
	  //cout << "[LCU]symbolname:"<<symbolname << endl;
	  //cout << "[LCU]requestparam:"<<requestparam << endl;
	  //cout << "[LCU]responseparam:"<<responseparam << endl;
	  //cout << "[LCU]undoother:"<<undoother << endl;
	  //cout << "[LCU]undootherserver:"<<undootherserver << endl;
      string method = symbolname;
      bool exeUndo = false;
      Logger& tradelog = fmlctx.getLogger();
      
      LOG_TRACE_P1(tradelog,"[LCU]此节点配置，MEM调试环境变量%s",getenv("MWLCU_MEM_TRACE"));
      //mem_rpt_trace(tradelog);
      
      PCFmlBuf outbuf;
      try {
         if(fmlctx.getUndoFlag()){
				exeUndo = true;
				ostringstream oout;
				oout << "[LCU]Undo动态库调用开始:"<<libname <<"."<<LCU_METHOD_UNDO_PREF << method <<"("<<LCU_FML_UNDOBUFFER_NAME<<","<<LCU_FML_UNDOBUFFER_NAME<<")"<< endl;
				LOG_TRACE_P1(fmlctx.getLogger(),"%s",oout.str().c_str());
				if(undoother == 1){
					method = undootherserver;
					ostringstream oout;
					oout << "[LCU]Undo动态库调用指向另一个函数:"<<libname <<"." << method <<"("<<LCU_FML_UNDOBUFFER_NAME<<","<<LCU_FML_UNDOBUFFER_NAME<<")"<< endl;
					LOG_TRACE_P1(fmlctx.getLogger(),"%s",oout.str().c_str());
					exeUndo = false;
				} else if(undoother == 2){
					LOG_TRACE_P1(fmlctx.getLogger(),"[LCU]此节点配置，不需要UNDO%s","");
					exeUndo = true;
				}
				// ==0 	exeUndo = true;
         }

         if(exeUndo == true){
         	  if(undoother == 0){
	            try {
	            	//默认undo方式，在原来函数前加Undo
					outbuf = fmlctx.getCFmlBuf(LCU_FML_UNDOBUFFER_NAME);

					outbuf->SetInt(LCU_FML_LOGSTEP_NAME,ctx.getLogStep());
					if(tradelog.isTraceEnabled() ){
						const string& logtrace = outbuf->ToString();
						LOG_TRACE_P2(tradelog,"Undo函数inbuf变量:%s\n%s",LCU_FML_UNDOBUFFER_NAME,logtrace.c_str());

					}
					//UndoInvoker undoinvoker = (UndoInvoker)manager->getSymbol(libname, LCU_METHOD_UNDO_PREF + symbolname);
					method = LCU_METHOD_UNDO_PREF;
					method += symbolname;
					Invoker undoinvoker = (Invoker)manager->getSymbol(libname, method);
					//调一下undo函数
					undoinvoker(*outbuf,*outbuf);

					int errcode = 0;
					if(outbuf->IsFldExist(LCU_FML_RESULTCODE)){
						errcode = outbuf->GetInt(LCU_FML_RESULTCODE);
            		}
                    if(errcode != 0){
			            if(outbuf->IsFldExist(LCU_FML_RESULTERRINFO)){
			                THROW_C(LCUExecutionException,errcode,outbuf->GetString(LCU_FML_RESULTERRINFO));
			            }else {
			                THROW_C(LCUExecutionException,errcode, "Undo异常");
			            }
					}
					if(tradelog.isTraceEnabled() ){
						const string& logtrace = outbuf->ToString();
						LOG_TRACE_P2(tradelog,"Undo函数outBuff变量:%s\n%s",LCU_FML_UNDOBUFFER_NAME,logtrace.c_str());

					}
	            } catch (LibraryException& le ){
	               //cout << "UNDO DID NOT EXECUTE:" << le << endl;
	               throw;
	            }
	          }//等于2就什么都没有执行
         } else {
            //执行正向和undo=1时的正向函数
            PCFmlBuf inbuf = fmlctx.getCFmlBuf(requestparam);
            if(fmlctx.getUndoFlag()) {
            	inbuf = fmlctx.getCFmlBuf(LCU_FML_UNDOBUFFER_NAME);
            }
            inbuf->SetInt(LCU_FML_LOGSTEP_NAME,ctx.getLogStep());
            inbuf->SetInt(LCU_FML_LAST_RESULTCODE,ctx.getErrorCode());
            inbuf->SetString(LCU_FML_LAST_RESULTERRINFO,ctx.getErrorInfo());

            //调用前先清空错误码
            inbuf->SetInt(LCU_FML_RESULTCODE , 0);
            inbuf->SetString(LCU_FML_RESULTERRINFO , "");

            outbuf = fmlctx.getCFmlBuf(responseparam);
            if(fmlctx.getUndoFlag()) {
            	outbuf = fmlctx.getCFmlBuf(LCU_FML_UNDOBUFFER_NAME);
            }

            if(tradelog.isTraceEnabled()){
            	ostringstream oout;
                oout << "[LCU]动态库运行开始:"<<libname <<"."<< method <<"(" <<requestparam << "," << responseparam <<")" <<endl;
            	const string& logtrace = inbuf->ToString();
            	LOG_TRACE_P2(tradelog,"输入函数InBuf变量:%s\n%s",requestparam.c_str(),logtrace.c_str());
            	LOG_TRACE_P1(tradelog,"%s",oout.str().c_str());
            }

            Invoker invoker = (Invoker)manager->getSymbol(libname, method);
           
            //调用完后取错误码
            invoker(*inbuf, *outbuf);


            LOG_TRACE_P1(tradelog,"%s","[LCU]动态库运行结束");


            if(outbuf->IsFldExist(LCU_FML_RESULTCODE)){
				fmlctx.setErrorCode(outbuf->GetInt(LCU_FML_RESULTCODE));
            }else{
                fmlctx.setErrorCode(0);
            }
            outbuf->SetInt(LCU_FML_LAST_RESULTCODE,fmlctx.getErrorCode());

            if(outbuf->IsFldExist(LCU_FML_RESULTERRINFO)){
                fmlctx.setErrorInfo(outbuf->GetString(LCU_FML_RESULTERRINFO));
            }else{
                fmlctx.setErrorInfo("");
            }
            outbuf->SetString(LCU_FML_LAST_RESULTERRINFO,fmlctx.getErrorInfo());

            if(tradelog.isTraceEnabled() ){
            	const string& logtrace = outbuf->ToString();
            	LOG_TRACE_P2(tradelog,"输出函数OutBuf变量:%s\n%s",responseparam.c_str(),logtrace.c_str());
            }
         }
    } catch (BusinessException & ex){
    	 //cout << "[LCU]动态库业务异常:"<<ex<< endl;
    	 ostringstream oout;
         oout << libname <<"."<< method <<"(" <<requestparam << "," << responseparam <<")" << endl;
    	 LOG_TRACE_P2(tradelog,"[LCU]动态库业务异常.%s%s",oout.str().c_str(),ex.toString().c_str());
         fmlctx.setException(ex);
         fmlctx.setErrorCode(ex.getCode());
         fmlctx.setErrorInfo(ex.toString());
         if(ex.getGrade()!= Grade::INDETERMINATE)
            fmlctx.getLogger().alert(ex);
         if(fmlctx.getUndoFlag()){
    	   	throw;
    	 }
    } catch (std::exception & e){    	 	
    	 	LOG_TRACE_P1(tradelog,"[LCU]动态库运行抛出标准异常.%s",e.what());
    	 	throw;
    } catch (...){           
            LOG_TRACE_P2(tradelog,"[LCU]动态库发生未知异常.%s%s","","");
            throw;
    }
    LOG_TRACE(tradelog,"[LCU]动态库运行结束.");   
    //如果错误编码在系统错误范围内, 则throw 执行异常, 放弃所有操作
    //mem_rpt_trace(tradelog);
      
}

void DLActionHandle::configure(const string & configuration) {
	  //cout << "[LCU]定义需要调用动态库:" << configuration << endl;
 	  //CTRade.InsertLog(buffer,buffer) ignoreerror;
 	  int pos = configuration.find(".");
      if (pos == string::npos) THROW(LCUDefinitionException,configuration + ", 协议错误");
      this->libname = configuration.substr(0, pos);
      int psc = configuration.find("(");
      if (psc == string::npos) THROW(LCUDefinitionException,configuration + ", 无法解析出入口函数名称");
      this->symbolname = configuration.substr(pos + 1, psc-pos-1);
      int pend = configuration.find(")");
      string params = configuration.substr(psc + 1, pend-psc-1);
      //cout << "params:" <<params <<endl;
      int pps = params.find(",");
      if (pps == string::npos) THROW(LCUDefinitionException,configuration + ", 无法解析出输入输出对象名称");
      this->requestparam = params.substr(0, pps);
      this->responseparam = params.substr(pps + 1,params.size()-pps-1);

		undoother = 0;
		string noukey = "NOUNDO=";
		string ukey = "UNDO=";
		int pp = configuration.find(noukey);
		if(pp != string::npos){
			undoother = 2;
			return ;
		}
		pp = configuration.find(ukey);
		if(pp != string::npos){
			undoother = 1;
		} else{
			undoother = 0;
		}
		if(undoother==1){
			undootherserver = configuration.substr(pp+ukey.size());
		}
}



SubProcessInvocationHandle::SubProcessInvocationHandle(){
    ignoreerror = false;
}

SubProcessInvocationHandle::~SubProcessInvocationHandle(){
}

void SubProcessInvocationHandle::startSubProcess(Context& ctx){

	//cout << "[LCU]subprocessname:"<<subprocessname << endl;
	//cout << "[LCU]requestparam:"<<requestparam << endl;
	//cout << "[LCU]responseparam:"<<responseparam << endl;
	//cout << "[LCU]undoother:"<<undoother << endl;
	//cout << "[LCU]undootherserver:"<<undootherserver << endl;

	FMLContext& fmlctx = (FMLContext&) ctx;
	Logger& tradelog = fmlctx.getLogger();
	bool undochange = false;
	if(fmlctx.getUndoFlag()){
		MWLCU& mwlcu = MWLCU::getInstance();
		if(undoother == 0){
			mwlcu.undoProcessExecute(this->subprocessname,fmlctx.getProcessId(),fmlctx);
			return ;
		} else if(undoother == 1){
			//cout <<"[LCU]执行子流程服务调用UNDO执行另一个服务："<<undootherserver << endl;
			LOG_TRACE_P1(tradelog,"[LCU]执行子流程服务调用UNDO执行另一个服务:%s",undootherserver.c_str());
			fmlctx.setUndoFlag(false);
			undochange = true;
		} else if(undoother == 2){
			//cout <<"[LCU]执行跨服务调用UNDO不需要执行" << endl;
			LOG_TRACE_P1(tradelog,"[LCU]执行跨服务调用UNDO不需要执行.%s","");
			return ;
		}
	}

	PCFmlBuf InBuf = fmlctx.getCFmlBuf(this->requestparam);
	PCFmlBuf OutBuf = fmlctx.getCFmlBuf(this->responseparam);

    if(undochange == true){
		InBuf = fmlctx.getCFmlBuf(LCU_FML_UNDOBUFFER_NAME);
		OutBuf = fmlctx.getCFmlBuf(LCU_FML_UNDOBUFFER_NAME);
		LOG_TRACE_P2(tradelog,"执行Undo正向子流程inbuf/outbuf变量,都是:%s\n%s",LCU_FML_UNDOBUFFER_NAME,LCU_FML_UNDOBUFFER_NAME);
   	}
   	if(tradelog.isTraceEnabled() ){
		LOG_TRACE_P1(tradelog,"执行子流程%s",(subprocessname+"("+requestparam + "," + responseparam +")") .c_str());
		const string& logtrace = InBuf->ToString();
		LOG_TRACE_P2(tradelog,"子流程InBuf变量:%s\n%s",requestparam.c_str(),logtrace.c_str());
	}

   PCFmlBuf subInBuf;
   if(this->requestparam == this->responseparam){
      subInBuf  = InBuf;
   } else {
      subInBuf  = PCFmlBuf(new CFmlBuf());
      subInBuf->Append(*InBuf.get());
   }

	subInBuf->SetInt(LCU_FML_LAST_RESULTCODE,fmlctx.getErrorCode());
	subInBuf->SetString(LCU_FML_LAST_RESULTERRINFO,ctx.getErrorInfo());
	//调用前先清空错误码
	subInBuf->SetInt(LCU_FML_RESULTCODE,0);
	subInBuf->SetString(LCU_FML_RESULTERRINFO,"");

	try {

		FMLContext  subfmlctx(fmlctx);

		MWLCU& mwlcu = MWLCU::getInstance();
		string subtrade = this->subprocessname;
		if(undochange == true){
			subtrade = this->undootherserver;
		}

		PCFmlBuf subOutBuf = mwlcu.subProcessExecute(subtrade,subfmlctx,subInBuf);
		if(undochange == true){
			fmlctx.setUndoFlag(true);
		}
		string subreq = mwlcu.getRequestName(this->subprocessname);
		string subres = mwlcu.getResponseName(this->subprocessname);

		if(!(this->requestparam == this->responseparam && subreq == subres)){
			OutBuf->Cover(*subOutBuf.get());   //replace
		}
       
		if(OutBuf->IsFldExist(LCU_FML_RESULTCODE)){
			fmlctx.setErrorCode(OutBuf->GetInt(LCU_FML_RESULTCODE));
		}
		if(OutBuf->IsFldExist(LCU_FML_RESULTERRINFO)){
		    fmlctx.setErrorInfo(OutBuf->GetString(LCU_FML_RESULTERRINFO));
		}
		if(tradelog.isTraceEnabled() ){
		     const string& logtrace = OutBuf->ToString();
		     LOG_TRACE_P2(tradelog,"子流程OutBuf变量:%s\n%s",responseparam.c_str(),logtrace.c_str());
		}
		
        if(undochange == true && fmlctx.getErrorCode()!=0){	    
        	fmlctx.getLogger().alert(Grade::WARNING, fmlctx.getErrorCode(), string("Undo执行正向子流程异常:" + fmlctx.getErrorInfo()).c_str());
			THROW_C(LCUExecutionException,fmlctx.getErrorCode(), string("Undo执行正向子流程异常:" + fmlctx.getErrorInfo()).c_str());    
	    }
        
  } catch (LCUException & ex){
    	 //cout << "[LCU]子流程内部异常:"<<ex<< endl;
    	 ostringstream oout;
         oout << subprocessname<<"(" <<requestparam << "," << responseparam <<")" << endl;
    	 LOG_TRACE_P2(tradelog,"[LCU]子流程内部异常:%s%s",oout.str().c_str(),ex.toString().c_str());
         fmlctx.setException(ex);
         fmlctx.setErrorCode(ex.getCode());
         fmlctx.setErrorInfo(ex.getMessage());
		if(undochange == true){
			fmlctx.setUndoFlag(true);
		}
		if(fmlctx.getUndoFlag()){
			throw;
		}
   } catch (BusinessException & ex){
    	 //cout << "[LCU]子流程业务异常:"<<ex<< endl;
    	 ostringstream oout;
         oout << subprocessname<<"(" <<requestparam << "," << responseparam <<")" << endl;
    	 LOG_TRACE_P2(tradelog,"[LCU]子流程业务异常:%s%s",oout.str().c_str(),ex.toString().c_str());
         fmlctx.setException(ex);
         fmlctx.setErrorCode(ex.getCode());
         fmlctx.setErrorInfo(ex.toString());
         if(undochange == true){
			fmlctx.setUndoFlag(true);
		 }
    	 if(fmlctx.getUndoFlag()){
		  	throw;
		 }
    } catch (std::exception & e){
    	 	//cout << "[LCU]子流程运行发生异常:"<< e.what()<< endl;
    	 	LOG_TRACE_P1(tradelog,"[LCU]子流程运行发生异常:%s",e.what());
    	 	if(undochange == true){
				fmlctx.setUndoFlag(true);
		    }
    	 	throw;
    } catch (...){
            //cout << "[LCU]子流程发生未知异常."<< endl;
            LOG_TRACE_P1(tradelog,"[LCU]子流程发生未知异常.%s","");
            throw;
    }

    
	
   //OutBuf->Print();
   //cout << "OutBuf->Print();" <<endl;
}

void SubProcessInvocationHandle::configure(const string & configuration){
      //cout << "[LCU]定义需要调用子流程:" << configuration << endl;
 	  //InsertLog(buffer,buffer) ignoreerror;
      int psc = configuration.find("(");
      if (psc == string::npos) THROW(LCUDefinitionException,configuration + ", 无法解析出入口函数名称");
      this->subprocessname = configuration.substr(0, psc);
      int pend = configuration.find(")");
      string params = configuration.substr(psc + 1, pend-psc-1);
      //cout <<" subprocessname:" << subprocessname <<" params:" <<params <<endl;
      int pps = params.find(",");
      if (pps == string::npos) THROW(LCUDefinitionException,configuration + ", 无法解析出输入输出对象名称");
      this->requestparam = params.substr(0, pps);
      this->responseparam = params.substr(pps + 1,params.size()-pps-1);

		undoother = 0;
		string noukey = "NOUNDO=";
		string ukey = "UNDO=";
		int pp = configuration.find(noukey);
		if(pp != string::npos){
			undoother = 2;
			return ;
		}
		pp = configuration.find(ukey);
		if(pp != string::npos){
			undoother = 1;
		} else{
			undoother = 0;
		}
		if(undoother==1){
			undootherserver = configuration.substr(pp+ukey.size());
		}


}


TuxedoCallerActionHandle::TuxedoCallerActionHandle(){
	  ignoreerror = false ;
}

TuxedoCallerActionHandle::~TuxedoCallerActionHandle(){
}

void TuxedoCallerActionHandle::execute(Context& ctx){
	MWLCU& mwlcu = MWLCU::getInstance();

	//cout << "[LCU]执行远程Tuxedo调用:"<<symbolname << endl;
    //cout << "[LCU]requestparam:"<<requestparam << endl;
    //cout << "[LCU]responseparam:"<<responseparam << endl;
    //cout << "[LCU]undoother:"<<undoother << endl;
    //cout << "[LCU]undootherserver:"<<undootherserver << endl;

    FMLContext& fmlctx =(FMLContext&) ctx;
    Logger& tradelog = fmlctx.getLogger();
	LOG_TRACE_P1(tradelog,"[LCU]TuxedoCaller运行开始.%s","");
	JConnect* lcon = jdb::getConnect();
	if(lcon->isTrans()) {
		int v = mwlcu.getTransLevel(this->symbolname);
		if(fmlctx.getUndoFlag() && undoother == 1){
			v = mwlcu.getTransLevel(this->undootherserver);
		}
		if( v == 1){	//1,2 子服务本地事务
		    v = mwlcu.getTransLevel("");//查主流程是什么事务
			if( v != 4)  THROW(LCUExecutionException,"[LCU]执行远程Tuxedo调用,已经在本地事务中,但被调远程流程为本地事务,定义错误,主叫流程需要定义为XA事务,或跨库独立事务,才能运行");
		} else if(v == 3) //子服务是XA事务 
			THROW(LCUExecutionException,"[LCU]执行远程Tuxedo调用,被调远程流程需要XA事务,且本流程在本地事务中,需要定义为XA事务,才能运行");
	}
	
	//当前不在事务中
	if(mwlcu.isInTransXA() == false && lcon->isTrans() == false){
		int v = mwlcu.getTransLevel(this->symbolname);
		if(fmlctx.getUndoFlag() && undoother == 1){
			v = mwlcu.getTransLevel(this->undootherserver);
		}
		if( v == 1 ){	//1,2 子服务本地事务
			v = mwlcu.getTransLevel("");//查主流程是什么事务
			if(v != 4) THROW(LCUExecutionException,"[LCU]执行远程Tuxedo调用,被调远程流程为本地事务,定义错误,主叫流程需要定义为跨库独立事务,才能运行");
	    }
	}
	
	
	int preidx = fmlctx.getServCount();

	//cout << "[LCU][TuxedoCaller]-----------------------------[ServIdx:" << preidx <<"]"<< endl;
	LOG_TRACE_P1(tradelog,"[LCU][TuxedoCaller]----------------[ServIdx:%d]",preidx);
	PCFmlBuf InBuf = fmlctx.getCFmlBuf(this->requestparam);

	PCFmlBuf OutBuf ;
    IMWCaller *theCaller;
	try {
		if(this->requestparam == this->responseparam){
			OutBuf = InBuf;
		} else {
			OutBuf = fmlctx.getCFmlBuf(this->responseparam);
		}
        if(fmlctx.getUndoFlag()){
        	string undobuf = LCU_FML_UNDOBUFFER_NAME;
        	
        	LOG_TRACE_P1(tradelog,"[LCU-Undo]执行Undo Tuxedo Caller , inbuf = outbuf = %s",LCU_FML_UNDOBUFFER_NAME);
        	InBuf = fmlctx.getCFmlBuf(undobuf);
        	OutBuf = InBuf;
        }

		//是否执行Undo 如果当前是执行的子服务也是
		if(fmlctx.getUndoFlag()){
			InBuf->SetString(LCU_FML_UNDOFLAG,LCU_FML_UNDOFLAG_VALUE);
			if(undoother == 1 ){
			     InBuf->SetString(LCU_FML_UNDOFLAG,"DOOTHER");
		    }
		    if(undoother == 2 ){
			     InBuf->SetString(LCU_FML_UNDOFLAG,"NOTDO");
		    }
		}
				
		InBuf->SetInt(LCU_TXDCALL_COUNT, preidx);
				
		//设置交易编码
		InBuf->SetString(LCU_FML_PROCESS_NAME,this->symbolname);

		InBuf->SetInt(LCU_FML_LAST_RESULTCODE,ctx.getErrorCode());
		InBuf->SetString(LCU_FML_LAST_RESULTERRINFO,ctx.getErrorInfo());
		//调用前先清空错误码
		InBuf->SetInt(LCU_FML_RESULTCODE,0);
		InBuf->SetString(LCU_FML_RESULTERRINFO,"");

		
		theCaller = CCallerFactory::CreateCaller(0);
		
		LOG_TRACE_P1(tradelog,"[LCU]执行跨服务调用Tuxedo SynCaller:%s",(symbolname + "("+requestparam + "," + responseparam +")") .c_str());
		if(tradelog.isTraceEnabled() ){
				const string& logtrace = InBuf->ToString();
				LOG_TRACE_P2(tradelog,"执行跨服务InBuf变量:%s\n%s",requestparam.c_str(),logtrace.c_str());
        }
        
		ASSERT(theCaller!=0);
		
		//false 取物理服务名,true取逻辑名需要命名服务
		if(fmlctx.getUndoFlag()){
			if(undoother == 0){
				theCaller->InterSynCall(this->symbolname, *InBuf, *OutBuf);
			} else if(undoother == 1){
				
				LOG_TRACE_P1(tradelog,"[LCU]执行跨服务调用UNDO执行另一个服务 %s",undootherserver.c_str());

				//设置交易编码
		        InBuf->SetString(LCU_FML_PROCESS_NAME,this->undootherserver);
				theCaller->InterSynCall(this->undootherserver, *InBuf, *OutBuf);
			} else if(undoother == 2){				
				LOG_TRACE_P1(tradelog,"[LCU]执行跨服务调用UNDO不需要执行.%s","");
			}
		}else {
		    theCaller->InterSynCall(this->symbolname, *InBuf, *OutBuf);
		}
		
		//cout << "[LCU]Tuxedo SynCall GetReply." << endl;
		//OutBuf->Print();

		if(tradelog.isTraceEnabled() ){
			LOG_TRACE_P1(tradelog,"[LCU]执行跨服务调用Tuxedo SynCall GetReply.%s","完毕");
         	const string& logtrace = OutBuf->ToString();
         	LOG_TRACE_P2(tradelog,"[LCU]执行跨服务调用返回OutBuf变量:%s\n%s",responseparam.c_str(),logtrace.c_str());
        }
        delete theCaller;
       
		if (OutBuf->IsFldExist(LCU_TXDCALL_COUNT)){//进程深度
			preidx = OutBuf->GetInt(LCU_TXDCALL_COUNT);
		} else {
			if(fmlctx.getUndoFlag()){
				OutBuf->SetInt(LCU_TXDCALL_COUNT, preidx - 1);
			} else {
				OutBuf->SetInt(LCU_TXDCALL_COUNT, preidx + 1);  //如果没有则用当前的count加1
			}
		}
		//inBuf->SetInt(LCU_TXDCALL_COUNT,ctx.getServCount());//把调过的子服务数一起输出去
		fmlctx.setServCount(OutBuf->GetInt(LCU_TXDCALL_COUNT)); //当前的count也要加1,
		if(OutBuf->IsFldExist(LCU_FML_RESULTCODE)){
			fmlctx.setErrorCode(OutBuf->GetInt(LCU_FML_RESULTCODE));
		}
		if(OutBuf->IsFldExist(LCU_FML_RESULTERRINFO)){
		    fmlctx.setErrorInfo(OutBuf->GetString(LCU_FML_RESULTERRINFO));
		}
		if(fmlctx.getUndoFlag()){
			OutBuf->SetString(LCU_FML_UNDOFLAG,LCU_FML_UNDOFLAG_VALUE);
		}
	} catch (Exception & ex){
    	
    	 LOG_TRACE_P1(tradelog,"[LCU]执行跨服务调用Tuxedo异常.%s",ex.toString().c_str());
         fmlctx.setException(ex);
         fmlctx.setErrorCode(ex.getCode());
         fmlctx.setErrorInfo(ex.toString());
         if(theCaller) delete theCaller;
    	 if(fmlctx.getUndoFlag()){
		  	throw;
		 }
	} catch (std::exception &e){
		
		LOG_TRACE_P1(tradelog,"[LCU]TuxedoCaller标准异常:.%s",e.what());
		if(theCaller) delete theCaller;
		throw;
	} catch (...){
		
		LOG_TRACE_P1(tradelog,"[LCU]TuxedoCaller非标准异常.%s","");
		if(theCaller) delete theCaller;
		throw;
	}
	
	LOG_TRACE_P1(tradelog,"[LCU]TuxedoCaller运行结束.%s","");
	//cout << "[LCU][TuxedoCaller]-----------------------------------------" << endl;

}

void TuxedoCallerActionHandle::configure(const string & configuration) {
	//cout << "[LCU]定义需要调用Tuxedo服务:" << configuration << endl;
	//InsertLog(buffer,buffer)

	int psc = configuration.find("(");
	if (psc == string::npos) THROW(LCUDefinitionException,configuration + ", 无法解析出入口函数名称");
	this->symbolname = configuration.substr(0, psc);
	int pend = configuration.find(")");
	string params = configuration.substr(psc + 1, pend-psc-1);
	//cout <<"service:" << symbolname <<" params:" <<params <<endl;
	int pps = params.find(",");
	if (pps == string::npos) THROW(LCUDefinitionException,configuration + ", 无法解析出输入输出对象名称");
	this->requestparam = params.substr(0, pps);
	this->responseparam = params.substr(pps + 1,params.size()-pps-1);

    undoother = 0;
	string noukey = "NOUNDO=";
	string ukey = "UNDO=";
	int pp = configuration.find(noukey);
	if(pp != string::npos){
		undoother = 2;
		return ;
	}
	pp = configuration.find(ukey);
	if(pp != string::npos){
		undoother = 1;
	} else{
		undoother = 0;
	}
	if(undoother==1){
		undootherserver = configuration.substr(pp+ukey.size());
	}

}

string DefaultDecisionHandle::decide(Context & ctx) {
	string trans;
	
	if( "" != configuration && !this->translist.empty() ){
	   DecideToolkit dtk;  //dtk.printtranslist(translist);
	   trans = dtk.getDecideResult(&ctx,this->translist);
	}
	//cout << "[LCU]configuration:" << configuration << "\r\n执行转移:"<< trans <<"/"<< endl;
	return trans;
}

void DefaultDecisionHandle::configure(const string &configuration_) {
	this->configuration = configuration_;
	if(this->translist.empty()){
      DecideToolkit dtk;
      dtk.parseScript(this->configuration,this->translist);
	}
}

string ErrorDecisionHandle::decide(Context & ctx) {
	string trans = "";
	if( "" != configuration && !this->translist.empty() ){
	   DecideToolkit dtk;
	   dtk.setErrorDecision(true);
	   //dtk.printtranslist(translist);
	   trans = dtk.getDecideResult((FMLContext*)&ctx,this->translist);
	}
	//cout << "[LCU]ErrorDecisionHandle:" << configuration << "\r\n执行转移:"<< trans <<"/"<< endl;
	return trans;
}

void ErrorDecisionHandle::configure(const string &configuration_) {
	this->configuration = configuration_;
	if(this->translist.empty()){
      DecideToolkit dtk;
      dtk.parseScript(this->configuration,this->translist);
	}
}


FmlOperatorActionHandle::FmlOperatorActionHandle()
{
}
FmlOperatorActionHandle::~FmlOperatorActionHandle(){
}

void FmlOperatorActionHandle::configure(const string & configuration_){
     this->configuration = configuration_;
	 if(this->operlist.empty()){
      	FmlOperator dtk;
      	dtk.parser(this->configuration,this->operlist);
	}
}


void FmlOperatorActionHandle::execute(Context& ctx){
	FMLContext& fmlctx=(FMLContext&)ctx;
	fmlctx.setErrorCode(0);
	fmlctx.setErrorInfo("");
	Logger& tradelog = fmlctx.getLogger();
	
    LOG_TRACE(tradelog,"[LCU]FmlOperatorActionHandle::execute()");
	if(fmlctx.getUndoFlag()) {
	 	
	 	LOG_TRACE(tradelog,"[LCU]在UNDO中，不需要执行FmlOperatorActionHandle");
	    return ;
	}
	try{
		if( "" != configuration && !this->operlist.empty() ){
		   FmlOperator dtk;
		   dtk.execute(fmlctx,this->operlist);
		}
	}catch(exception& e){

	    LOG_TRACE_P1(tradelog,"[LCU]FmlOperator.exception", e.what());
	    LOG_TRACE_P1(tradelog,"[LCU]FmlOperator.configuration:", configuration.c_str());
		
		throw;
    }
}

LCUAdminActionHandle::LCUAdminActionHandle(){
       this->cmd = "";
       this->paramtype = "all";
       this->param = "";
}

LCUAdminActionHandle::~LCUAdminActionHandle(){
}

void LCUAdminActionHandle::execute(Context& ctx){
   FMLContext& fmlctx =(FMLContext&) ctx;
   cout << "[LCU]LCU命令开始执行:" << this->cmd <<" -" << this->paramtype << this->param << endl;
   PCFmlBuf p = fmlctx.getCFmlBuf("request");
   string transcode = p->GetString(LCU_FML_PROCESS_NAME);
   if(transcode!= "lcuadmin"){
       THROW(LCUDefinitionException,"您的权限不足,LCU命令未执行");
   }
    //改成取字段值，其它不改
   if(paramtype != "all"){

   	   if(p->IsFldExist("X_SUBTRANS_CODE")){
   	    	this->param = p->GetString("X_SUBTRANS_CODE");
   	    	this->paramtype = "D";
   	   }
   	   if(paramtype == "N") {
         	THROW(LCUDefinitionException,"系统暂不支持此类命令参数");
   	   }
  }
   MWLCU& mwlcu = MWLCU::getInstance();
   PDefinitionLoaderDao rloader = mwlcu.getDefinitionsLoader();
   PUDBIDBDefinitionsLoader  loader = dynamic_pointer_cast< UDBIDBDefinitionsLoader> (rloader);
   if(cmd == "refresh" ){
       if( paramtype == "all" ){
            loader->refresh();
            cout << "[LCU]LCU命令执行完毕:refresh -all" << endl;
            return ;
       } else if(paramtype=="D" && this->param != ""){
            loader->refreshDefinitionByName(this->param);
            cout << "[LCU]LCU命令执行完毕:refresh -D" << this->param << endl;
            return ;
       }
   }else {
       THROW(LCUDefinitionException,"系统暂不支持此类命令:"+cmd);
   }
   THROW(LCUDefinitionException,"命令或参数无效");
}

void LCUAdminActionHandle::configure(const string & configuration){

      int psc = configuration.find("=");
      if (psc == string::npos) THROW(LCUDefinitionException,configuration + ", 无法解析命令");
      int pcmd = configuration.find(" " , psc+1);
      if (pcmd < 0) {
          this->cmd=configuration.substr(psc+1);
          return;
      }
      this->cmd = configuration.substr(psc+1, pcmd-psc-1);
     
      pcmd = configuration.find("-" , pcmd);
      if (pcmd < 0)return;
      paramtype = configuration.substr(pcmd+1,1);
    
      if(paramtype == "a") {
         this->paramtype = "all";
         return ;//暂不支持
      }
      else if (configuration.substr(pcmd+1,1)=="D") this->paramtype = "D";
      else {
            this->paramtype = "N";
            return ;//暂不支持
      }
      this->param = configuration.substr(pcmd+2);

}


/**
 * 创建一个句柄, 生命周期有客户负责(客户程序负责delete该对象)
 * @param 句柄的类型名称
 * @return 句柄
 * @exception execution_error 执行期间出现异常
 */
PHandle DefaultHandleFactory::create(const string& className) {
	//cout << "Create Handle:"<< className <<endl;
	if (className == "shareobject") {
		return PHandle(new DLActionHandle(& manager));
   }else if (className == "bufferoperator") {
		return PHandle(new FmlOperatorActionHandle());
   }else if (className == "decision") {
		return PHandle(new DefaultDecisionHandle());
   }else if (className == "errordecision") {
		return PHandle(new ErrorDecisionHandle());
   }else if (className == "subprocess") {
		return PHandle(new SubProcessInvocationHandle());
   }else if(className  == "tuxedocaller"){
		return PHandle(new TuxedoCallerActionHandle());
	}else if(className  == "lcuadmin"){
		return PHandle(new LCUAdminActionHandle());
	}else		
		THROW(LCUDefinitionException,"系统无法定位代理类:"+className);
}

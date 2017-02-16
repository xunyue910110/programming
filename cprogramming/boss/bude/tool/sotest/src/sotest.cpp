//---------------------------------------
//Title  : sotest for MWLCU 
//Author : juyf@lianchuang.com  
//Date   : 2005/01/25
//---------------------------------------
#include "mwci.h"
#include "udbi.h"
#include "sotest.h"
#include "Exception.h"

using namespace std; 
using namespace UDBI;

MWSOTEST MWSOTEST::MWSOTESTInstance;

//0=不需要事务 1=本地事务 2=XA事务 
int MWSOTEST::getTransLevel(const string& processname){ 
	string req = processname;
	int v = 0;
	if (req == "notrans")        { v = 0;}
	else if (req == "false")     { v = 0;}
	else if (req == "true")      { v = 1;}
	else if (req == "translocal"){ v = 1;}	
	else if (req == "translocal-rollback") { v = 2;}
	else if (req == "transXA")   { v = 3;}
	return v;  
} 

bool MWSOTEST::isInTransXA(){
    int v = tpgetlev();
    switch(v){
      case 0: return false;
      case 1: return true;
      default: return true;
    }   
} 

bool MWSOTEST::isInTransLocal(){
     JConnect* lcon = jdb::getConnect();
     return lcon->isTrans();
} 

//根据需要开事务
void MWSOTEST::transBegin(int transLevel){  
   switch(transLevel){
      case 0:
           break;
      case 1: {
            JConnect* lcon = jdb::getConnect();
            if(lcon->isTrans()){
               cout << "[sotest]已经在本地事务中不需要再开事务" <<endl;
            } else {              
               //lcon->TransRollback();//不能把隐藏事务清掉，否则会掩盖程序逻辑错误
               lcon->TransBegin();
               cout << "[sotest]开测试流程本地事务" <<endl;
            }
            break;
         }
      case 2: {
            JConnect* lcon = jdb::getConnect();
            if(lcon->isTrans()){
               cout << "[sotest]已经在本地事务中不需要再开事务" <<endl;
            } else {             
               lcon->TransRollback();//把隐藏事务清掉
               lcon->TransBegin();
               cout << "[sotest]开测试流程本地事务" <<endl;
            }
            break;         
         }                 
      case 3: {
           if(isInTransXA()){
               cout << "[sotest]已经在XA事务中不需要再开事务" <<endl;
   	     } else {
   	         //tpabort(0);
   	         tpbegin(100,0);	    
               cout << "[sotest]开测试流程XA事务" <<endl;
   	     } 
   	     break;
   	   }
      default:
        break;
   }
}
//roll=true提交事务，= false回滚事务
void MWSOTEST::transCommit(bool iscommit,int ctx){ 
   JConnect* lcon = jdb::getConnect();  
   if(lcon->isTrans()){
      if(iscommit){
         //id不等于0的子流程不能提交本地事务         
         if(ctx == 0){
            lcon->TransCommit();
            cout << "[sotest]提交测试流程本地事务"<<endl; 
         } 
         //else  cout << "[sotest]不提交测试流程本地事"<<endl;
      } else {
         lcon->TransRollback();
         cout << "[sotest]测试流程回滚本地事务"<<endl;  
      }
   }
   //else cout << "[sotest]不在本地事务中:"<< (string)(iscommit?"true":"false") <<endl;  
   if(isInTransXA()){
      if(iscommit){
         //只有主进程才能提交事务
         if(ctx == 0){
            tpcommit(0);          
            cout << "[sotest]提交测试流程XA事务"<<endl; 
         }
         else  cout << "[sotest]不提交XA事务"<<endl;     
      } else {
         tpabort(0);
         cout << "[sotest]测试流程回滚XA事务"<<endl;  
      }   
   } 
   //else cout << "[sotest]不在XA事务中,iscommit="<< (string)(iscommit?"true":"false") <<endl;  
}

//执行流程
PCFmlBuf MWSOTEST::testExecute(PCFmlBuf inBuf){   
   
      PCFmlBuf outBuf;
   	string processname;	 
   	string errinfo;
   	int errcode = 0; int ctx = 0; 
   	bool tranflag = false;
   	bool logflag  = false;
   	bool undoflag = false;
   	
   	try { 
   	  	string workPath = getenv("BOSS_LIB");
         manager.setWorkPath(workPath);      
   	   cout <<"[sotest]so动态库文件路径BOSS_LIB:"<<workPath <<endl;
   	   if(inBuf->IsFldExist(LCU_FML_PROCESS_NAME)){
   			processname = inBuf->GetString(LCU_FML_PROCESS_NAME); 
   		} else { 	    
   			errinfo = "[sotest]shareobject configure 不存在"; 			
   			throw execution_error(errinfo);
   		}
   	   
   	   cout << "[sotest]测试流程执行开始:" << processname <<  endl; 	   
   		//是否执行Undo
   		if(inBuf->IsFldExist(LCU_FML_UNDOFLAG)){
    		   string inflag = inBuf->GetString(LCU_FML_UNDOFLAG);			
   		   if(strcmp(LCU_FML_UNDOFLAG_VALUE, inflag.c_str()) == 0){
               cout << "[sotest]流程执行回退开始" << endl; 
               undoflag = true; 
   		   }	   
   		}
   	   
   		string transAttribute = "false";
   	   if(inBuf->IsFldExist(LCU_TRANSABLE_NAME)){
    		   transAttribute = inBuf->GetString(LCU_TRANSABLE_NAME);
   		}
   		
   		int v = getTransLevel(transAttribute);
   		this->transBegin(v);	   		
   		
   		SOActionHandle so(& manager);
   		so.configure(processname);
   		outBuf = so.execute(inBuf,undoflag);   		
   		
   		if(undoflag){ 
      	   string flowidname = LCU_FML_DEF_FLOWID;
      	   if(inBuf->IsFldExist(flowidname.c_str())){      	     
      	      logflag = true;
      	   } else {
      	      cout << "[sotest][worning]流程定义了需要回退,由于没有输入调用流水号,此次执行将无法回退."  << endl;
      	   }
   	   }    	 		   		
   	      	   
   	   //只有需要回退且 流程需要事务或已经在事务中
   	   if(logflag && ( v > 0 || this->isInTransLocal() || this->isInTransXA())){
   	      cout << "[sotest]此流程需要写流程日志,便于日后回退,由于是so测试，所以并未写入数据库中";
   	   }
   	   this->transCommit(true,ctx);   	   
   	   cout << "[sotest]测试流程执行结束." << endl;  
   	    	 
   	}catch(const mwException& e) {
   		errcode = -1;		
   		errinfo = e.message;   	
   		transCommit(false, ctx);
   	}catch(EUdbiException & e) {
   	   errcode = -1;
   		errinfo = e.what();
   		cout << e <<endl;
   	   transCommit(false, ctx);	
   	}catch(const exception& e) {
   		errcode = -1;
   		errinfo = e.what();
   		transCommit(false, ctx);
   	}catch(...) {
   		errcode = -1;		
   		errinfo = "未知异常";		
   		transCommit(false, ctx);
   	}			
   	if(errcode < 0){
   	   cout << errinfo <<endl;
   	   outBuf = inBuf;//如果有异常就取输入buffer返回
      }
   	if(!outBuf->IsFldExist(LCU_FML_RESULTERRINFO)) {
   		outBuf->SetString(LCU_FML_RESULTERRINFO,errinfo);
   	}
   	if(errcode != 0 && !outBuf->IsFldExist(LCU_FML_RESULTCODE)){
   		outBuf->SetInt(LCU_FML_RESULTCODE,errcode);		
   	} 
   	return outBuf;
}

SOActionHandle::SOActionHandle(DynamicLibraryManager * _manager) {       
   this->manager = _manager;
}    

SOActionHandle::~SOActionHandle(){
}

PCFmlBuf SOActionHandle::execute(PCFmlBuf inBuf,bool isUndo){
   PCFmlBuf outbuf;
   try {
         if(this->requestparam == this->responseparam){
            outbuf = inBuf;
         } else {
            outbuf = PCFmlBuf(new CFmlBuf());
         }         
         if(isUndo){           
            cout << "[sotest]Undo动态库调用开始:"<<libname <<"."<<LCU_METHOD_UNDO_PREF<<symbolname <<"("<<LCU_FML_UNDOBUFFER_NAME<<")"<< endl;
            UndoInvoker undoinvoker = (UndoInvoker)manager->getSymbol(libname, LCU_METHOD_UNDO_PREF + symbolname);
            inBuf->SetInt(LCU_FML_LOGSTEP_NAME,1);
            outbuf = inBuf;                                       
            undoinvoker(*outbuf);
         } else {
            cout << "[sotest]动态库运行开始:"<<libname <<"."<< symbolname <<"(" <<requestparam << "," << responseparam <<")"<< endl;
            Invoker invoker = (Invoker)manager->getSymbol(libname, symbolname);
            //调用完后取错误码
            invoker(*inBuf, *outbuf); 
         }     
        
    } catch (LibraryException& le ){
         cout << "[sotest]动态库函数句柄异常："<< le.what() << endl;              
         throw;        
    } catch (execution_error & ee){
    	   throw;   	
    } catch (Exception & ex){
       	cout << "[sotest]业务发生异常："<<ex.what()<< endl;
       	throw;
    } catch (std::exception & e){
       	cout << "[sotest]动态库运行发生异常："<<e.what()<< endl; 
       	throw; 
    } catch (...){
         cout << "[sotest]动态库发生未知异常."<< endl;
         throw;
    }  
    cout << "[sotest]动态库运行结束."<< endl; 
    return outbuf;
}
	
void SOActionHandle::configure(const string & configuration) {	
	   cout << "[sotest]定义需要调用动态库:" << configuration << endl; 	 
 	   int pos = configuration.find(".");
      if (pos == string::npos) throw definition_error(configuration + ", 协议错误");
      this->libname = configuration.substr(0, pos);
      int psc = configuration.find("(");
      if (psc == string::npos) throw definition_error(configuration + ", 无法解析出入口函数名称");
      this->symbolname = configuration.substr(pos + 1, psc-pos-1);
      int pend = configuration.find(")");
      string params = configuration.substr(psc + 1, pend-psc-1);
      //cout << "params:" <<params <<endl;
      int pps = params.find(",");
      if (pps == string::npos) throw definition_error(configuration + ", 无法解析出输入输出对象名称");
      this->requestparam = params.substr(0, pps);
      this->responseparam = params.substr(pps + 1,params.size()-pps-1);	 
      
}


definition_error::definition_error(const string & msg) : runtime_error(msg) {
}

definition_error::~definition_error() throw() {
}

execution_error::execution_error(const string & msg) : runtime_error(msg) {
}

execution_error::~execution_error() throw() {
}

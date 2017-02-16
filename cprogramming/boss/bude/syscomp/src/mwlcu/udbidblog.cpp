#include "mwlcu/fmlcontext.h" 
#include "mwlcu/udbidblog.h"
#include <exception>
#include "log4cpp/Logger.h"

using namespace std;
using namespace UDBI;
using namespace ebpm4c;	
using namespace log4cpp; 
namespace ebpm4c{


int UDBIUndoLogDao::getLogSteps(string flowid,int servidx){
   //cout << "[LCU]寻找回退LogStep:TRADE_ID=" << flowid<<",servidx=" << servidx;
   std::basic_stringstream <char> stream;
   string track;
   int count = 0;	
   try{
		JTable * rst = pcon->CreateJTable();           
		rst->AddSql("select nvl(nodetrack,'') As NODETRACK from TL_BPM_LCU_UNDO_LOG t where flowid=:FLOW and servidx = :SERV");
		rst->SetParam(":FLOW" , flowid);
		rst->SetParam(":SERV",servidx);           
		rst->ExecSelect();
		while(rst->Fetch()){
	   	   track = rst->GetString("NODETRACK"); 
	   	   stream << track;
	   	}
	   	delete rst;   	
   } catch(EUdbiException & e ){
      cout << "[LCU]UDBIUndoLogDao::getLogSteps 读回退轨迹异常:" << e <<endl;
      throw;
   }
	stream >> track;   	
	if("" == track){ 
	   //throw execution_error("[LCU]流程回退轨迹不存在,无法回退. FLOWID:"+flowid) ;
      THROW(LCUExecutionException,"流程回退轨迹不存在,无法回退. FLOWID:"+flowid) ;
   }
	string tmp;
   int his = 0;
	int pos = 0;
	pos = track.find(';',his);
	//cout <<"Step TRACK:"<< track <<endl;
	while(pos >= 0){
	  tmp = track.substr(his, pos - his); 
	  his = tmp.find("sub"); 	 
	  if(his < 0) count++;
	  his = pos + 1 ;	 
	  pos = track.find(';',his);	  
   }    

   return count;
}

int64_t UDBIUndoLogDao::readlog(vector<string>& idlist,string flowid, int servidx,int processidx, string definitionName){
    int64_t defid;
    try {
        JTable * rst = pcon->CreateJTable();
        //cout << flowid <<"/"<<processidx << "/" <<definitionName<<endl;
        
        rst->AddSql("select nvl(nodetrack,'') As NODETRACK,nvl(definitionid,'') as DEFID from TL_BPM_LCU_UNDO_LOG t where flowid=:FLOW and servidx = :SERV and processidx =:PROC and definitionname=:DEFNAME ");
        rst->SetParam(":FLOW" , flowid);
        rst->SetParam(":SERV",servidx);
        rst->SetParam(":PROC" , processidx);
        rst->SetParam(":DEFNAME" , definitionName);
        
        rst->ExecSelect();
   	
        string track;	
        if(rst->Fetch()){
            track = rst->GetString("NODETRACK");
            defid = rst->GetInt64("DEFID");
        }
        delete rst;
        if("" == track) {
            //throw execution_error("流程回退轨迹不存在,无法回退. FLOWID:"+flowid) ;
            THROW(LCUExecutionException,"流程回退轨迹不存在,无法回退. FLOWID:"+flowid) ;
        }
        string tmp;
        int his = 0;
        int pos = 0;
        pos = track.find(';',his);
        //cout << "READ TRACK:" << track <<endl;
        while(pos >= 0){
            tmp = track.substr(his, pos - his);   	 
            idlist.insert(idlist.end(),tmp);
            his = pos + 1 ;	 
            pos = track.find(';',his);	  
        }    
    } catch(EUdbiException & e ){
        cout << "[LCU]UDBIUndoLogDao::readlog 读回退轨迹异常:" << e <<endl;
        throw;
    }
   return defid;
}

void UDBIUndoLogDao::setUndoSucess(string flowid ,int servidx,int processidx,const int64_t& def_id){
  
    JTable * rst = pcon->CreateJTable();  
    rst->AddSql("update TL_BPM_LCU_UNDO_LOG set ISUNDOABLE = 0 , undodate = sysdate where flowid=:FLOW and servidx = :SERV and processidx =:PROC  and definitionid=:DEFID");
    rst->SetParam(":FLOW",flowid);
    rst->SetParam(":SERV",servidx);
    rst->SetParam(":PROC",processidx);	
    rst->SetParam(":DEFID",int64str(def_id));      
    rst->ExecUpdate();	    
    delete rst;	
}
  

void UDBIUndoLogDao::writelog(const int64_t& definitionid,string& definitionname,Context& ctx){
    string flowid;
    int processidx;
    string nodetrack;
    
    const vector <pair<Node* ,int> >&  pnodes = ctx.getLogs();       
    vector<pair<Node* ,int> >::const_iterator it = pnodes.begin(); 
    
    Node* pnode;
    string def_id;	  
    std::basic_stringstream <char> deff;
    deff << definitionid; 
    deff >> def_id;
   
    std::basic_stringstream <char> ss;
    for(; it != pnodes.end(); it++){ 
      pnode = (Node*)(it->first);       
      if(dynamic_cast<ProcessState*>(pnode)){
        ss <<"sub(" << pnode->getId() << ","<< it->second << ");";
      } else if(dynamic_cast<State*>(pnode)) {
        ss << pnode->getId() <<";";
      }     
    } 
    ss >> nodetrack;
    FMLContext& fmlctx = (FMLContext&)ctx;
    flowid = fmlctx.getFlowId();
    processidx = fmlctx.getProcessId();
    //string date_ = getCurrentTime();
    //cout << "[UNDOLOG]TRANSCODE:" << definitionname <<endl;
    //cout << "[UNDOLOG]LOGID:" << flowid << endl;
    //cout << "[UNDOLOG]PROCID:" << processidx << endl;
    //cout << "[UNDOLOG]DEFID:" << def_id << endl;    
    //cout << "[UNDOLOG]DEFNAME:" << definitionname << endl;
    //cout << "[UNDOLOG]TRACK:" << nodetrack << endl;
    //cout << "[UNDOLOG]Date_:" << date_ << endl;   
   if(flowid == ""){
      //cout << "[LCU]没有流水号,不需要输入到日志表中." << endl;
      return;
   }
   if(nodetrack == "") {
      //cout << "[LCU]没有调用轨迹,不需要输入到日志表中."<<endl;
      return;
   }

   int servidx= fmlctx.getServIdx();
   JTable * rst = pcon->CreateJTable();   
   try {   
        rst->AddSql("insert into TL_BPM_LCU_UNDO_LOG(flowid,servidx,processidx,definitionid,definitionname,nodetrack,extainfo,date_,isundoable,MONTH) ");
        rst->AddSql("values(:FLOW,:SERV, :PROC, :DEFID, :DEFNAME, :TRACK, ' ', sysdate, 1, to_number(to_char(sysdate,'mm')))");
       
        rst->SetParam(":FLOW",flowid);
        rst->SetParam(":SERV",servidx);
        rst->SetParam(":PROC",processidx);
        rst->SetParam(":DEFID",def_id);
        rst->SetParam(":DEFNAME",definitionname);
        rst->SetParam(":TRACK",nodetrack);
        //rst->SetParam(":DATE_",date_); 
        rst->ExecInsert();  
	   
    } catch (EUdbiException& e){
	     cout << "[LCU]插入UNDO日志EUdbiException:" <<e<<endl;
	     //Logger& log = Logger::getLogger(LCU_LOG_TRACE);
	     //log.alert(Grade::MINOR,e.getCode(),e,e.what());   
	     delete rst;        
	     throw;
    }catch(...){
	     cout << "[LCU]插入UNDO日志Exception" <<endl;
	     delete rst; 
	     throw;
    }
    //cout << "[LCU]插入UNDO日志成功:" <<flowid <<endl;
	delete rst;
}

bool  UDBIUndoLogDao::isUndoFinished(string flowid,int servidx,int processidx,string procname ){
    cout << "[LCU]寻找回退轨迹:TRADE_ID=" << flowid<<",procename=" << procname;
    JTable * rst = pcon->CreateJTable();
    string u; 
	try {
		    rst->AddSql("select nvl(isundoable,1) As ISUNDOABLE from TL_BPM_LCU_UNDO_LOG t where flowid=to_number(:FLOW) and servidx =:SERV and processidx =:PROC and definitionname=:PRCNAME");
		    rst->SetParam(":FLOW",flowid);
		    rst->SetParam(":SERV",servidx);    
		    rst->SetParam(":PROC",processidx);
		    rst->SetParam(":PRCNAME",procname);
		    rst->ExecSelect();	
		    
		    if(rst->Fetch()){	   
		       u = rst->GetString("ISUNDOABLE");       
		    } else {
		        cout << "[LCU]没有找到UNDO轨迹,流水号为:" << flowid << " 服务号:"<< servidx << " 子流程号:" << processidx << endl;
		    }
	} catch(EUdbiException & e ){
      cout << "[LCU]UDBIUndoLogDao::isUndoFinished 读回退轨迹异常:" << e <<endl;
      delete rst; 
      throw;
	}
    delete rst;    
    return   ( u == "0") ? true : false;   
}

int  UDBIUndoLogDao::getLogServCount(string flowid){
    int r = 0;
    JTable * rst = pcon->CreateJTable();
    rst->AddSql("select count(distinct(servidx)) as SERVCOUNT from TL_BPM_LCU_UNDO_LOG t where flowid= :FLOW");
    rst->SetParam(":FLOW",flowid);  
    rst->ExecSelect();	
    if(rst->Fetch()){	   
        r = rst->GetInt("SERVCOUNT");
    }
    delete rst;
    return r;   
}

void  UDBIUndoLogDao::insertMonitorInfo(string process,string server,string servindex,
	string staffid,string depatid,string eparchy_code,string city_code){
   
    JTable * rst = pcon->CreateJTable();
    try {
	    rst->AddSql("insert into TL_S_LCU_MONITOR(OPER_TIME,X_TRANS_CODE,SERVICE_NAME,SERVIDX,TRADE_STAFF_ID,TRADE_DEPART_ID,TRADE_EPARCHY_CODE,TRADE_CITY_CODE,MONTH)");
	    rst->AddSql(" values(sysdate,:FLOW,:SERV,:SIDX,:STAFF, :DEPA, :EPA,:CITY, to_number(to_char(sysdate,'mm')))");
	    rst->SetParam(":FLOW",process);  
	    rst->SetParam(":SERV",server);
	    rst->SetParam(":SIDX",servindex);	    
        rst->SetParam(":STAFF",staffid);
        rst->SetParam(":DEPA", depatid);
        rst->SetParam(":EPA", eparchy_code);
        rst->SetParam(":CITY", city_code);
       
        rst->ExecInsert();        
        
    } catch (...){
       delete rst;
       throw;
    }
    delete rst;    
}

};

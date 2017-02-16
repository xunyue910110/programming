#include "Procedure.h"
#include "base/StringUtil.h"
#include "RateGlobalInfo.h"


Procedure::Procedure(){};

Procedure::~Procedure() {};

void Procedure::setRateInfoObj(RateGlobalInfo *r_pRateInfo)
{
  m_pRateInfo    = r_pRateInfo;
  m_pcdr         = r_pRateInfo->m_pCdr;
  m_pmManager    = r_pRateInfo->m_paramMgr;
  m_pRateConfig  = r_pRateInfo->m_pConfig;
  m_billMgr      = r_pRateInfo->m_billMgr;
}

bool  Procedure::initialize() 
{
    return true;
}


int Procedure::reset() 
{
    return 0;
}

int Procedure::execute() 
{
    return 0;
}





///////////////////////////////////////////////////////////////////////////////

map<string,Procedure*> * ProcedureHelper::GetProcedures() 
{
    static map<string,Procedure*> procedures;
    return &procedures;
}

Procedure * ProcedureHelper::GetProcedure(const string & name) 
{
    map<string,Procedure*> &procs = *(GetProcedures());
    
    map<string,Procedure*>::iterator itr;
    itr=procs.find(name);
    if (itr != procs.end()) 
    {
        return itr->second;  
    }
    return 0;          
};

int ProcedureHelper::SetProcedure(const string &name,Procedure * procedure) 
{
    map<string,Procedure*> &procs = *(GetProcedures());
    Procedure * p =GetProcedure(name);
    if (p==0)  {
        procs[name] = procedure;
    }
    procedure->m_className = name;
    
    #ifdef _PRICE_TRACE_DUMP_
    	cout << "register class instance:" << name << endl;
    #endif
    
    return 0;
}

//  1  --- 没有相应的过程
//  0  --- 完全成功
// <0  --- 错误

int ProcedureHelper::ExecuteProcedure(const string &name) 
{
    string proc_name,proc_arguments = "";
    vector<string> procs;procs.clear();

    StringUtil::split(name,"(",procs,true);
    if (procs.size() > 1) 
    {
        proc_name = procs[0];
    } else 
    {
        proc_name = name;
    }

    Procedure *proc = ProcedureHelper::GetProcedure(proc_name);
    if (proc == 0)
        return 9;

	  #ifdef _DEBUG_
    	//cout << "execute procedure:" << proc->m_className << endl; 
    #endif

    //的到过程参数 XXXXProc(args)
    if (procs.size() > 1) 
    {
        proc_arguments = procs[1];
        proc_arguments.erase(proc_arguments.find_last_not_of(")")+1,string::npos);
    }

    proc->m_arguments = proc_arguments;
    proc->reset();
    return proc->execute();
}

//不带参数方式执行
int ProcedureHelper::ExecuteProcedure(Procedure* pProc)
{
  pProc->m_arguments = "";
  pProc->reset();  
  return pProc->execute();
}


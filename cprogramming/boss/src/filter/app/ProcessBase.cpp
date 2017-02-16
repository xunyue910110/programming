#include "ProcessBase.h"
#include "base/StringUtil.h"

ProcessBase::ProcessBase(){};

ProcessBase::~ProcessBase() {};

void ProcessBase::setGlobalInfo(FilterGlobalInfo *r_pFilterInfo)
{
  m_FilterInfo     = r_pFilterInfo;
  m_cdr            = r_pFilterInfo->m_cdr;
  m_pcdr           = &(r_pFilterInfo->m_cdr);
  m_Config         = r_pFilterInfo->m_Config;
  m_globalManager  = r_pFilterInfo->m_globalManager;
}

bool  ProcessBase::initialize()
{
    return true;
}

int ProcessBase::reset()
{
    return 0;
}

int ProcessBase::execute()
{
    return 0;
}

///////////////////////////////////////////////////////////////////////////////

map<string,ProcessBase*> * ProcessBaseHelper::GetProcedures()
{
    static map<string,ProcessBase*> processbase;
    return &processbase;
}

ProcessBase * ProcessBaseHelper::GetProcedure(const string & name)
{
    map<string,ProcessBase*> &procs = *(GetProcedures());

    map<string,ProcessBase*>::iterator itr;

    itr=procs.find(name);
    if (itr != procs.end())
    {
        return itr->second;
    }
    return 0;
}

int ProcessBaseHelper::SetProcedure(const string &name,ProcessBase * processbase)
{
    map<string,ProcessBase*> &procs = *(GetProcedures());

    ProcessBase * p =GetProcedure(name);
    if(p==0)
    {
      procs[name] = processbase;
    }
    processbase->m_className = name;

    #ifdef _PROCESS_TRACE_DUMP_
    	cout << "register class instance:" << name << endl;
    #endif

    return 0;
}

//  1  --- 没有相应的过程
//  0  --- 完全成功
// <0  --- 错误
int ProcessBaseHelper::ExecuteProcedure(const string &name)
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

    ProcessBase *proc = ProcessBaseHelper::GetProcedure(proc_name);
    if (proc == 0)
        return 9;

	#ifdef _PROCESS_TRACE_DUMP_
    	cout << "execute procedure:" << proc->m_className << endl;
    #endif

    //得到过程参数 XXXXProc(args)
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
int ProcessBaseHelper::ExecuteProcedure(ProcessBase* pProc)
{
  pProc->m_arguments = "";
  pProc->reset();
  return pProc->execute();
}


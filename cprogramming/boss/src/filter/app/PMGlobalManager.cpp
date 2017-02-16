#include "PMGlobalManager.h"
#include "Inoutputer.h"
#include "base/StringUtil.h"

#include "appfrm/Application.h"
#include "appfrm/ErrorHandle.h"

bool PMGlobalManager::initialize(FilterConfig *r_pConfig,int r_indicator)
{

  setAppName(r_pConfig->m_appName);
  setChannelNo(r_pConfig->m_channelNo);
  setDbConnInfo(r_pConfig->m_dbUser,r_pConfig->m_dbPassword,r_pConfig->m_dbServName);

  PMHelperall::m_indicator = r_indicator;
  map<string,ParamContainerBase*>::iterator t_itr;	       
  map<string,ParamContainerBase*> & t_containers = *(PMHelperall::GetParamContainers());
  for (t_itr=t_containers.begin(); t_itr!=t_containers.end(); ++t_itr) 
  {
	  m_paramContainerList.push_back((t_itr->second));
	}
	//指示完成，将指示器值置为0
  if(PMHelperall::m_indicator!=0)
  {
    PMHelperall::m_indicator=0;
  }
  //初始化DB，加载到私有内存
  if(!initialization())
  {
     #ifdef _DEBUG_
       cout<<"initial dbparam false!"
           <<__FILE__<<__LINE__<<endl;
     #endif
     theErrorMessages->insert(FILTER_ERROR_INITIALIZE,"initial dbparam false!: " + m_errorMessage);
     return false;
  }
  setUpdateInterval(r_pConfig->m_interval);

  //一次分拣暂设定不连接MDB
  if(r_pConfig->m_inputRuleMgr.m_isNeeduseMdb == 'Y' || r_pConfig->m_inputRuleMgr.m_isNeeduseMdb == 'y')
  {
     if(setMdbInfo(r_pConfig->m_mdbInfo)==false)
     {
       #ifdef _DEBUG_
         cout<<"setMdbInfo("<<r_pConfig->m_mdbInfo<<") false!"
             <<__FILE__<<__LINE__<<endl;
       #endif
       return false;
      }
  }

  #ifdef _DEBUG_
		cout<<"m_dbUser:"<<r_pConfig->m_dbUser<<endl;
		cout<<"m_dbPassword:"<<r_pConfig->m_dbPassword<<endl;
		cout<<"m_dbServName:"<<r_pConfig->m_dbServName<<endl;
    //dump2File(); 
  #endif

  return true;  
}

//dump param data to files 
void PMGlobalManager::dump2File() 
{
  list<ParamContainerBase*>::iterator t_itr;

  for (t_itr = m_paramContainerList.begin(); 
       t_itr != m_paramContainerList.end(); ++t_itr) 
  {
    ParamContainerBase &t_pc = **t_itr;
    PO_Property t_prop;
    t_pc.getProperty(t_prop);

    string t_path = "./dump/";
    t_path += m_appName;
    string t_file = t_prop.fileName + "_" + StringUtil::toString(m_channelNo);
    Inoutputer t_outFile;
    t_outFile.setName(t_file.c_str(),t_path.c_str());
    t_outFile.open(ios::out);
    t_pc.dump(t_outFile.m_fstream);
    t_outFile.close();
    //cout << "dump data:" << t_path <<"/"<< t_file << endl;
  }
}


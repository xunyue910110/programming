#include "PMManager.h"
#include "Inoutputer.h"
#include "base/StringUtil.h"

bool PMManager::initialize(RateConfig *r_pConfig,int r_indicator)
{
  setAppName(r_pConfig->m_appName);                       
  setUpdateInterval(r_pConfig->m_interval);
  setChannelNo(r_pConfig->m_channelNo); 
  setDbConnInfo(r_pConfig->m_dbUser,r_pConfig->m_dbPassword,
                r_pConfig->m_dbServName);
  //if(setMdbInfo(r_pConfig->m_mdbInfo)==false)
  if(setMdbInfo(r_pConfig->m_mdbInfo_userInfo)==false)
  {
    #ifdef _DEBUG_
      cout<<"setMdbInfo("<<r_pConfig->m_mdbInfo_userInfo<<") false!"
          <<__FILE__<<__LINE__<<endl;
    #endif
    return false;
  }

  PMHelper::m_indicator = r_indicator;
  map<string,ParamContainerBase*>::iterator t_itr;	       
  map<string,ParamContainerBase*> & t_containers = *(PMHelper::GetParamContainers());
  for (t_itr=t_containers.begin(); t_itr!=t_containers.end(); ++t_itr) 
  {
	  m_paramContainerList.push_back((t_itr->second));
	}
	//指示完成，将指示器值置为0
  if(PMHelper::m_indicator!=0)
  {
    PMHelper::m_indicator=0;
  }
  if(!initialization()) 
  {
    #ifdef _DEBUG_
      cout<<"initialization false!"<<__FILE__<<__LINE__<<endl;
    #endif
    return false;
  }


  #ifdef _DEBUG_
    //dump2File(); 
  #endif
  return true;  
}

//dump param data to files 
void PMManager::dump2File() 
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
    cout << "dump data:" << t_path <<"/"<< t_file << endl;
  }
}



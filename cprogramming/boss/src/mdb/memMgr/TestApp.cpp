#include "TestApp.h"
#include <iostream>
#include <fstream>
//#include <errno.h>
#include "base/PString.h"
#include <fcntl.h>

#include "MemManager.h"

TestApp g_application;
Application * const theApp = &g_application;

TestApp::TestApp()
{
  m_pTableDesc = NULL;
  m_pIndexDesc = NULL;
}

TestApp::~TestApp()
{
}

bool TestApp::initialization()
{
  setoptstr("c:");
  
  if (!Application::initialization())
      return false;
  /*    
  #ifndef _DEBUG_ 
      set_asdaemon(true);
      set_onlyone(true);
      set_runinbkg(true);
  #endif
  */
  m_objectName = "程序初始化";
  m_status = PROC_READY;
  updateStatus();
  m_pMemMgr=NULL;
  
  return true;
}

bool TestApp::beforeLoop()
{
  return true;
}


bool TestApp::recovery()
{
  return true;
}

bool TestApp::loopProcess()
{
  dealCommand();
  m_stop=true;
  
  cout<<"已经运行完毕"<<__FILE__<<__LINE__<<endl;
  return true;
}

bool TestApp::end()
{
  delete m_pMemMgr;
  return Application::end();
}

bool TestApp::dealCommand()
{
  char argsl[256+1];
  while(m_stop==false)
  {
      printf("-->");
      fgets(argsl,255,stdin);
      argsl[strlen(argsl)-1]=0;
      if(strlen(argsl)==0) continue;
  	
      if(strcasecmp(argsl,"help")==0||
         strcasecmp(argsl,"h")==0)
      {
          help();
          continue;
      }
      if(strcasecmp(argsl,"quit")==0 ||
         strcasecmp(argsl,"exit")==0 ||
         strcasecmp(argsl,"q")==0 ||
         strcasecmp(argsl,"e")==0 ) 
         break;
         
      if(parseArgs(argsl)==false)
      {
        cout<<"命名错误 帮助请用help"<<endl;
        continue;
      }
      if(!processCommand()) 
      {
        cout<<"命名执行失败 processCommand"<<__FILE__<<__LINE__<<endl;
        //return false;
      }
  }

  return true;
}

void TestApp::help()
{
  cout<<"\t命令格式如下(命令不区分大小写)："<<endl;
  cout<<"\t h\\help for Help"<<endl;
  cout<<"\t CreateMdb MdbName"<<endl;
  //cout<<"\t StartMdb  MdbName"<<endl;
  cout<<"\t ConnectMdb   MdbName"<<endl;
  cout<<"\t DisconnectMdb "<<endl;
  cout<<"\t StopMdb   MdbName"<<endl;
  cout<<"\t GetSpaceInfo spaceName"<<endl;
  cout<<"\t GetSpaceInfos"<<endl;
  cout<<"\t GetTableDescList"<<endl;
  cout<<"\t GetTableDefList"<<endl;
  cout<<"\t GetTableDesc tableName"<<endl;
  cout<<"\t TruncateTb tableName"<<endl;
  cout<<"\t TruncateIdx indexName"<<endl;
  cout<<"\t GetTbMemInfo tableName/indexName"<<endl;
  cout<<"\t DropTable tableName"<<endl;
  cout<<"\t DropIndex indexName,tableName"<<endl;
  cout<<"\t GetIndexByName indexName"<<endl;
  cout<<"\t GetIndexByTable tableName"<<endl;
  cout<<"\t DumpSpaceInfo fileName"<<endl;
  cout<<"\t CreateSpace 表空间名称,磁盘文件名,表空间大小(M)"<<endl;
  cout<<"\t GetSpaceByTable tableName"<<endl;
  cout<<"\t AddTableSpace spaceName,tableName,tableType(2索引)"<<endl;
  cout<<"\t GetSessionInfos"<<endl;
  cout<<"\t RegSession"<<endl;
  cout<<"\t UnregSession sessionId"<<endl;
  cout<<"\t ClearSessions"<<endl;
  cout<<"\t AddGparam paramName,paramvalue"<<endl;
  cout<<"\t GetGparam paramName"<<endl;
  cout<<"\t GetGparamList"<<endl;
  cout<<"\t UpdateGparam paramName,paramvalue"<<endl;
  cout<<"\t DeleteGparam paramName"<<endl;
  return ;
}

bool TestApp::parseArgs(char *args)
{
  //cout<<"解析命令:"<<args<<" 开始!"<<endl;
  int i;
  //1. 分解为命令和参数
  for(i=0;i<(int)(COMMND_UNKNOW);i++)
  {
    if(strncasecmp(args,COMMAND_LIST[i],strlen(COMMAND_LIST[i]))==0)
    {
      m_commandType=(CommandType)i;
      m_pCommandParam = args+strlen(COMMAND_LIST[i]);
      break;
    }
  }
  if(i==(int)COMMND_UNKNOW)
  {
    cout<<"解析命令:"<<args<<" 失败!"<<endl;
    return false;
  }
  while(isspace(*m_pCommandParam) && (*m_pCommandParam!=0)) 
    m_pCommandParam++;

  //cout<<"解析命令:"<<args<<" 成功!"<<endl;
  return true;
}
bool  TestApp::processCommand()
{
  try
  {
    if(m_pMemMgr!=NULL)
    {
      if(m_pMemMgr->updateMgrInfo()==false)
      {
        cout<<"m_pMemMgr->updateMgrInfo false!"<<endl;
      }
    }
    bool t_bRet=true;
    switch(m_commandType)
    {
      case COMMND_CAPTEST:
        if(m_pMemMgr==NULL)
        {
          cout<<"未连接数据库!"<<__FILE__<<__LINE__<<endl;
          return false;
        }
        return cap_test();      
      case COMMND_CREATE_SPACE:
        if(m_pMemMgr==NULL)
        {
          cout<<"未连接数据库!"<<__FILE__<<__LINE__<<endl;
          return false;
        }
        return createSpace(m_pCommandParam);
        break;
        
      case COMMND_CREATE_MDB:
        if(m_pMemMgr!=NULL)
        {
          delete m_pMemMgr;
        }
        m_pMemMgr=new MemManager(m_pCommandParam);
        strcpy(m_dbName,m_pCommandParam);
        try
        {
          m_pMemMgr->creatMdb();
        }catch(Mdb_Exception &e)
        {
          delete m_pMemMgr;
          m_pMemMgr=NULL;
          throw e;
        }
        //暂时不delete
        delete m_pMemMgr;
        m_pMemMgr=NULL;
        return t_bRet;
      case COMMND_STARTE_MDB:
        if(m_pMemMgr!=NULL)
        {
          #ifdef _DEBUG_
            cout <<"数据库已连接!"<<__FILE__<<__LINE__<<endl;
          #endif
          return false;
        }
        m_pMemMgr=new MemManager(m_pCommandParam);
        strcpy(m_dbName,m_pCommandParam);
        try
        {
          m_pMemMgr->startMdb();
        }catch(Mdb_Exception &e)
        {
          delete m_pMemMgr;
          m_pMemMgr=NULL;
          throw e;
        }
        delete m_pMemMgr;
        m_pMemMgr=NULL;
        return t_bRet;
      case COMMND_OPEN_MDB:
        if(m_pMemMgr!=NULL)
        {
          #ifdef _DEBUG_
            cout <<"数据库已连接!"<<__FILE__<<__LINE__<<endl;
          #endif
          return false;
        }
        m_pMemMgr=new MemManager(m_pCommandParam);
        strcpy(m_dbName,m_pCommandParam);
        try
        {
          m_pMemMgr->openMdb();
        }catch(Mdb_Exception &e)
        {
          delete m_pMemMgr;
          m_pMemMgr=NULL;
          throw e;
        }
        return t_bRet;
      case COMMND_CREATETABLE:
        if(m_pMemMgr==NULL)
        {
          return false;
        }
        t_bRet=true;
        if(createTable()==false)
        {
          t_bRet = false;
        }
        return t_bRet;
      case COMMND_CREATEINDEX:
        if(m_pMemMgr==NULL)
        {
          return false;
        }
        t_bRet=true;
        if(createIndex()==false)
        {
          t_bRet = false;
        }
        return t_bRet;
      case COMMND_CLOSE_MDB:
        if(m_pMemMgr==NULL)
        {
          cout<<"未连接数据库!"<<__FILE__<<__LINE__<<endl;
          return false;
        }
        try
        {
          m_pMemMgr->closeMdb();
        }catch(Mdb_Exception &e)
        {
          delete m_pMemMgr;
          m_pMemMgr=NULL;
          throw e;
        }
        delete m_pMemMgr;
        m_pMemMgr = NULL;
        return t_bRet;
      case COMMND_GETSPACEINFOS:
        {
          vector<SpaceInfo> t_spaceList;
          if(m_pMemMgr==NULL)
          {
            cout<<"未连接数据库!"<<__FILE__<<__LINE__<<endl;
            return false;
          }
          if(m_pMemMgr->getSpaceInfoList(t_spaceList)==false)
          {
            cout<<"获取信息失败!"<<__FILE__<<__LINE__<<endl;
            return false;
          }
          cout<<"数据表空间信息如下：除了控制区表空间!"<<endl;
          for(vector<SpaceInfo>::iterator t_spItr=t_spaceList.begin();
              t_spItr!=t_spaceList.end();t_spItr++)
          {
            cout<<"--------------------------------------"<<endl;
            cout<<*t_spItr;
            cout<<"--------------------------------------"<<endl;
          }
          return true;
        }
      case COMMND_GETSPACEINFO:
        {
           SpaceInfo  t_spaceInfo;
          if(m_pMemMgr==NULL)
          {
            cout<<"未连接数据库!"<<__FILE__<<__LINE__<<endl;
            return false;
          }
          if(m_pMemMgr->getSpaceInfo(m_pCommandParam,t_spaceInfo)==false)
          {
            cout<<"获取Space:{"<<m_pCommandParam<<"}信息失败!"<<__FILE__<<__LINE__<<endl;
          }
          cout<<"--------------------------------------"<<endl;
          cout<<t_spaceInfo;
          cout<<"--------------------------------------"<<endl;
          return true;
        }  
      case COMMND_GETTABLEDESCLIST:
        {
          vector<TableDesc>  t_tableDescList;
          if(m_pMemMgr==NULL)
          {
            cout<<"未连接数据库!"<<__FILE__<<__LINE__<<endl;
            return false;
          }
          if(m_pMemMgr->getTableDescList(t_tableDescList)==false)
          {
            cout<<"获取表描述符信息列表失败!"<<__FILE__<<__LINE__<<endl;
          }
          for(vector<TableDesc>::iterator t_tbItr=t_tableDescList.begin();
              t_tbItr!=t_tableDescList.end();t_tbItr++)
          {
            cout<<"--------------------------------------"<<endl;
            cout<<*t_tbItr;
            cout<<"--------------------------------------"<<endl;
          }
          return true;
        }        
      case COMMND_GETTABLEDEFLIST:
        {
          vector<TableDef>  t_tableDefList;
          if(m_pMemMgr==NULL)
          {
            cout<<"未连接数据库!"<<__FILE__<<__LINE__<<endl;
            return false;
          }
          if(m_pMemMgr->getTableDefList(t_tableDefList)==false)
          {
            cout<<"获取表定义信息列表失败!"<<__FILE__<<__LINE__<<endl;
          }
          for(vector<TableDef>::iterator t_tbdefItr=t_tableDefList.begin();
              t_tbdefItr!=t_tableDefList.end();t_tbdefItr++)
          {
            cout<<"--------------------------------------"<<endl;
            cout<<*t_tbdefItr;
            cout<<"--------------------------------------"<<endl;
          }
          return true;
        }        
      case COMMND_GETTABLEDESC:
        {
          TableDesc  *t_pTableDesc;
          if(m_pMemMgr==NULL)
          {
            cout<<"未连接数据库!"<<__FILE__<<__LINE__<<endl;
            return false;
          }
          if(m_pMemMgr->getTableDescByName(m_pCommandParam,t_pTableDesc)==false)
          {
            cout<<"获取表描述符信息失败!tablename="<<m_pCommandParam
                <<" "<<__FILE__<<__LINE__<<endl;
          }else
          {
            cout<<*t_pTableDesc<<endl;
          }
          return true;
        }        
      case COMMND_NEWTBMEM:
          return newTbMem(m_pCommandParam);
      case COMMND_NEWINDEXMEM:  
          return newIndexMem(m_pCommandParam);
      case COMMND_PRINTMEM:
          return printMemInfo();
      case COMMND_TRUNCATETB:
        {
          if(m_pMemMgr==NULL)
          {
            cout<<"未连接数据库!"<<__FILE__<<__LINE__<<endl;
            return false;
          }
          if(m_pMemMgr->truncateTable(m_pCommandParam)==false)
          {
            cout<<"truncateTable("<<m_pCommandParam<<")失败!"<<__FILE__<<__LINE__<<endl;
          }
          m_tbmemList.clear();
          return true;
        }   
          
      case COMMND_TRUNCATEIDX:
        {
          if(m_pMemMgr==NULL)
          {
            cout<<"未连接数据库!"<<__FILE__<<__LINE__<<endl;
            return false;
          }
          if(m_pMemMgr->truncateIndex(m_pCommandParam)==false)
          {
            cout<<"truncateIndex("<<m_pCommandParam<<")失败!"<<__FILE__<<__LINE__<<endl;
          }
          m_idxmemList.clear();
          return true;
        }   
      case COMMND_FREETBMEM:
       {
          if(m_pMemMgr==NULL)
          {
            cout<<"未连接数据库!"<<__FILE__<<__LINE__<<endl;
            return false;
          }
          
          return freeTbMem(m_pCommandParam);
        }     
      case COMMND_FREEIDXMEM:
       {
          if(m_pMemMgr==NULL)
          {
            cout<<"未连接数据库!"<<__FILE__<<__LINE__<<endl;
            return false;
          }
          return freeIdxMem(m_pCommandParam);
        }     
      case COMMND_GETTBINFO:
       {
          if(m_pMemMgr==NULL)
          {
            cout<<"未连接数据库!"<<__FILE__<<__LINE__<<endl;
            return false;
          }
          TbMemInfo t_memInfo;
          if(m_pMemMgr->getTableMemInfo(m_pCommandParam,t_memInfo)==false)
          {
            cout<<"getTableMemInfo("<<m_pCommandParam<<")失败!"<<__FILE__<<__LINE__<<endl;
          }
          t_memInfo.dumpInfo(cout);
          return true;
        }     
      case COMMND_DROPTABLE:
        {
          if(m_pMemMgr==NULL)
          {
            cout<<"未连接数据库!"<<__FILE__<<__LINE__<<endl;
            return false;
          }
          if(m_pMemMgr->dropTable(m_pCommandParam)==false)
          {
            cout<<"删除表"<<m_pCommandParam<<"失败!"<<__FILE__<<__LINE__<<endl;
          }
          return true;
        }   
      case COMMND_DROPINDEX:
        {
          if(m_pMemMgr==NULL)
          {
            cout<<"未连接数据库!"<<__FILE__<<__LINE__<<endl;
            return false;
          }
          if(dropIndex(m_pCommandParam)==false)
          {
            cout<<"删除表"<<m_pCommandParam<<"失败!"<<__FILE__<<__LINE__<<endl;
          }
          return true;
        }   
      case COMMND_GETINDEXBYNAME:
        {
          IndexDesc *t_pIndexDesc;
          if(m_pMemMgr==NULL)
          {
            cout<<"未连接数据库!"<<__FILE__<<__LINE__<<endl;
            return false;
          }
          if(m_pMemMgr->getIndexDescByName(m_pCommandParam,t_pIndexDesc)==false)
          {
            cout<<"getIndexDescByName:"<<m_pCommandParam<<"失败!"<<__FILE__<<__LINE__<<endl;
          }else
          {
            cout<<*t_pIndexDesc;
          }
          return true;
        }   
      case COMMND_GETINDEXSBYTABLE:
        {
          vector<IndexDef> t_indexDefList;
          if(m_pMemMgr==NULL)
          {
            cout<<"未连接数据库!"<<__FILE__<<__LINE__<<endl;
            return false;
          }
          if(m_pMemMgr->getIndexListByTable(m_pCommandParam,t_indexDefList)==false)
          {
            cout<<"getIndexListByTable:"<<m_pCommandParam<<"失败!"<<__FILE__<<__LINE__<<endl;
          }else
          {
            for(vector<IndexDef>::iterator t_indexDefItr=t_indexDefList.begin();
                t_indexDefItr!=t_indexDefList.end();t_indexDefItr++)
            {
              cout<<*t_indexDefItr<<endl;
            }
          }
          return true;
        }   
      case COMMND_DUMPSPACEINFO:
        {
          if(m_pMemMgr==NULL)
          {
            cout<<"未连接数据库!"<<__FILE__<<__LINE__<<endl;
            return false;
          }
          dumpSpaceInfo(m_pCommandParam);
          return true;
        }   
      case COMMND_GETSPACEBYTABLE:
        {
          if(m_pMemMgr==NULL)
          {
            cout<<"未连接数据库!"<<__FILE__<<__LINE__<<endl;
            return false;
          }
          vector<string> t_spaceList;
          if(m_pMemMgr->getSpaceListByTable(m_pCommandParam,t_spaceList)==false)
          {
            if(m_pMemMgr->getSpaceListByIndex(m_pCommandParam,t_spaceList)==false)
            {
              cout<<"GetSpaceByTable false!"<<endl;
              return false;
            }
          }
          for(vector<string>::iterator t_spItr=t_spaceList.begin();
              t_spItr!=t_spaceList.end();t_spItr++)
          {
            cout<<*t_spItr<<endl;
          }
          return true;
        } 
      case COMMND_ADDTABLESPACE:
        {
          if(m_pMemMgr==NULL)
          {
            cout<<"未连接数据库!"<<__FILE__<<__LINE__<<endl;
            return false;
          }
          if(addTableSpace(m_pCommandParam)==false)
          {
            cout<<"addTableSpace("<<m_pCommandParam<<") failed!"<<endl;
          }
          return true;
        } 
      case COMMND_GETSESSIONINFOS:
        {
          if(m_pMemMgr==NULL)
          {
            cout<<"未连接数据库!"<<__FILE__<<__LINE__<<endl;
            return false;
          }
          vector<SessionInfo> t_sessionList;
          if(m_pMemMgr->getSessionInfos(t_sessionList)==false)
          {
            cout<<"getSessionInfos() failed!"<<endl;
          }else
          {
            for(vector<SessionInfo>::iterator r_sessionItr=t_sessionList.begin();
                r_sessionItr!=t_sessionList.end();r_sessionItr++)
            {
              cout<<"\t"<<*r_sessionItr;
            }
          }
          return true;
        } 
      case COMMND_REGSESSION:
        {
          if(m_pMemMgr==NULL)
          {
            cout<<"未连接数据库!"<<__FILE__<<__LINE__<<endl;
            return false;
          }
          SessionInfo t_session;
          t_session.m_pId = getpid();
          time(&(t_session.m_time));
          strcpy(t_session.m_ipAddr,"192.168.4.114");
          if(m_pMemMgr->registerSession(t_session)==false)
          {
            cout<<"getSessionInfos() failed!"<<endl;
            
          }else
          {
            cout<<"t_session = "<<t_session<<endl;
          }
          return true;
        } 
      case COMMND_UNREGSESSION:
        {
          if(m_pMemMgr==NULL)
          {
            cout<<"未连接数据库!"<<__FILE__<<__LINE__<<endl;
            return false;
          }
          SessionInfo t_session;
          t_session.m_sessionId = atoi(m_pCommandParam);
          if(m_pMemMgr->unRegisterSession(t_session)==false)
          {
            cout<<"unRegisterSession("<<m_pCommandParam<<") failed!"<<endl;
          }
          return true;
        }       
      case COMMND_CLEARSESSIONS:
         {
          if(m_pMemMgr==NULL)
          {
            cout<<"未连接数据库!"<<__FILE__<<__LINE__<<endl;
            return false;
          }
          if(m_pMemMgr->clearSessionInfos()==false)
          {
            cout<<"unRegisterSession("<<m_pCommandParam<<") failed!"<<endl;
          }
          return true;
        }   
      case COMMND_ADDGPARAM:
        {
          if(m_pMemMgr==NULL)
          {
            cout<<"未连接数据库!"<<__FILE__<<__LINE__<<endl;
            return false;
          }
          return addGparam(m_pCommandParam);
        }       
      case COMMND_GETGPARAM:
        {
          if(m_pMemMgr==NULL)
          {
            cout<<"未连接数据库!"<<__FILE__<<__LINE__<<endl;
            return false;
          }
          GlobalParam t_gParam;
          if(m_pMemMgr->getGlobalParam(m_pCommandParam,t_gParam)==false)
          {
            cout<<"getGlobalParam("<<m_pCommandParam<<") failed!"<<endl;
          }else
          {
            cout<<t_gParam;
          }
          return true;
        }       
      case COMMND_GETGPARAMLIST:
        {
          if(m_pMemMgr==NULL)
          {
            cout<<"未连接数据库!"<<__FILE__<<__LINE__<<endl;
            return false;
          }
          vector<GlobalParam> t_gParamList;
          if(m_pMemMgr->getGlobalParams(t_gParamList)==false)
          {
            cout<<"getGlobalParams() failed!"<<endl;
          }else
          {
            for(vector<GlobalParam>::iterator t_gpItr=t_gParamList.begin();
                t_gpItr!=t_gParamList.end();t_gpItr++)
            {
              cout<<*t_gpItr;
            }
          }
          return true;
        }       
      case COMMND_UPDATEGPARAM:
        {
          if(m_pMemMgr==NULL)
          {
            cout<<"未连接数据库!"<<__FILE__<<__LINE__<<endl;
            return false;
          }
          return updateGparam(m_pCommandParam);
        }       
        
      case COMMND_DELGPARAM:
        {
          if(m_pMemMgr==NULL)
          {
            cout<<"未连接数据库!"<<__FILE__<<__LINE__<<endl;
            return false;
          }
          if(m_pMemMgr->deleteGlobalParam(m_pCommandParam)==false)
          {
            cout<<"deleteGlobalParam("<<m_pCommandParam<<") failed!"<<endl;
          }
          return true;
        }       
      case COMMND_GETRECLIST:
         {
          if(m_pMemMgr==NULL)
          {
            cout<<"未连接数据库!"<<__FILE__<<__LINE__<<endl;
            return false;
          }
          if(getRecList()==false)
          {
            return false;
          }
          return true;
        }   
      case COMMND_STOP_MDB:
        if(m_pMemMgr!=NULL)
        {
          delete m_pMemMgr;
          m_pMemMgr=NULL;
        }
        m_pMemMgr=new MemManager(m_pCommandParam);
        strcpy(m_dbName,m_pCommandParam);
        try
        {
          m_pMemMgr->stopMdb();
        }catch(Mdb_Exception &e)
        {
          delete m_pMemMgr;
          m_pMemMgr=NULL;
          throw e;
        }
          delete m_pMemMgr;
          m_pMemMgr=NULL;
        return true;
      default:
        cout<<"未知命令,实行命令失败!"<<endl;
        return false;
    };
  }catch(Mdb_Exception &e)
  {
    cout<<e.GetString()<<endl;
  }
  return true;
}
bool TestApp::createSpace(const char * param)
{
  try
  {
  SpaceInfo t_spaceInfo;
  PString t_param;
  vector<PString> t_paramList;
  t_param = param;
  t_param.splitTrim(',',t_paramList);
  if(t_paramList.size()!=3)
  {
    cout<<"参数个数错误!"<<__FILE__<<__LINE__<<endl;
    return false;
  }
  strcpy(t_spaceInfo.m_dbName,m_dbName);
  t_spaceInfo.m_type     = TABLE_SPACE;
  strcpy(t_spaceInfo.m_spaceName,t_paramList[0].c_str());
  strcpy(t_spaceInfo.m_fileName,t_paramList[1].c_str());
  t_spaceInfo.m_size = atol(t_paramList[2].c_str())*1024*1024;
  if(t_spaceInfo.m_size<=0) 
  {
    cout<<"表空间大小参数错误!"<<__FILE__<<__LINE__<<endl;
    return false;
  }
  bool    t_bRet=false;
  
 t_bRet = m_pMemMgr->createTbSpace(t_spaceInfo);
  if(t_bRet==true)
  {
    cout<<"创建表空间成功!"<<__FILE__<<__LINE__<<endl;
    cout<<"-----t_spaceInfo-----"<<endl;
    cout<<t_spaceInfo;
    cout<<"---------------------"<<endl;
  }else
  {
    cout<<"创建表空间失败!"<<__FILE__<<__LINE__<<endl;
  }           
  }catch(Mdb_Exception &e)
  {
    throw e;
  }

  return true;
}



bool TestApp::dumpSpaceInfo(char * r_fileName)
{
  int fd= open(r_fileName, O_RDWR|O_CREAT,0666);
  if(fd<0)
  {
    cout<<"open r_fileName false!"<<endl;
    return false;
  }
  if(dup2(fd, STDERR_FILENO)!=STDERR_FILENO)
  {
    cout<<"dup2 file error!"<<endl;
    close(fd);
    return false;
  }
  
  m_pMemMgr->dumpSpaceInfo(cerr);    
  cerr.flush();
  cerr.clear();
  close(fd);
  return true;
}  

bool TestApp::addTableSpace(const char * param)
{
  bool t_bRet=true;
  PString t_param;
  vector<PString> t_paramList;
  t_param = param;
  t_param.splitTrim(',',t_paramList);
  if(t_paramList.size()!=3)
  {
    cout<<"参数个数错误!"<<__FILE__<<__LINE__<<endl;
    return false;
  }
  if(m_pMemMgr->addTableSpace(t_paramList[0].c_str(),t_paramList[1].c_str(),
      (T_TABLETYPE)atoi(t_paramList[2].c_str()))==false)
  {
    t_bRet = false;
  }
  
  return t_bRet;
}
bool TestApp::dropIndex(const char * param)
{
  bool t_bRet=true;
  PString t_param;
  vector<PString> t_paramList;
  t_param = param;
  t_param.splitTrim(',',t_paramList);
  if(t_paramList.size()!=2)
  {
    cout<<"参数个数错误!"<<__FILE__<<__LINE__<<endl;
    return false;
  }
  if(m_pMemMgr->dropIndex(t_paramList[0].c_str(),t_paramList[1].c_str())==false)
  {
    t_bRet = false;
  }
  
  return t_bRet;
}

bool TestApp::createTable()
{
  try
  {
  if(m_pMemMgr==NULL) return false;
  TableDef  t_tableDef;
  sprintf(t_tableDef.m_tableName,"%s","user_bill");
  t_tableDef.m_tableType = DATA_TABLE;
  t_tableDef.m_columnNum = 3;
  sprintf(t_tableDef.m_columnList[0].m_name,"%s","userid");
  t_tableDef.m_columnList[0].m_type=VAR_TYPE_VSTR;
  t_tableDef.m_columnList[0].m_len = 16;
  
  sprintf(t_tableDef.m_columnList[1].m_name,"%s","billid");
  t_tableDef.m_columnList[1].m_type=VAR_TYPE_INT;
  t_tableDef.m_columnList[1].m_len = 4;

  sprintf(t_tableDef.m_columnList[2].m_name,"%s","billvalue");
  t_tableDef.m_columnList[2].m_type=VAR_TYPE_LONG;
  t_tableDef.m_columnList[2].m_len = 8;
  t_tableDef.m_spaceNum=0;
  t_tableDef.m_keyFlag=1;
  t_tableDef.m_keyClumnNum=2;
  sprintf(t_tableDef.m_keyColumnList[0],"%s","userid");
  sprintf(t_tableDef.m_keyColumnList[1],"%s","billid");
  t_tableDef.m_lockType=1;
  t_tableDef.m_logType=1;
  TableDesc *t_ptableDesc;
  if(m_pMemMgr->createTable(t_tableDef,t_ptableDesc)==false)
  {
    cout<<"m_pMemMgr->createTable false"<<__FILE__<<__LINE__<<endl;
    return false;
  }
  m_pTableDesc=t_ptableDesc;
  }catch(Mdb_Exception &e)
  {
    throw e;
  }
  return true;
}
bool TestApp::createIndex()
{
  try
  {
  if(m_pMemMgr==NULL) return false;
  IndexDef  t_indexDef;
  sprintf(t_indexDef.m_indexName,"%s","idx_userBill_userid");
  t_indexDef.m_indexType = HASH_INDEX;
  t_indexDef.m_hashSize  = 100000;
  sprintf(t_indexDef.m_tableName,"%s","user_bill");
  t_indexDef.m_columnNum = 1;
  sprintf(t_indexDef.m_columnList[0],"%s","userid");

  t_indexDef.m_spaceNum=0;
  t_indexDef.m_isUnique=0;
  
  IndexDesc *t_pIndexDesc;
  if(m_pMemMgr->createIndex(t_indexDef,t_pIndexDesc)==false)
  {
    cout<<"m_pMemMgr->createIndex false"<<__FILE__<<__LINE__<<endl;
    return false;
  }
  m_pIndexDesc = t_pIndexDesc;
  }catch (Mdb_Exception e)
  {
    throw e;
  }
  return true;
}


bool TestApp::newTbMem(const char * param)
{
  try
  {
  size_t newNum = atol(param);
  static long t_userId=1000000000000000;
  static int  t_billid=0;
  static long t_value=0;
  char   t_strUserId[16+1];
  if(m_pMemMgr==NULL) return false;
  t_billid  = 0;
  t_value   = 0;
  if(m_pTableDesc==NULL)
  {
    m_pMemMgr->getTableDescByName("user_bill",m_pTableDesc);
  }
  vector<MdbAddress> t_tbmemList;
  if(m_pMemMgr->allocateTableMem(m_pTableDesc,newNum,t_tbmemList)==false)
  {
    cout<<"m_pMemMgr->allocateTableMem false"<<__FILE__<<__LINE__<<endl;
    return false;
  }
  for(vector<MdbAddress>::iterator t_itr=t_tbmemList.begin();
      t_itr!=t_tbmemList.end();t_itr++)
  {
    m_tbmemList.push_back(*t_itr);
    t_userId++;
    t_billid++;
    t_value++;
    sprintf(t_strUserId,"%ld",t_userId);
    cout<<"{t_strUserId="<<t_strUserId<<" "
        <<"t_billid="<<t_billid<<" "
        <<"t_value="<<t_value;
    m_pMemMgr->memcopy(t_itr->m_addr+m_pTableDesc->m_tableDef.m_columnList[0].m_offSet,
                       t_strUserId,16);
    m_pMemMgr->memcopy(t_itr->m_addr+m_pTableDesc->m_tableDef.m_columnList[1].m_offSet,
                       &t_billid,sizeof(int));
    m_pMemMgr->memcopy(t_itr->m_addr+m_pTableDesc->m_tableDef.m_columnList[2].m_offSet,
                       &t_value,sizeof(long));
    cout<<"}"<<endl;                     
  }
  }catch(Mdb_Exception &e)
  {
    throw e;
  }
  return true;
}
bool TestApp::newIndexMem(const char *param)
{
  try
  {
  size_t newNum = atol(param);
  
  if(m_pMemMgr==NULL) return false;
  if(m_pIndexDesc==NULL)
  {
    m_pMemMgr->getIndexDescByName("idx_userBill_userid",m_pIndexDesc);
  }
  vector<MdbAddress> t_idxmemList;
  if(m_pMemMgr->allocateIdxMem(*m_pIndexDesc,newNum,t_idxmemList)==false)
  {
    cout<<"m_pMemMgr->allocateTableMem false"<<__FILE__<<__LINE__<<endl;
    return false;
  }
  for(vector<MdbAddress>::iterator t_itr=t_idxmemList.begin();
      t_itr!=t_idxmemList.end();t_itr++)
  {
    m_idxmemList.push_back(*t_itr);
  }
  }
  catch(Mdb_Exception &e)
  {
    throw e;
  }
  return true;
  
}
bool TestApp::freeTbMem(const char *param)
{
  try
  {
  if(m_pMemMgr==NULL) return false;
  if(m_pTableDesc==NULL)
  {
    m_pMemMgr->getTableDescByName("user_bill",m_pTableDesc);
  }
  vector<MdbAddress> t_tbmemList;
  MdbAddress  t_addr;
  PString t_param;
  vector<PString> t_paramList;
  t_param = param;
  t_param.splitTrim(',',t_paramList);
  if(t_paramList.size()%2!=0)
  {
    cout<<"参数个数错误!"<<__FILE__<<__LINE__<<endl;
    return false;
  }
  bool t_flag;
  for(vector<PString>::iterator t_strItr=t_paramList.begin();
      t_strItr!=t_paramList.end();t_strItr=t_strItr+2)
  {
    t_addr.m_pos.setSpaceCode(atoi(t_strItr->c_str()));
    t_addr.m_pos.setOffSet(atol((t_strItr+1)->c_str()));
    t_flag=false;
    for(vector<MdbAddress>::iterator t_memItr=m_tbmemList.begin();
        t_memItr!=m_tbmemList.end();t_memItr++)
    {
      if(t_memItr->m_pos == t_addr.m_pos)
      {
        m_tbmemList.erase(t_memItr);t_flag=true;
        break;
      }
    }
    if(t_flag==false)
    {
      cout<<"该记录不在该描述符内: "<<t_addr.m_pos<<endl;
    }else
    {
      t_tbmemList.push_back(t_addr);
    }
  }
  if(m_pMemMgr->freeTableMem(m_pTableDesc,t_tbmemList)==false)
  {
    cout<<"m_pMemMgr->freeTableMem false"<<__FILE__<<__LINE__<<endl;
    return false;
  } 
  }catch(Mdb_Exception &e)
  {
    throw e;
  }
  return true;
}
bool TestApp::freeIdxMem(const char *param)
{
  try
  {
  bool t_flag;
  if(m_pMemMgr==NULL) return false;
  if(m_pIndexDesc==NULL)
  {
    m_pMemMgr->getIndexDescByName("idx_userBill_userid",m_pIndexDesc);
  }
  vector<ShmPosition> t_idxmemList;
  ShmPosition t_shmPos;
  PString t_param;
  vector<PString> t_paramList;
  t_param = param;
  t_param.splitTrim(',',t_paramList);
  if(t_paramList.size()%2!=0)
  {
    cout<<"参数个数错误!"<<__FILE__<<__LINE__<<endl;
    return false;
  }
  for(vector<PString>::iterator t_strItr=t_paramList.begin();
      t_strItr!=t_paramList.end();t_strItr=t_strItr+2)
  {
    t_shmPos.setSpaceCode(atoi(t_strItr->c_str()));
    t_shmPos.setOffSet(atol((t_strItr+1)->c_str()));
    t_flag = false;
    for(vector<MdbAddress>::iterator t_memItr=m_idxmemList.begin();
        t_memItr!=m_idxmemList.end();t_memItr++)
    {
      if(t_memItr->m_pos == t_shmPos)
      {
        m_idxmemList.erase(t_memItr);
        t_flag = true;
        break;
      }
    }
    
    if(t_flag==false)
    {
      cout<<"该记录不在该描述符内: "<<t_shmPos<<endl;
    }else
    {
      t_idxmemList.push_back(t_shmPos);
    }
  }
  if(m_pMemMgr->freeIdxMem(*m_pIndexDesc,t_idxmemList)==false)
  {
    cout<<"m_pMemMgr->freeIdxMem false"<<__FILE__<<__LINE__<<endl;
    return false;
  } 
  }catch(Mdb_Exception &e)
  {
    throw e;
  }
  return true;
}
bool TestApp::printMemInfo()
{
  int i=0;
  cout<<"---------TbMemList----------------"<<endl;
  for(vector<MdbAddress>::iterator t_tbMItr=m_tbmemList.begin();
      t_tbMItr!=m_tbmemList.end();t_tbMItr++)
  {
    cout<<t_tbMItr->m_pos<<" ";i++;
    if(i>=5)
    {
      cout<<endl;
      i=0;
    }
  }
  cout<<endl;i=0;
  cout<<"----------------------------------"<<endl;
  cout<<"---------TbMemList----------------"<<endl;
  for(vector<MdbAddress>::iterator t_idxMItr=m_idxmemList.begin();
      t_idxMItr!=m_idxmemList.end();t_idxMItr++)
  {
    cout<<t_idxMItr->m_pos<<" ";i++;
    if(i>=5)
    {
      cout<<endl;
      i=0;
    }
  }
  cout<<endl;i=0;
  cout<<"----------------------------------"<<endl;
  return true;
}

bool TestApp::addGparam(const char *param)
{
  try
  {
  if(m_pMemMgr==NULL) return false;
  PString t_param;
  vector<PString> t_paramList;
  t_param = param;
  t_param.splitTrim(',',t_paramList);
  if(t_paramList.size()!=2)
  {
    cout<<"参数个数错误!"<<__FILE__<<__LINE__<<endl;
    return false;
  }
  GlobalParam t_gParam;
  strcpy(t_gParam.m_paramName,t_paramList[0].c_str());
  strcpy(t_gParam.m_value,t_paramList[1].c_str());
  if(m_pMemMgr->addGlobalParam(t_gParam)==false)
  {
    cout<<"addGlobalParam() failed!"<<endl;
  }
  }catch(Mdb_Exception &e)
  {
    throw e;
  }
  return true;
}
bool TestApp::updateGparam(const char *param)
{
  try
  {
  if(m_pMemMgr==NULL) return false;
  PString t_param;
  vector<PString> t_paramList;
  t_param = param;
  t_param.splitTrim(',',t_paramList);
  if(t_paramList.size()!=2)
  {
    cout<<"参数个数错误!"<<__FILE__<<__LINE__<<endl;
    return false;
  }
  if(m_pMemMgr->updateGlobalParam(t_paramList[0].c_str(),t_paramList[1].c_str())==false)
  {
    cout<<"updateGlobalParam() failed!"<<endl;
  }
  }catch(Mdb_Exception &e)
  {
    throw e;
  }
  return true;
}
//性能测试
bool TestApp::cap_test()
{
  try
  {
  size_t newNum = 1;
  
  if(m_pMemMgr==NULL) return false;
  if(m_pTableDesc==NULL)
  {
    m_pMemMgr->getTableDescByName("user_bill",m_pTableDesc);
  }
  vector<MdbAddress> t_tbmemList;
  //1.页内分配效率
  long i;
  time_t time1,time2;
  cout<<"性能测试1开始......."<<endl;
  time(&time1);
  #ifdef FP_DEBUG
      FP_BEGIN(new_del_10000000)
  #endif
  long t_tryNum=10000000;
  for(i=0;i<t_tryNum;i++)
  {
    if(m_pMemMgr->allocateTableMem(m_pTableDesc,newNum,t_tbmemList)==false)
    {
      cout<<"m_pMemMgr->allocateTableMem false"<<__FILE__<<__LINE__<<endl;
      return false;
    }
    if(m_pMemMgr->freeTableMor<MdbAddress> t_tbmemList;
  //1.椤靛唴鍒嗛厤鏁堢巼
  long i;
  time_t time1,time2;
  cout<<"鎬ц兘娴嬭瘯1寮
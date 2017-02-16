/**
*   表空间管理类.
*   提供表空间的维护操作:新增、删除等
*   @author 高将飞 <gaojf@lianchuang.com>
*   @version 1.0
*   @date 2008-04-14
*/
#ifndef _TestApp_H_INCLUDE_200080414_
#define _TestApp_H_INCLUDE_200080414_
#include "appfrm/Application.h"
#include "MdbConstDef.h"
#include "MdbAddress.h"
enum CommandType
{
  COMMND_CREATE_SPACE = 0,
  COMMND_DELETE_SPACE    ,
  COMMND_CREATE_MDB      ,
  COMMND_STARTE_MDB      ,
  COMMND_OPEN_MDB        ,
  COMMND_CLOSE_MDB       ,
  COMMND_STOP_MDB        ,
  COMMND_GETSPACEINFOS   ,
  COMMND_GETSPACEINFO    ,
  COMMND_GETTABLEDESCLIST,
  COMMND_GETTABLEDEFLIST ,
  COMMND_GETTABLEDESC    ,
  COMMND_DROPTABLE       ,
  COMMND_DROPINDEX       ,
  COMMND_GETINDEXBYNAME  ,
  COMMND_GETINDEXSBYTABLE,
  COMMND_DUMPSPACEINFO   ,
  COMMND_CREATETABLE     ,
  COMMND_CREATEINDEX     ,
  COMMND_NEWTBMEM        ,
  COMMND_NEWINDEXMEM     ,
  COMMND_PRINTMEM        ,
  COMMND_TRUNCATETB      ,
  COMMND_TRUNCATEIDX     ,
  COMMND_FREETBMEM       ,
  COMMND_FREEIDXMEM      ,
  COMMND_GETTBINFO       ,
  COMMND_GETSPACEBYTABLE ,
  COMMND_ADDTABLESPACE   ,
  COMMND_GETSESSIONINFOS ,
  COMMND_REGSESSION      ,
  COMMND_UNREGSESSION    ,
  COMMND_CLEARSESSIONS   ,
  COMMND_ADDGPARAM       ,
  COMMND_GETGPARAM       ,
  COMMND_GETGPARAMLIST   ,
  COMMND_UPDATEGPARAM    ,
  COMMND_DELGPARAM       ,
  COMMND_CAPTEST         ,
  COMMND_GETRECLIST      ,
  COMMND_UNKNOW
};

const char COMMAND_LIST[][32+1] = {
  "CreateSpace ",
  "DeleteSpace ",
  "CreateMdb "  ,
  "StartMdb "   ,
  "ConnectMdb ",
  "DisconnectMdb",
  "StopMdb "     ,
  "GetSpaceInfos",
  "GetSpaceInfo ",
  "GetTableDescList",
  "GetTableDefList",
  "GetTabledesc ",
  "DropTable ",
  "DropIndex ",
  "GetIndexByName ",
  "GetIndexByTable ",
  "DumpSpaceInfo",
  "CreateTable"  ,
  "CreateIndex"  ,
  "NewTbMem ",
  "NewIdxMem ",
  "PrintMem",
  "TruncateTb ",
  "TruncateIdx ",
  "FreeTbMem ",
  "FreeIdxMem ",
  "GetTbMemInfo ",
  "GetSpaceByTable ",
  "AddTableSpace ",
  "GetSessionInfos",
  "RegSession",
  "UnregSession ",
  "ClearSessions",
  "AddGparam ",
  "GetGparam ",
  "GetGparamList",
  "UpdateGparam ",
  "DeleteGparam ",
  "CapTest",
  "GetRecList"
};


class MemManager;
class TableDesc;
class IndexDesc;
class TestApp : public Application
{
  public:
    TestApp();
    ~TestApp();
    
  protected:
    virtual bool initialization();
    virtual bool beforeLoop();
    virtual bool recovery();
    virtual bool loopProcess();
    virtual bool end();


  protected:
    bool dealCommand();
    void help();
    bool parseArgs(char *args);
    bool processCommand();
  private:
    bool getSysParams();
  private:
    CommandType  m_commandType;
    char        *m_pCommandParam;
    MemManager  *m_pMemMgr;
    
  private:
    bool dumpSpaceInfo(char * r_fileName);    
  private:
    bool createSpace(const char * param);
    bool addTableSpace(const char * param);
    bool createTable();
    bool createIndex();
    bool dropIndex(const char * param);
    bool newTbMem(const char * param);
    bool newIndexMem(const char *param);
    bool printMemInfo();
    bool freeTbMem(const char *param);
    bool freeIdxMem(const char *param);
    bool addGparam(const char *param);
    bool updateGparam(const char *param);
    bool getRecList();
    bool cap_test();
  private:
    T_NAMEDEF m_dbName;
  private:
    vector<MdbAddress> m_tbmemList; //表申请的内存
    vector<MdbAddress> m_idxmemList;//索引申请的内存
    TableDesc         *m_pTableDesc;
    IndexDesc         *m_pIndexDesc;
};



#endif //_TestApp_H_INCLUDE_200080414_

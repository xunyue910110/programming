#ifndef _CONTROLMEMMGR_H_INCLUDE_20080422_
#define _CONTROLMEMMGR_H_INCLUDE_20080422_
#include "SpaceMgrBase.h"
#include "Mdb_Config.h"
#include "CtlElementTmpt.h"
#include "TableDescript.h"

class ControlMemMgr: public SpaceMgrBase
{
  public:
    ControlMemMgr();
    virtual ~ControlMemMgr();
  public:    
    void initialize(MdbCtlInfo &r_ctlInfo);
    /**
     *createControlSpace 创建控制区表空间.
     *@param   r_flag : 0 第一次创建,1 从文件导入信息
     *@return  true 创建成功,false 失败
     */
    bool createControlSpace(const int &r_flag);
    
    //初始化控制区信息
    bool initCtlDataInfo(const int &r_flag);
    bool deleteControlSpace();//删除表空间
    unsigned int getSpaceCode();//获取下一个表空间代码
    int  getSessionId(); //获取SessionId
    bool realseSid(const int &r_sid);//释放SessionId
    bool reInitSessionInfos(); //重新初始化Session列表 add by gaojf 2009-3-2 3:54
    bool addSpaceInfo(const SpaceInfo &r_spaceInfo);
    bool getSpaceInfo(const char * r_spaceName,SpaceInfo &r_spaceInfo);
    bool getSpaceInfos(vector<SpaceInfo> &r_spaceInfo);
  public:
    bool attach_init(); ///<attach方式初始化
  public:
    bool registerSession(SessionInfo &r_sessionInfo);
    bool unRegisterSession(const SessionInfo &r_sessionInfo);
    bool getSessionInfos(vector<SessionInfo> &r_sessionInfoList);
  public:
    //只负责创建表结构：无对应的索引
    bool addTableDesc(const TableDesc &r_tableDesc,TableDesc* &r_pTableDesc);
    //删除表描述符时：前提，表必须已经truncate，且对应的索引也全部删除
    bool deleteTableDesc(const TableDesc &r_tableDesc);
    bool getTableDescList(vector<TableDesc> &r_tableDescList);
    bool getTableDescByName(const char *r_tableName,TableDesc* &r_pTableDesc);
  public:
    bool addIndexDesc(const IndexDesc &r_indexDesc,IndexDesc* &r_pIndexDesc);
    bool deleteIndexDesc(const IndexDesc &r_indexDesc);
    bool getIndexDescByName(const char *r_indexName,IndexDesc* &r_pIndexDesc);
    void getIndexDescByPos(const size_t &r_offSet,IndexDesc* &r_pIndexDesc);
  public:
    bool addGlobalParam(const GlobalParam &r_gParam);
    bool getGlobalParam(const char *r_paramname,GlobalParam &r_gParam );
    bool getGlobalParams(vector<GlobalParam>& r_gparamList);
    bool updateGlobalParam(const char *r_paramname,const char *r_value);
    bool deleteGlobalParam(const char *r_paramname);
  public:
    MDbGlobalInfo * getMdbGInfo()
    {
      return m_pGobalInfos;
    }
    void updateTbDescTime();
    void updateIndexTime();
    void updateSpTime();
    void updateDbTime();
    size_t getOffset(const char *r_addr)
    {
      return r_addr-getSpaceAddr();
    }
  protected:
    //第一次创建控制区
    bool firstCreator();
    //校验大小是否足够
    bool checkSpaceSize();
    //attach 后 init 
    bool initAfterAttach();
  private:
    MdbCtlInfo  *m_pCtlInfo;
  private: //该段定义控制区结构构成
    MDbGlobalInfo              *m_pGobalInfos;    ///<全局信息
    ListManager<SpaceInfo>      m_spInfoMgr;      ///<表空间信息
    ListManager<GlobalParam>    m_gParamMgr;      ///<全局参数
    ListManager<TableDesc>      m_tbDescMgr;      ///<表描述符
    ListManager<IndexDesc>      m_idxDescMgr;     ///<索引描述符
    ListManager<SessionInfo>    m_sessionInfoMgr; ///<Session信息
  public:
    //调试接口
    virtual bool dumpSpaceInfo(ostream &r_os);
};
#endif //_CONTROLMEMMGR_H_INCLUDE_20080422_

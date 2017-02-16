#ifndef _MDB_CONFIG_H_INCLUDE_20080414_
#define _MDB_CONFIG_H_INCLUDE_20080414_
#include "TableDefParam.h"
#include "SpaceInfo.h"

class SysParam;
class MdbCtlInfo
{
  public:
    T_NAMEDEF       m_dbName;          ///<数据库名
    T_NAMEDEF       m_ctlSpaceName;    ///<控制区表空间名
    T_FILENAMEDEF   m_ctlDiskFile;     ///<控制区表空间对应文件
    size_t          m_size;            ///<控制区表空间大小
    size_t          m_pageSize;        ///<页面大小(1M)
    int             m_gparamMaxSpNum;  ///<支持的允许的表空间数量
    int             m_gparamMaxNum;    ///<支持的全局参数个数(最大个数)
    int             m_tableMaxNum;     ///<支持的表最大个数
    int             m_indexMaxNum;     ///<支持的索引最大个数
    int             m_sessionMaxNum;   ///<支持的Session最大个数
    T_NAMEDEF       m_idxHashSpaceName;///<索引表空间（存放hash部分）
    T_FILENAMEDEF   m_logPath;         ///<日志路径
    T_FILENAMEDEF   m_lockfile;        ///<信号量控制文件
  public:
    bool getCtlInfo(const char * r_dbName,SysParam* pSysParam);
    friend ostream& operator<<(ostream& os,const MdbCtlInfo& r_obj);
};

class Mdb_Config
{
  public:
    MdbCtlInfo            m_ctlInfo;
    //表空间定义信息（包括索引表空间定义信息和数据表空间定义信息）
    vector<SpaceInfo>     m_spaceInfoList;
  public:
    //根据数据库名从配置文件中取配置信息
    bool getConfigInfo(const char * r_dbName);
  private:
    bool getConfigInfoLocal(const char * r_dbName,SysParam* r_pSysParam);
    bool check();
};

#endif //_MDB_CONFIG_H_INCLUDE_20080414_


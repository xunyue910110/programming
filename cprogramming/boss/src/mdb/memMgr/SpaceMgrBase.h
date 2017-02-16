#ifndef _SPACEMGRBASE_MANAGER_H_20080415_
#define _SPACEMGRBASE_MANAGER_H_20080415_
#include <sys/shm.h>
#include "MdbConstDef.h"
#include "SpaceFile.h"
#include "SpaceInfo.h"
#include "DoubleLock.h"
#include "Mdb_Exception.h"

class SpaceMgrBase
{
  protected:
    //以下信息在表空间创建之前即定
    SpaceInfo    m_spaceHeader;
    SpaceFile    m_file;

    //表空间创建之后信息
    SpaceInfo    *m_pSpHeader;  ///<表空间头
  public:
    SpaceMgrBase()
    {
      m_pdbLock=NULL;
    }
    virtual ~SpaceMgrBase()
    {
      if(m_spaceHeader.m_shmAddr!=NULL)
      {
        shmdt(m_spaceHeader.m_shmAddr);
        m_spaceHeader.m_shmAddr=NULL;
      }
      if(m_pdbLock!=NULL)
      {
        delete m_pdbLock;
        m_pdbLock = NULL;
      }
    }
  public:
    bool dumpDataIntoFile();
    bool loadDataFromFile();
  public:
    //调试接口
    virtual bool dumpSpaceInfo(ostream &r_os);
    // 内存使用率 add by chenm
    // 默认返回所有表空间使用率
    virtual float getTableSpaceUsedPercent(){return 0;}
  public:
    /**
     *initialize 初始化.
     *@param   r_SpaceInfo:  表空间定义参数
     *@return  true 成功,false 失败
     */
    bool  initialize(const SpaceInfo  &r_SpaceInfo);
    bool  attach();
    bool  detach();
    SpaceInfo *getSpaceInfo();
    unsigned int getSpaceCode()
    {
      return m_spaceHeader.m_spaceCode;
    }
    char * getSpaceAddr()
    {
      return m_spaceHeader.m_shmAddr;
    }
    char * getSpaceName()
    {
      return m_spaceHeader.m_spaceName;
    }
  public:
    /**
     *createSpace 创建表空间.
     *@param   r_SpaceInfo:  表空间定义参数
     *@param   r_shmKey    :  表空间的ShmKey
     *@param   r_shmId     :  表空间ShmId
     *@return  true 创建成功,false 失败
     */
    bool createSpace(SpaceInfo  &r_SpaceInfo,const int &r_flag);
    static bool deleteSpace(SpaceInfo  &r_SpaceInfo);
    /**
     *deleteSpace 删除表空间.
     *@param   r_shmId :  表空间ShmId
     *@return  true 成功,false 失败
     */
    static bool deleteSpace(const int &r_shmId);

    /**
     *attachSpace 连接表空间
     *@param   r_shmKey:  表空间ShmKey
     *@param   r_size  :  表空间大小
     *@param   r_shmId :  表空间ShmId
     *@param   r_pShm  :  连接后的地址
     *@return  true 成功,false 失败
     */
    static bool attachSpace(const key_t &r_shmKey,const size_t &r_size,
                                  int &r_shmId,char* &r_pShm);

    /**
     *attachSpace 连接表空间
     *@param   r_shmId :  表空间ShmId
     *@param   r_pShm  :  连接后的地址
     *@return  true 成功,false 失败
     */
    static bool attachSpace(const int &r_shmId,char * &r_pShm);
    
    /**
     *detachSpace 连接表空间
     *@param   r_pShm  :  地址
     *@return  true 成功,false 失败
     */
    static bool detachSpace(char * &r_pShm);
  protected:
    DoubleLock  *m_pdbLock;
};


#endif //_SPACEMGRBASE_MANAGER_H_20080415_

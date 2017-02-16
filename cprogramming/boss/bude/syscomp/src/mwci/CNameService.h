//! 名字服务
/*!
用来提供统一的名字服务调用
包括
	- 本地名字服务,从本地XML文件中查找
	- 远程名字服务,从服务器中获取
	- 增量同步功能
	- 全量同步功能
	- 批量更新远程
当本地查找失败后会自动从远端查找
同步功能只有在外界调用是才会执行
,不会自己进行

    	该类不可构造,使用GetInstanct()静态函数获取实力地址,
    	每个进程只有一个实例
*/


#ifndef CNAMESERVICE_H_HEADER_INCLUDED_BE5857CC
#define CNAMESERVICE_H_HEADER_INCLUDED_BE5857CC

#include "mwci/CContext.h"
#include "mwci/CNaming.h"
#include "mwci/CLocalNameServiceImpl.h"
#include "mwci/CRemoteNameServiceImpl.h"
#include "mwci/CDBNameServiceImpl.h"

//##ModelId=4191C8EB028E
class CNameService
{
  public:

    ///工具函数用来判断两个时间的先后
    //##ModelId=41C77DBE0185
	static bool IsTMBiger(const STD tm&,const STD tm&);

    ///批量更新远段(一般在数据库服务器上)
    /**
    	\param vtheNamings 新的名字信息
    	\param theCaller  调用类实例
    	\return 操作成功返回true,否则返回false
    */
    //##ModelId=4191CB2D023F
    bool BatChgNaming(const vector<CNaming>& vtheNamings,IMWCaller* theCaller);

    ///增量同步(一般在数据库服务器上)
    /**
	从远程获取更新时间大于本地时间的Naming数据,更新到本地内存和XML文件中
    	\param theCaller 调用类实例
    	\return 操作成功返回true,否则返回false
    */
    //##ModelId=4191CC51031B
    bool IncreSyn(IMWCaller* theCaller);

    ///全量同步(一般在数据库服务器上)
    /**
    从远程获取更新时间大于本地时间的Naming数据,更新到本地内存和XML文件中
    	\param theCaller 调用类实例
    	\return 操作成功返回true,否则返回false
    */
    //##ModelId=4191CD2A0114
    bool FullSyn(IMWCaller* theCaller);

///不知道干什么用的
    //##ModelId=4191E02C023E
    string GetService();

    ///获取远端名字服务器的连接地址
    /**
    	\return 远端名字服务器的连接地址(IP:PORT)
    */
    //##ModelId=4191E0480338
    string GetServerInfo();

    ///获取CNameService实例
    /**
    	该类不可构造,每个进程只有一个实例
      \return CNameService实例指针
    */
    //##ModelId=4192099301B2
    static CNameService* GetInstance();

    ///设置Context 信息
    /**
      设置<LOCALCONTEXTINFO>段的内容
    	\param cContext 取得的数据
    	\return 操作成功返回true,否则返回false
    */
    //##ModelId=419307CD0241
    bool SetContext(CContext cContext);

    ///设置SetLogicalName
    /**
	设置的逻辑名
    	\param strLogicalName 逻辑名
    	\return 操作成功返回true,否则返回false
    */
    //##ModelId=419307D50184
    bool SetLogicalName(const string & strLogicalName);

    ///更据逻辑名在查找Naming
    /**
	使用逻辑名进行查找
    	\param cContext 本地的Context Info,关键是其中的contextid
    	\param strLogicalName 逻辑服务名
	\param theCaller 调用类实例
	\param NeedRemote 是否需要进行远程名字解析(进行远程名字解析需要连接远端服务器，只有在本地名字查找失败之后才进行)
    	\param iIndex 下标
    	\return 取得的数据
    */
    //##ModelId=4194AA7B015E
    CNaming LookUpNaming(CContext cContext, string strLogicalName,IMWCaller* theCaller,bool NeedRemote=true,int iIdx=0);

    ///暂时没有任何作用
    //##ModelId=4194AA7B0172
    bool ReLoadNaming();

    ///删除符合条件的全部本地Naming
    /**
    	\param cNaming 要删除的Naming,只会去判断其LogicalName是否相同
     	\return 操作成功返回true,否则返回false
    */
    //##ModelId=4194AA7B0190
    bool ErasureNaming(CNaming cNaming);

    ///提交指定的Naming
    /**
	提交可以理解成更新( Update )
    	\param cNaming 要提交的Naming,只会去判断其LogicalName是否相同
    	\return 操作成功返回true,否则返回false
    */
    //##ModelId=4194AA7B01A4
    bool SubmitNaming(CNaming cNaming);

    ///列出远端全部的Naming
    /**
       \param theCaller 调用类实例
    	\param vtheNaming 用于存放Naming的vector
    	\return 操作成功返回true,否则返回false
    */
    //##ModelId=4194AA7B01C2
    bool ListNaming(vector<CNaming>& theNamingS,IMWCaller* theCaller);

    ///查找远端更新过的Naming
    /**
	\param cContext 本地Context Info
	\param tmUptTime 本地的名字服务的更新时间
    	\param vtheNaming 用于存放Naming的vector
       \param theCaller 调用类实例
    	\return 操作成功返回true,否则返回false
    */
    //##ModelId=4194AA7B01E0
    bool FindUptNaming(CContext cContext,STD tm tmUptTime, vector<CNaming>& theNamingS,IMWCaller* theCaller);

    ///没有任何作用
    //##ModelId=4194AA7B024F
    bool AddNaming(CNaming cNaming);

    ///装载一个本地Naming 的XML文件到内存中
    /**
	\param strFileName 带完整路径的XML文件名
    	\return 操作成功返回true,否则返回false
    */
    //##ModelId=4194AA7B026D
    bool SetCfgFile(string strFileName);

    ///根据逻辑名在查找Naming
    /**
	使用逻辑名进行查找，自动从本地XML文件中获取Context信息
    	\param strLogicalName 逻辑服务名
	\param theCaller 调用类实例
	\param NeedRemote 是否需要进行远程名字解析(进行远程名字解析需要连接远端服务器，只有在本地名字查找失败之后才进行)
    	\param iIndex 下标
    	\return 取得的数据
    */
    //##ModelId=4194AD0202D1
    CNaming LookUpNaming(string strLogicalName,IMWCaller* theCaller,bool NeedRemote=true,int iIdx=0);

    ///根据逻辑名在查找Naming
    /**
	使用逻辑名进行查找，自动从本地XML文件中获取Context信息
    	\param strLogicalName 逻辑服务名
	\param pcon 数据库类实例
    	\param iIndex 下标
    	\return 取得的数据
    */
    //##ModelId=4194AD0202D1
    CNaming LookUpNaming(string strLogicalName);

    ///查找远端更新过的Naming
    /**
	自动从本地XML文件中获取Context信息
	\param tmUptTime 本地的名字服务的更新时间
    	\param vtheNaming 用于存放Naming的vector
       \param theCaller 调用类实例
    	\return 操作成功返回true,否则返回false
    */
    //##ModelId=4195656200AC
    bool FindUptNaming(STD tm tmUptTime, vector<CNaming>& theNamingS,IMWCaller* theCaller);

    //##ModelId=41956562019D
   //CNaming LookUpNaming(int iIdx=0);

    ///设置名字服务是否需要从本地查找
    /**
	自动从本地XML文件中获取Context信息
	\param tmUptTime 本地的名字服务的更新时间
    	\param vtheNaming 用于存放Naming的vector
       \param theCaller 调用类实例
    	\return 操作成功返回true,否则返回false
    */
    //#
    //##ModelId=419566C00382
    void SetIfLookUpLocal(bool);

    ///解析
    //##ModelId=41BE9C1B006B
    virtual ~CNameService();

    ///本地名字服务实例
    //##ModelId=4191C93601D8
    CLocalNameServiceImpl* m_LocalService;

    ///远程名字服务实例
    //##ModelId=4191C93A0043
    CRemoteNameServiceImpl* m_RemoteService;
    CDBNameServiceImpl* m_DBService;
  protected:


  private:

    ///构造
    /**
	\sa GetInstance()
    */
	//##ModelId=41BE68FD0391
    CNameService();

     ///设置远端名字服务器地址信息
     /**
     		自动从本地文件获取
     */
    //##ModelId=41BEA8400350
    bool SetNamingSvcInfo();

    ///Context 信息
    //##ModelId=4195656902A1
    CContext m_cContext;

    ///逻辑名
    //##ModelId=41956575033F
    string m_strLogicalName;

    /// Naming 信息
    //##ModelId=4195657001F7
    CNaming m_cNaming;

    /// 是否需查找本地Naming
    //##ModelId=419566C002D6
    bool m_bIfLookUpLocal;

     ///存放该类的唯一实例地址
    //##ModelId=419566C002B1
    static CNameService *m_cNameService;

    ///是否已经知道了远程名字服务信息
    //##ModelId=41BEA9850267
    bool m_bKnowNamingSvc;

    ///是否已经知道了本地Context 信息
    //##ModelId=41BEA9A3015C
    bool m_bKnowContext;

};



#endif /* CNAMESERVICE_H_HEADER_INCLUDED_BE5857CC */

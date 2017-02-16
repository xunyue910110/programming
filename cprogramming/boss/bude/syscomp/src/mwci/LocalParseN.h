#ifndef LOCALPARSEN_H_HEADER_INCLUDED_BE4AE18E
#define LOCALPARSEN_H_HEADER_INCLUDED_BE4AE18E
#include "mwci/CLNSInfo.h"
#include "mwci/CLNSNaming.h"
#include "mwci/CNaming.h"

class CNaming;
class CContext;

// LocalNameDocumentParse
//##ModelId=41B50F6D024E
class LocalParseN
{
  public:
    /**
	  \param theLocCI 取得的返回值
	  \return
	- 0 - successful
    - 0ther - error or no data found
	*/
    //##ModelId=41B50F7F036C
    int GetLocalCI(CContext& theLocCI);

    /**
	  \param theRemoteNS 取得的返回值
	  \return
	- 0 - successful
    - 0ther - error or no data found
	*/
    //##ModelId=41B50F7F0394
    int GetRemoteNS(CNaming& theRemoteNS);

	///构造并载入XML文件
    //##ModelId=41B50F7F03B2
    LocalParseN(const char *filename);

	///构造
    //##ModelId=41B6A61D00D8
    LocalParseN();

	///是否成功载入XML文件
    //##ModelId=41B50F7F03DA
    bool IsReady() const;

	///获取XML文件名

    //##ModelId=41B50F800006
    string GetFileName() const;

    //##ModelId=41B50F80001A
    virtual ~LocalParseN();

    ///保存内存中的数据到硬盘
	/**
	  \return
		- 0 - successful
		- 0ther - error or no data found
	*/
    //##ModelId=41B50F80002E
    int SaveToFile();

    /// Set (update) 一个Naming
	/**
	  \param LogicName 逻辑名
	  \param theNaming Naming
	  \param iIndex 下标
	  \return
		- 0 - successful
		- 0ther - error or no data found
	*/
    //##ModelId=41B50F800042
    int SetNameSvc(const string& LogicName, CNaming& theNaming, int iIndex = 0);

    /// 获取 一个Naming
	/**
	  \param LogicName 逻辑名
	  \param theNaming Naming
	  \param iIndex 下标
	  \return
		- 0 - successful
		- 0ther - error or no data found
	*/
    //##ModelId=41B50F80007F
    int GetNameSvc(const string& LogicName, CNaming& theNaming, int iIndex = 0);

    /// 设置 context 信息
	/**
	  \param theLocCI context 信息
	  \return
		- 0 - successful
		- 0ther - error or no data found
	*/
    //##ModelId=41B50F8000A7
    int SetLocalCI(const CContext& theLocCI);

    /// 设置 名字服务器 信息
	/**
	  \param theLocCI 名字服务器 信息
	  \return
		- 0 - successful
		- 0ther - error or no data found
	*/
    //##ModelId=41B50F8000ED
    int SetRemoteNS(CNaming& theRemoteNS);

    /// 添加一个 Naming 信息
	/**
	  如果LogicName 和 theNaming.LogicName 不一致,按LogicName为准
	  \param LogicName 逻辑名
	  \param theNaming Naming 信息
	  \return
		- 0 - successful
		- 0ther - error or no data found
	*/
    //##ModelId=41B50F80011F
    int AddNameSvc(string LogicName, CNaming& theNaming);

    /// 删除一个 Naming 信息
	/**
	  \param LogicName 逻辑名
	  \param iIndex 下标
		- -1 - Delete all Naming with the logic_name
	  \return
		- 0 - successful
		- 0ther - error or no data found
	*/
    //##ModelId=41B50F800147
    int DelNameSvc(string LogicName,
        int iIndex = -1);

    /// 列出本地全部Naming
	/**
	  \param vtheNaming 本地全部Naming
	  \return
		- 0 - successful
		- 0ther - error or no data found
	*/
    //##ModelId=41B6AC2A0072
    int ListNaming(vector<CNaming> &vtheNaming);

	/// 删除本地全部Naming
	/**
		只删除内存中的数据
		\return
		- 0 - successful
		- 0ther - error or no data found
	*/
    //##ModelId=41BEA4E9029D
    int DeleteAllNaming();


  protected:

	///暂时没用
    //##ModelId=41B51F8700F7
    int fnChkFile();

	///检查Naming 是否正确(是否超长)
    //##ModelId=41B6A61D0100
    int fnChkNaming(CNaming&);

    ///检查Context 是否正确(是否超长)
	//##ModelId=41B6A61D0197
    int fnChkContext(const CContext&);

	///解析XML文件,获取 theName 标签的下一个数据
	/**
		\param	theAll		要处理的全部XML文件映射
		\param	theName		要获取的XML标签
		\param	theValue	获取出来的数据
		\param	offset		位置偏移
		\return
			- true 成功
			- false 失败
	*/
    //##ModelId=41C77ED1025D
    bool getNextValue(const string& theAll,const string& theName,string& theValue,int& offset);

	///解析XML文件,获取 theName 标签的数据
	/**
		\param	theAll		要处理的全部XML文件映射
		\param	theName		要获取的XML标签
		\param	theValue	获取出来的数据
		\return
			- true 成功
			- false 失败
	*/
    //##ModelId=41C77ED103B2
    bool getValue(const string& theAll,const string& theName,string& theValue);

	///解析XML文件,获取 LocalContextInfo 标签的数据
	/**
		\param	theLocalContextInfo		要处理的XML文件映射
		\param	theValue	获取出来的数据
		\return
			- true 成功
			- false 失败
	*/
    //##ModelId=41C77ED200D8
    bool getLocalContextInfo(const string& theLocalContextInfo,CLNSInfo& theValue);

	///解析XML文件,获取 RemoteNamingSrv 标签的数据
	/**
		\param	theRemoteNamingSrv		要处理的XML文件映射
		\param	theValue	获取出来的数据
		\return
			- true 成功
			- false 失败
	*/
    //##ModelId=41C77ED20196
    bool getRemoteNamingSrv(const string& theRemoteNamingSrv,CLNSInfo& theValue);

    ///解析XML文件,获取全部 Naming 标签的数据
	/**
		\param	theNaming	要处理的XML文件映射
		\param	theValue	获取出来的数据
		\return
			- true 成功
			- false 失败
	*/
	//##ModelId=41C77ED2024B
    bool getNaming(const string& theNaming,CLNSNaming& theValue);

  private:
    ///标记是否可以正常使用
	//##ModelId=41B5118F03BC
    bool m_bReady;

	///好像没有多大用处哦
    //##ModelId=41B51190009C
    FILE *m_fp;

	///XML文件名
    //##ModelId=41B511900114
    string m_sFileName;

	///内存中的数据是否比文件中的新
    //##ModelId=41B5119001BE
    bool m_bIsUpdated;

	///本地Context Info 和远端Naming Server的地址信息
    //##ModelId=41B511A70207
    CLNSInfo m_NSInfo;

	///存放实体
    //##ModelId=41B511C2009E
    std::list<CLNSNaming> m_listNaming;
    std::list<CLNSNaming>::iterator itr,theEnd;


};



#endif /* LOCALPARSEN_H_HEADER_INCLUDED_BE4AE18E */

//! 本地名字服务实现
/*!
本地XML文件格式如下
<LOCALNAMESERVICE>
	<LOCALCONTEXTINFO>
		<CONTEXTID>1234</CONTEXTID>
		<EPARCHYCODE>38</EPARCHYCODE>
		<CITYCODE>381</CITYCODE>
		<ORIGINCODE>381</ORIGINCODE>
		<NAMINGUPTTIME>20041220164815</NAMINGUPTTIME>
	</LOCALCONTEXTINFO>
	<REMOTENAMINGSRV>
		<IP>192.168.8.11</IP>
		<PORT>4000</PORT>
		<MACHINENAME>MACHINAME1</MACHINENAME>
	</REMOTENAMINGSRV>
	<NAMING>
		<LOGICALNAME>look at me33</LOGICALNAME>
		<MACHINENAME>Sun220</MACHINENAME>
		<IP>192.168.8.11</IP>
		<PORT>4000</PORT>
		<PHYSICALNAME>LIST</PHYSICALNAME>
		<TYPECODE>1</TYPECODE>
	</NAMING>
	<NAMING>
		<LOGICALNAME>look at me1</LOGICALNAME>
		<MACHINENAME>YuWen123</MACHINENAME>
		<IP>192.168.8.11</IP>
		<PORT>4000</PORT>
		<PHYSICALNAME>GET_UPDATED</PHYSICALNAME>
		<TYPECODE>3</TYPECODE>
	</NAMING>
</LOCALNAMESERVICE>

*/

#ifndef CLOCALNAMESERVICEIMPL_H_HEADER_INCLUDED_BE586D2C
#define CLOCALNAMESERVICEIMPL_H_HEADER_INCLUDED_BE586D2C
#include "mwci/CContext.h"
#include "mwci/CNaming.h"
#include "mwci/LocalParseN.h"


//##ModelId=41906CF1025E
class CLocalNameServiceImpl
{
  public:

    ///删除指定的Naming
    /**
    	\param cNaming 要删除的Naming,只会去判断其LogicalName是否相同
    	\param iIndex 下标,默认删除符合条件的全部Naming
    	\return 操作成功返回true,否则返回false
    */
    //##ModelId=41907A1D03C5
    bool ErasureNaming(CNaming cNaming, int iIndex = -1);

    ///提交指定的Naming
    /**
	提交可以理解成更新( Update )
    	\param cNaming 要提交的Naming,只会去判断其LogicalName是否相同
    	\param iIndex 下标,默认符合条件的第一个
    	\return 操作成功返回true,否则返回false
    */
    //##ModelId=41907A6D0009
    bool SubmitNaming(CNaming cNaming, int iIndex = 0);

    ///列出全部的Naming
    /**
    	\param vtheNaming 用于存放Naming的vector
    	\return 操作成功返回true,否则返回false
    */
    //##ModelId=41908148011B
    bool ListNaming(vector<CNaming> &vtheNaming);

    ///添加一个Naming
    /**
    	\param cNaming 要添加的Naming
    	\return 操作成功返回true,否则返回false
    */
    //##ModelId=41918C6700A6
    bool AddNaming(CNaming cNaming);

    ///加载一个Xml文件
    /**
    	\param strFileName Xml文件的完整路径
    	\return 操作成功返回true,否则返回false
    */
    //##ModelId=4191BB8701F7
    bool SetCfgFile(string strFileName);

    ///获取Context 信息
    /**
       获取<LOCALCONTEXTINFO>段的内容
    	\param theContext 取得的数据
    	\return 操作成功返回true,否则返回false
    */
    //##ModelId=4194AEBC0381
    bool GetContext(CContext& theContext);

    ///设置Context 信息
    /**
      设置<LOCALCONTEXTINFO>段的内容
    	\param theContext 取得的数据
    	\return 操作成功返回true,否则返回false
    */
    //##ModelId=41C77DBE00C7
    bool SetContext(const CContext& theContext);

    ///获取Naming的更新时间
    /**
    	\param theTime 取得的数据
    	\return 操作成功返回true,否则返回false
    */
    //##ModelId=4194AF1603E4
    bool GetUptTime(STD tm &theTime);

    ///更据逻辑名在本地查找Naming
    /**
    	\param theNaming 取得的数据
    	\param strLogicalName 逻辑名
    	\param iIndex 下标
    	\return 操作成功返回true,否则返回false
    */
    //##ModelId=419567C90089
    bool LookUpNaming(CNaming& theNaming, string & strLogicalName, int iIndex = 0);

    ///更据逻辑名在本地查找Naming
    /**
	使用事先设置的逻辑名进行查找
    	\param theNaming 取得的数据
    	\param iIndex 下标
    	\return 操作成功返回true,否则返回false
    	\sa SetLogicalName()
    */
    //##ModelId=419567C9010B
    bool LookUpNaming(CNaming& theNaming, int iIndex = 0);

    ///设置SetLogicalName
    /**
	设置的逻辑名
    	\param strLogicalName 逻辑名
    	\return 操作成功返回true,否则返回false
    	\sa LookUpNaming(CNaming& theNaming, int iIndex = 0)
    */
    //##ModelId=419567C90133
    bool SetLogicalName(string strLogicalName);

    ///获取远端名字服务器的连接地址
    /**
    	\return 远端名字服务器的连接地址(IP:PORT)
    */
    //##ModelId=4195695C039F
    string GetNamingSrvInfo();

    /// 暂时无用
    //##ModelId=41957F180364
    bool IsNeedLocalService();

    ///构造
    //##ModelId=41B6AF0D0042
    CLocalNameServiceImpl();

    ///解析
    //##ModelId=41B6AF0D0074
    virtual ~CLocalNameServiceImpl();

    ///把内存中的名字信息写回Xml文件
    //##ModelId=41BEA5CA0124
    void Save();

    ///清空内存中全部名字服务信息
    //##ModelId=41BFAF9C01ED
    void EmptyNaming();

  private:
    //!存放本地XML文件路径和名称
    //##ModelId=4195680802E2
    string m_strCfgFileName;

    //!本地XML文件解析类
    //##ModelId=419568470044
    LocalParseN* m_pLocalNaming;

    //!存放当前状态
    //##ModelId=41B6A9D903A4
    int m_iState;

    //!存放当前逻辑名
    //##ModelId=41B6F4DD0195
    string m_LogicName;

};



#endif /* CLOCALNAMESERVICEIMPL_H_HEADER_INCLUDED_BE586D2C */

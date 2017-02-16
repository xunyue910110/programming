//! 远程名字服务实现
/*!
 远程名字服务,从服务器中获取Naming信息
*/
#ifndef CREMOTENAMESERVICEIMPL_H_HEADER_INCLUDED_BE586DB5
#define CREMOTENAMESERVICEIMPL_H_HEADER_INCLUDED_BE586DB5
#include "mwci/CContext.h"
#include "mwci/CNaming.h"
#include "mwci/IMWCaller.h"

// 远端名字服务实现
//##ModelId=41906D0A0065
class CRemoteNameServiceImpl
{
  public:

    ///批量更新远段(一般在数据库服务器上)
    /**
    	\param vtheNamings 新的名字信息
    	\param theCaller  调用类实例
    	\return 操作成功返回true,否则返回false
    */
    //##ModelId=419184A10308
    bool BatChgNaming(const vector<CNaming>& vtheNamings,IMWCaller *theCaller);

    ///列出远端全部的Naming
    /**
       \param theCaller 调用类实例
    	\param vtheList 用于存放Naming的vector
    	\return 操作成功返回true,否则返回false
    */
    //##ModelId=4194AAC803C2
    bool ListNaming(vector<CNaming>& vtheList,IMWCaller *theCaller);

    ///查找远端更新过的Naming
    /**
	\param lContextid 本地Context ID
	\param tmUptTime 本地的名字服务的更新时间
    	\param theNaming 用于存放Naming的vector
       \param theCaller 调用类实例
    	\return 操作成功返回true,否则返回false
    */
    //##ModelId=4194AAC803CC
    bool FindUptNaming(long  lContextid, const STD tm& tmUptTime, vector<CNaming>& theNaming,IMWCaller *theCaller);

     ///设置远端名字服务器地址信息
     /**
     		\param SrvInfo 名字服务器的IP地址和段口号
     */
    //##ModelId=41BD39D3035C
    bool SetNamingSrvInfo(string SrvInfo);

    ///根据逻辑名在查找Naming
    /**
	使用逻辑名进行查找，自动从本地XML文件中获取Context信息
	\param lContextid  本地Context ID
    	\param strLogicalName 逻辑服务名
	\param theNaming 取得的数据
	\param theCaller 调用类实例
    	\param iIndex 下标
    	\return 取得的数据
    */
    //##ModelId=4194ADA2038F
    bool LookUpNaming(long  lContextid, string strLogicalName, CNaming& theNaming,IMWCaller* theCaller,int iIdx=0);

    ///工具函数用来将字符串转换成时间
    //##ModelId=41BD5A950228
    static bool charTOtm(const char *theString, STD tm &theTime);

    ///工具函数用来将时间转换成字符串
    //##ModelId=41BD91510321
    static bool tmTOchar(const STD tm &theTime, char *theString);

    ///列出远端全部的Naming
    /**
	\param lContextID Context ID
    	\param vtheList 用于存放Naming的vector
       \param theCaller 调用类实例
    	\return 操作成功返回true,否则返回false
    */
     //##ModelId=41BED44200ED
    bool ListNaming(long lContextID, vector<CNaming>& vtheList,IMWCaller *theCaller);

  private:

    ///远端名字服务器地址信息
    //##ModelId=41BD3B8103AA
    string m_ConnectString;


};



#endif /* CREMOTENAMESERVICEIMPL_H_HEADER_INCLUDED_BE586DB5 */

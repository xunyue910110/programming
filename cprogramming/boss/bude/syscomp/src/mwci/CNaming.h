//! 名字存放实体
/*!
用来存放一个Naming的信息
*/

#ifndef CNAMING_H_HEADER_INCLUDED_BE586DE9
#define CNAMING_H_HEADER_INCLUDED_BE586DE9

#include <string>
#include "mwci/CContext.h"
// 名字信息
//##ModelId=4190734D0378
class CNaming
{
  public:
    ///Context 信息
    //##ModelId=4190771D01CC
    CContext m_cContext;

    ///逻辑服务名
    //##ModelId=419179E6007D
    string m_strLogicName;

    ///服务器IP地址
    //##ModelId=41907377026A
    string m_strMachineIP;

    ///服务器段口号
    //##ModelId=419073770274
    string m_strMachinePort;

    ///服务器名称
    //##ModelId=419073770288
    string m_strMachineName;

    ///服务物理名
    //##ModelId=419073770292
    string m_strPhysicalName;

    ///服务更新时间
    //##ModelId=41907DA701FC
    STD tm m_tmReDeployTime;

    /// 服务类型
    /**
	- '0' - 其他服务(不会使用本地服务缓存)
       - '1' - 优先从本地缓存读取的BOSS内部服务
    	- '2' - 不使用本地缓存的BOSS内部服务
     	- '3' - 优先从本地缓存读取的BOSS外部服务
     	- '4' - 不使用本地缓存的BOSS外部服务
    */
    //##ModelId=419089F40236
    string m_cSvcTypeCode;

};



#endif /* CNAMING_H_HEADER_INCLUDED_BE586DE9 */

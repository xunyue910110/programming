//! 名字服务内部存放的数据实体
/*!
 有关长度的定义在MWGlobal.h 中
 用于在内存中存放
 <LOCALCONTEXTINFO> 段
 <REMOTENAMINGSRV> 段
 实际上有用的字段只有
 ContextID
 NamingUptTime
 IP
 PORT
*/

#ifndef CLNSINFO_H_HEADER_INCLUDED_BE4A9207
#define CLNSINFO_H_HEADER_INCLUDED_BE4A9207
# include "mwci/MWGlobal.h"
// C_LocalNamingService_Info
//##ModelId=41B50CBE02DC
class CLNSInfo
{
  public:
    //!CONTEXT ID
    //##ModelId=41B50DA6018B
    int ContextID;

    //!地洲编码
    //##ModelId=41B50DD101C9
    char EparchyCode[LNS_CODE_LENGTH+1];

    //!市县编码
    //##ModelId=41B50DD102B0
    char CityCode[LNS_CODE_LENGTH+1];


    //!来源编码
    //##ModelId=41B50DD1035A
    char OriginCode[LNS_CODE_LENGTH+1];

    //!本地名字更新时间
    //##ModelId=41B50DD20026
    STD tm NamingUptTime;

    //!是否需要本地服务
    //##ModelId=41B50DD200C6
    bool NeedLocSvc;

    //!名字服务的机器名称
    //##ModelId=41B50E380037
    char MachineName[LNS_NAME_LENGTH+1];

    //!名字服务的机器IP
    //##ModelId=41B50E380019
    char IP[LNS_IP_LENGTH+1];

    //!名字服务的机器段口号
    //##ModelId=41B50E380069
    char PORT[LNS_PORT_LENGTH+1];

};



#endif /* CLNSINFO_H_HEADER_INCLUDED_BE4A9207 */

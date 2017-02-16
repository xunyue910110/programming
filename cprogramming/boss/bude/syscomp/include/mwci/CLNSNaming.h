//! 名字服务内部存放的数据实体
/*!
 有关长度的定义在MWGlobal.h 中
 用于在内存中存放
<NAMING> 段
 实际上有用的字段只有

*/

#ifndef CLNSNAMING_H_HEADER_INCLUDED_BE4ABB26
#define CLNSNAMING_H_HEADER_INCLUDED_BE4ABB26
# include "mwci/MWGlobal.h"
//##ModelId=41B50C5C00F1
class CLNSNaming
{
  public:

       //!比较两个Naming是否相等,只比较LogicName
	//##ModelId=41B51B0201F7
	friend inline bool operator==(const CLNSNaming &oa, const CLNSNaming &ob)
	{return strcmp(oa.LogicName,ob.LogicName)==0;}

	//!比较两个Naming是否相等,只比较LogicName
	 //##ModelId=41B6A61C01C7
	friend inline bool operator==(const CLNSNaming &oa, const char * ob)
	{return strcmp(oa.LogicName,ob)==0;}

	//!比较两个Naming是否相等,只比较LogicName
       //##ModelId=41B6A61C029A
	friend inline bool operator==(const CLNSNaming &oa, const string &ob)
	{return strcmp(oa.LogicName,ob.c_str())==0;}

    //!服务逻辑名,可能存放的是[逻辑名]+[交易编码]
    //##ModelId=41B6A61C007D
    char LogicName[LNS_NAME_LENGTH+1];

    //!机器名称
    //##ModelId=41B50CE5029C
    char MachineName[LNS_NAME_LENGTH+1];

    //!服务IP地址
    //##ModelId=41B50CED01C1
    char IP[LNS_IP_LENGTH+1];

    //!服务段口号
    //##ModelId=41B50CF10362
    char PORT[LNS_PORT_LENGTH+1];

    //!服务物理名
    //##ModelId=41B50CF6016A
    char PhyName[LNS_NAME_LENGTH+1];

    //!服务类型
    //##ModelId=41B50D0100EE
    char TypeCode[2];

};



#endif /* CLNSNAMING_H_HEADER_INCLUDED_BE4ABB26 */

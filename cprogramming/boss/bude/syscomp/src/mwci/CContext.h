#ifndef CCONTEXT_H_HEADER_INCLUDED_BE587109
#define CCONTEXT_H_HEADER_INCLUDED_BE587109
#include "mwci/MWGlobal.h"
// 上下文
//##ModelId=41907341033E
class CContext
{
  public:

    //!ContextID
    //##ModelId=4190738C0274
    int m_nContextID;

    //!地洲编码
    //##ModelId=41907D2F007C
    string m_strEparchyCode;

    //!来源编码
    //##ModelId=41907D3800DA
    string m_strOriginCode;

    //!市县编码
    //##ModelId=41917E2C0102
    string m_strCityCode;

    //!本地名字更新时间
    //##ModelId=41B00E4A01BA
	STD tm m_tmNamingUptTime ;

     //!暂时没有使用
    //##ModelId=41B00E4A0232
	bool m_bNeedLocSvc ;
};



#endif /* CCONTEXT_H_HEADER_INCLUDED_BE587109 */

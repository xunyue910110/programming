/*
* Copyright (c) 2005, 联创科技
* All rights reserved.
* 
* $Header: /cvsroot/BSS4.2/BSS4.2/billing/bude/syscomp/src/flowline/engine/src/flowline/adapter/ResourceProcessorAdapter.h,v 1.1 2011/04/15 07:39:45 sunam Exp $
* $Log: ResourceProcessorAdapter.h,v $
* Revision 1.1  2011/04/15 07:39:45  sunam
* *** empty log message ***
*
* Revision 1.1  2011/03/24 12:01:04  hanyu
* *** empty log message ***
*
* Revision 1.1  2009/09/08 15:57:15  shenglj
* *** empty log message ***
*
* Revision 1.1  2006/01/19 06:37:26  kongdb
* new flowline from yuaneg
*
* Revision 1.1  2005/05/08 11:57:50  luzl
* *** empty log message ***
*
* Revision 1.2  2005/03/14 12:53:37  luzl
* *** empty log message ***
*
* Revision 1.1  2005/03/07 05:58:13  luzl
* 改名dpp为flowline
*
* Revision 1.1  2005/03/04 10:24:27  luzl
* init
* 
*/
#ifndef _RESOURCE_PROCESSOR_ADAPTER_H_
#define _RESOURCE_PROCESSOR_ADAPTER_H_
#include "config-all.h"
#include "Adapter.h"
#include "flowline/Resource.h"
#include <string>
#include <vector>

namespace flowline {
class ResourceProcessor;
namespace adapter {
/** 外部动作调用句柄 */
class ResourceProcessorAdapter : public ElementAdapter {
public:
    /**
     * 执行一个动作
     * @param ctx 流程的上下文
     * @exception execution_error 执行期间出现异常
     */
    virtual void process( Context & ctx, PResource input, vector<PResource>& output ) = 0;
    virtual void finish( Context& ctx, vector<PResource>& output ) = 0;
    PResource createResource( Context& ctx, const string& repositoryName = "" );
};

}
}
#endif //_RESOURCE_PROCESSOR_ADAPTER_H_
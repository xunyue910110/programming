/*
* Copyright (c) 2005, 联创科技
* All rights reserved.
* 
* $Header: /cvsroot/BSS4.2/BSS4.2/billing/bude/syscomp/src/flowline/engine/src/flowline/adapter/ResourceRepositoryAdapter.h,v 1.1 2011/04/15 07:39:46 sunam Exp $
* $Log: ResourceRepositoryAdapter.h,v $
* Revision 1.1  2011/04/15 07:39:46  sunam
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
* Revision 1.4  2005/04/27 14:59:28  luzl
* *** empty log message ***
*
* Revision 1.3  2005/03/23 06:15:26  luzl
* *** empty log message ***
*
* Revision 1.2  2005/03/14 12:53:37  luzl
* *** empty log message ***
*
* Revision 1.1  2005/03/07 05:58:14  luzl
* 改名dpp为flowline
*
* Revision 1.1  2005/03/04 10:24:27  luzl
* init
* 
*/
#ifndef _RESOURCE_REPOSITORY_ADPATER_H_
#define _RESOURCE_REPOSITORY_ADPATER_H_
#include "config-all.h"
#include "Adapter.h"
#include "flowline/Resource.h"
#include "flowline/Metadata.h"
#include <vector>
USING_NAMESPACE(std);

namespace flowline{
	
namespace adapter{
	using namespace flowline;
	
    class ResourceRepositoryAdapter : public ElementAdapter {
    public:
    	/**列出本周期的资源*/
    	virtual void list(Context& ctx, vector<PResource>& result) = 0;
    	/**但资源被成功处理后, 需要做的事情*/
        virtual void afterTargetCommitted(Context& ctx, PResource source) = 0;
    	
    	//输出
    	virtual PResource create(Context& ctx) = 0;
        virtual void prepareCommit(Context& ctx, PResource target) = 0;
        virtual void commit(Context& ctx, PResource target) = 0;
        virtual void rollback(Context& ctx, PResource target) = 0;
    	
    	//通过名称, 恢复出对象
    	virtual PResource recover(const string& resourceName) = 0;
    	
    	//PDataSet createDataSet();
    	
    	const Metadata* getMetadata();
    };	  
}
}    
#endif //_RESOURCE_REPOSITORY_ADPATER_H_

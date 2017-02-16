/*
* Copyright (c) 2005, 联创科技
* All rights reserved.
* 
* $Header: /cvsroot/BSS4.2/BSS4.2/billing/bude/syscomp/src/flowline/engine/src/flowline/ResourceRepository.h,v 1.1 2011/04/15 07:39:45 sunam Exp $
* $Log: ResourceRepository.h,v $
* Revision 1.1  2011/04/15 07:39:45  sunam
* *** empty log message ***
*
* Revision 1.1  2011/03/24 12:01:03  hanyu
* *** empty log message ***
*
* Revision 1.1  2009/09/08 15:57:15  shenglj
* *** empty log message ***
*
* Revision 1.1  2006/01/19 06:37:26  kongdb
* new flowline from yuaneg
*
* Revision 1.1  2005/05/08 11:57:48  luzl
* *** empty log message ***
*
* Revision 1.5  2005/04/27 14:59:30  luzl
* *** empty log message ***
*
* Revision 1.4  2005/03/23 06:15:05  luzl
* *** empty log message ***
*
* Revision 1.3  2005/03/14 12:55:33  luzl
* 分离只读只写
*
* Revision 1.2  2005/03/14 12:46:15  juyf
* *** empty log message ***
*
* Revision 1.1  2005/03/07 05:58:12  luzl
* 改名dpp为flowline
*
* Revision 1.1  2005/03/04 10:24:26  luzl
* init
* 
*/
#ifndef _RESOURCE_REPOSITORY_H_
#define _RESOURCE_REPOSITORY_H_
#include "flowline/Node.h"
#include "flowline/Adaptable.h"
#include "flowline/Resource.h"
namespace flowline {
namespace adapter {
class ResourceRepositoryAdapter;
class InputRepositoryAdapter;
class OutputRepositoryAdapter;
}

class ResourceRepository : public Storage, public Adaptable {
public:
	ResourceRepository();
	//
	virtual void doEnter( Context & ctx, Node * from );
	
    virtual PResource pull();

    /** 对端被提交之后 */
    virtual void afterTargetCommitted( Context & ctx );

    /** 对端被回滚之后 */
    virtual void afterTargetRollbacked( Context & ctx );
    
//    /**
//     * 取得正在处理的源
//     */ 
//    void getProcessingResources(vector< PResource>& sources);

	/////////////////////

    /** 产生一个资源 */
    virtual PResource create( Context & ctx );

    /** 准备提交 */
    virtual void prepareCommit( Context & ctx, PResource target );

    /** 提交生成的资源 */
    virtual void commit( Context & ctx, PResource target );

    /** 回滚掉目标资源 */
    virtual void rollback( Context & ctx, PResource target );

	/////////////////
	
    
 private:
	//20060116change -Z
 	//ResourceRepositoryAdapter* ResourceRepository::getRepositoryAdapter();
 	ResourceRepositoryAdapter* getRepositoryAdapter();
 		
 	
    vector < PResource > m_resources;

    /** 指示当前在m_resources中处理到的位置 */
    size_t m_processIndex;
    
    
};

}

#endif //_RESOURCE_REPOSITORY_H_

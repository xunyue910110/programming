/*
* Copyright (c) 2005, 联创科技
* All rights reserved.
* 
* $Header: /cvsroot/BSS4.2/BSS4.2/billing/bude/syscomp/src/flowline/engine/src/flowline/ResourceRepository.cpp,v 1.1 2011/04/15 07:39:42 sunam Exp $
* $Log: ResourceRepository.cpp,v $
* Revision 1.1  2011/04/15 07:39:42  sunam
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
* Revision 1.3  2005/03/14 12:55:46  luzl
* *** empty log message ***
*
* Revision 1.2  2005/03/09 03:17:12  juyf
* *** empty log message ***
*
* Revision 1.1  2005/03/07 05:58:12  luzl
* 改名dpp为flowline
*
* Revision 1.1  2005/03/04 10:24:25  luzl
* init
* 
*/

#include "ResourceRepository.h"
#include "flowline/adapter/ResourceRepositoryAdapter.h"
#include "flowline/Task.h"
#include "flowline/ResourceProcessor.h"

namespace flowline {
using namespace flowline::adapter;

ResourceRepository::ResourceRepository() : m_processIndex(0) {
}


void ResourceRepository::doEnter( Context & ctx, Node * from ) {
      m_resources.clear();//zzc  ---处理完资源列表后释放
      getRepositoryAdapter() ->list( ctx, m_resources );
      FL_TRACE_P1("获取%d个资源", m_resources.size());
      for(int i=0; i<m_resources.size();i++) {
      	m_resources[i]->setRepository(this);
      }  
        	
    if ( getSubsequentNodeCount() ) {
        getSubsequentNode()->enter( ctx, this );
    }
//   m_resources.clear();//zzc  ---处理完资源列表后释放
}


PResource ResourceRepository::pull() {
    if ( m_processIndex >= m_resources.size() ) {
        bool finished = true;
        vector<Node * >::iterator it = m_previousNodes.begin();
        for (; it != m_previousNodes.end(); it++ ) {
            if ( !( *it ) ->isAllFinished() ) {
                finished = false;
                break;
            }
        }
        if ( finished ) {
        	  FL_TRACE("本次批处理完成");
        	  setAllFinished( true );
        } else {
        	  FL_TRACE("暂时没有资源可处理");
        }

        return PResource();
    }
    PResource& r = m_resources[m_processIndex++];
    FL_TRACE_P1("资源%s被取走", r->getName().c_str());
    return r;
}



/** 对端被提交之后 */
void ResourceRepository::afterTargetCommitted( Context & ctx ) {
    for ( size_t i = 0; i < m_processIndex; i++ ) {
        getRepositoryAdapter() ->afterTargetCommitted( ctx, m_resources[ i ] );
    }
    m_resources.erase( m_resources.begin(), m_resources.begin() + m_processIndex );
    m_processIndex = 0;
}

/** 对端被回滚之后 */
void ResourceRepository::afterTargetRollbacked( Context & ctx ) {
    m_processIndex = 0;
}

/** 产生一个资源 */
PResource ResourceRepository::create( Context & ctx ) {
    PResource res = getRepositoryAdapter() ->create( ctx );
    res->setRepository(this);
    return res;
}
/** 准备提交 */
void ResourceRepository::prepareCommit( Context & ctx, PResource target ) { //TODO
    getRepositoryAdapter() ->prepareCommit( ctx, target );
}

/** 提交生成的资源 */
void ResourceRepository::commit( Context & ctx, PResource target ) {
    getRepositoryAdapter() ->commit( ctx, target );
}


/** 回滚掉目标资源 */
void ResourceRepository::rollback( Context & ctx, PResource target ) {
    getRepositoryAdapter() ->rollback( ctx, target );
}

ResourceRepositoryAdapter* ResourceRepository::getRepositoryAdapter() {
	return dynamic_cast<ResourceRepositoryAdapter*>(getAdapter());
}

};

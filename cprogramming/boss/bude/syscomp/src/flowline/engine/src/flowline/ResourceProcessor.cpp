/*
* Copyright (c) 2005, 联创科技
* All rights reserved.
* 
* $Header: /cvsroot/BSS4.2/BSS4.2/billing/bude/syscomp/src/flowline/engine/src/flowline/ResourceProcessor.cpp,v 1.1 2011/04/15 07:39:42 sunam Exp $
* $Log: ResourceProcessor.cpp,v $
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
* Revision 1.4  2005/03/23 06:11:03  luzl
* *** empty log message ***
*
* Revision 1.2  2005/03/14 12:55:46  luzl
* *** empty log message ***
*
* Revision 1.1  2005/03/07 05:58:12  luzl
* 改名dpp为flowline
*
* Revision 1.1  2005/03/04 10:24:25  luzl
* init
* 
*/

#include "ResourceProcessor.h"
#include "ResourceRepository.h"
#include "flowline/adapter/ResourceProcessorAdapter.h"
#include <sys/time.h>
#include "Context.h"
namespace flowline {
using namespace flowline::adapter;

void AbstractResourceProcessor::doInit(FlowLineDao& dao) {}

void AbstractResourceProcessor::doEnter( Context & ctx, Node * from ) {
    ResourceRepository * irr = dynamic_cast < ResourceRepository * > ( from );

    vector < PResource > result;
    set < ResourceRepository * > targetRepositories;

    while ( !isAllFinished() ) {
        FL_TRACE("开始取资源");
        PResource res = irr->pull();
        targetRepositories.clear();
        
        result.clear();
        try {
            if ( !res ) {
                FL_TRACE("资源没取到");
                if ( !irr->isAllFinished() ) {
                    break;
                }
                FL_TRACE("本次批处理结束, 执行finish操作");
                finish( ctx, result );
            } else {
                FL_TRACE_P1("处理资源%s", res->getName().c_str());
                Context ctx2(&ctx);
                ctx2.setAttribute("sourceName", res->getName());
                struct timeval begin, end;
                gettimeofday(&begin, 0);

                process( ctx2, res, result );
                
                gettimeofday(&end, 0);
                if(m_sources.empty())
                    m_begin = begin.tv_sec;
                m_sources.push_back(pair<PResource, int>(res, 1000*(end.tv_sec - begin.tv_sec) + (end.tv_usec - begin.tv_usec)/1000));
                //getSubsequentNodeCount();
            }

            if ( result.size() != 0 ) {
                for ( vector < PResource >::iterator it = result.begin(); it != result.end(); it++ ) {
                    PResource & r = * it;
                    FL_TRACE("do prepareCommit");
                    dynamic_cast<ResourceRepository*>(r->getRepository()) ->prepareCommit( ctx, r );
                }
                ctx.getFlowLineDao().logExecution(ctx.getCycleLogId(), ctx.getBatchLogId(),
                                                  *this, m_sources, result, m_begin, time(0));
                m_sources.clear();

                for ( vector < PResource >::iterator it = result.begin(); it != result.end(); it++ ) {
                    PResource & r = * it;
                    FL_TRACE("do commit");
                    dynamic_cast<ResourceRepository*>(r->getRepository()) ->commit( ctx, r ); //throw()
                    targetRepositories.insert( r->getRepository() );
                }

                FL_TRACE("do afterTargetCommitted");
                irr->afterTargetCommitted( ctx ); //throw()
 	       //ctx.getFlowLineDao().logExecution(ctx.getCycleLogId(), ctx.getBatchLogId(),
                //                                *this, m_sources, result, m_begin, time(0));
                //`m_sources.clear();
	          
 		            //finish( ctx, result );//zzc- add--8-8
            }
        } catch (...) {

            cout<<".........catch............"<<endl;
            if ( result.size() != 0 ) {
                for ( vector < PResource >::iterator it = result.begin(); it != result.end(); it++ ) {
                    PResource & r = * it;
                    FL_TRACE("do rollback");
                    dynamic_cast<ResourceRepository*>(r->getRepository()) ->rollback( ctx, r );
                }


                FL_TRACE("do afterTargetRollback");
                irr->afterTargetRollbacked( ctx ); //throw()
            }

            throw;
        }

        for ( set< ResourceRepository * >::iterator it = targetRepositories.begin(); it != targetRepositories.end(); it++ ) {
                ( * it ) ->enter( ctx, this );
            }
    }
}

void ResourceProcessor::process( Context & ctx, PResource source, vector < PResource > & result ) {
    getResourceProcessorAdapter()->process( ctx, source, result );
}

void ResourceProcessor::finish( Context & ctx, vector < PResource > & result ) {
    getResourceProcessorAdapter()->finish( ctx, result );
    setAllFinished( true );
}

ResourceProcessorAdapter* ResourceProcessor::getResourceProcessorAdapter() {
    return dynamic_cast < ResourceProcessorAdapter * > ( getAdapter() );
}
};

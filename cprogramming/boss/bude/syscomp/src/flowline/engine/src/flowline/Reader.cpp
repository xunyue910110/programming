/*
* Copyright (c) 2005, 联创科技
* All rights reserved.
* 
* $Header: /cvsroot/BSS4.2/BSS4.2/billing/bude/syscomp/src/flowline/engine/src/flowline/Reader.cpp,v 1.1 2011/04/15 07:39:42 sunam Exp $
* $Log: Reader.cpp,v $
* Revision 1.1  2011/04/15 07:39:42  sunam
* *** empty log message ***
*
* Revision 1.1  2011/03/24 12:01:02  hanyu
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
* Revision 1.3  2005/04/27 14:59:30  luzl
* *** empty log message ***
*
* Revision 1.2  2005/03/14 12:55:46  luzl
* *** empty log message ***
*
* Revision 1.1  2005/03/07 05:58:12  luzl
* 改名dpp为flowline
*
* Revision 1.1  2005/03/04 10:24:26  luzl
* init
* 
*/
#include "Reader.h"
#include "Writer.h"
#include "DataBuffer.h"
namespace flowline {

void Reader::initNodes(Node* node) {
	if(node != this)
		m_internalNodes.push_back(node);
		
	Writer* w = dynamic_cast<Writer*>(node);
	if(w) {
	//	m_writers.push_back(w);
	} else {
		vector <Node *>::iterator it = node->getSubsequentNodes().begin();
		for(;it!= node->getSubsequentNodes().end(); it++) {
			initNodes(*it);
		}
	}
}
	
void Reader::doInit(FlowLineDao& dao) {
	AbstractResourceProcessor::doInit(dao);
	initNodes(this);
}
void Reader::onBeginProcessResource(Context & ctx, PResource  resource) {
    for (vector < Node * >::iterator it = m_internalNodes.begin(); it != m_internalNodes.end(); it++) {
        dynamic_cast < DataSupportable * > (* it)->beginProcessResource(ctx, resource);
    }
}
void Reader::process(Context & ctx1, PResource source, vector < PResource > & result) {
    DataProcessContext ctx(&ctx1, result);
  
    beginProcessResource(ctx, source);

    DataBuffer * buffer = dynamic_cast < DataBuffer * > (getSubsequentNode());
    while (true) {
        PDataSet data = source->read(ctx);
        if (!data) {
            setResourceFinished(true);
        	buffer->enter(ctx, this);
        	break;
        } else {
            buffer->push(data);
            
            buffer->enter(ctx, this);
        }
    }

    //vector < Writer * >::iterator it = m_writers.begin();
    //for ( ; it != m_writers.end(); it++) {
    //    (* it)->popResult(result);
    //}

}
void Reader::finish(Context & ctx1, vector < PResource > & result) {
    DataProcessContext ctx(&ctx1, result);
    DataBuffer * buffer = dynamic_cast < DataBuffer * > (getSubsequentNode());
    setAllFinished( true );
    buffer->enter(ctx, this);
    //vector < Writer * >::iterator it = m_writers.begin();
    //for ( ; it != m_writers.end(); it++) {
    //    (* it)->popResult(result);
    //}
}
};

/*
* Copyright (c) 2005, 联创科技
* All rights reserved.
* 
* $Header: /cvsroot/BSS4.2/BSS4.2/billing/bude/syscomp/src/flowline/engine/src/flowline/DataBuffer.cpp,v 1.1 2011/04/15 07:39:40 sunam Exp $
* $Log: DataBuffer.cpp,v $
* Revision 1.1  2011/04/15 07:39:40  sunam
* *** empty log message ***
*
* Revision 1.1  2011/03/24 12:01:01  hanyu
* *** empty log message ***
*
* Revision 1.1  2009/09/08 15:56:36  shenglj
* *** empty log message ***
*
* Revision 1.1  2006/01/19 06:37:26  kongdb
* new flowline from yuaneg
*
* Revision 1.1  2005/05/08 11:57:48  luzl
* *** empty log message ***
*
* Revision 1.3  2005/04/27 14:59:29  luzl
* *** empty log message ***
*
* Revision 1.2  2005/03/14 12:55:46  luzl
* *** empty log message ***
*
* Revision 1.1  2005/03/07 05:58:11  luzl
* 改名dpp为flowline
*
* Revision 1.1  2005/03/04 10:24:26  luzl
* init
* 
*/
#include "DataBuffer.h"
#include "flowline/DataProcessor.h"
#include "flowline/Reader.h"
namespace flowline {
void DataBuffer::doEnter(Context & ctx, Node * from) {
    Executor & next = dynamic_cast < Executor& > (*getSubsequentNode());
    
    if (dynamic_cast < DataSupportable& > (*from).isResourceFinished())  {
    	setResourceFinished(true);
    	if (from->isAllFinished()) 
    		setAllFinished(true);
    }
    next.enter(ctx, this);
}
PDataSet DataBuffer::pull() {
	PDataSet temp = m_data;
	m_data = PDataSet();
	return temp;
}
void DataBuffer::push(PDataSet  data) {
	m_data = data;
}

const Metadata* DataBuffer::forwardRef() {
		vector <Node *> & pnodes = getSubsequentNodes();
    	if(pnodes.size() != 0) {
    		vector <Node *> & snodes = pnodes[0]->getSubsequentNodes();
    		if(snodes.size() != 0) {
    			return dynamic_cast<Storage*>(snodes[0])->getMetadata();
    		}
    	}
    	return 0;
}

const Metadata* DataBuffer::backwardRef() {
	vector <Node *> & pnodes = getPreviousNodes();
    	if(pnodes.size() != 0) {
    		vector <Node *> & snodes = pnodes[0]->getPreviousNodes();
    		if(snodes.size() != 0) {
    			return dynamic_cast<Storage*>(snodes[0])->getMetadata();
    		}
    	}
    	return 0;
}
const Metadata* DataBuffer::getMetadata() {
    const Metadata* m = Storage::getMetadata();
    
    if(m == 0 || m->fields().size() == 0) {
    	vector <Node *> & pnodes = getPreviousNodes();
    	if(pnodes.size() != 0) {
    		if(dynamic_cast<Reader*>(pnodes[0])) {
    			m = backwardRef();
    			if(m == 0 || m->fields().size() == 0)
    				m = forwardRef();
    			return m;
    		}
    	}
    	
    	m = forwardRef();
		  if(m == 0|| m->fields().size() == 0)
				m = backwardRef();		
    }
    return m;
}
};

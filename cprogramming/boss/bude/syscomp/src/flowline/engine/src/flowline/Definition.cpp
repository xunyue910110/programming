/*
* Copyright (c) 2005, 联创科技
* All rights reserved.
* 
* $Header: /cvsroot/BSS4.2/BSS4.2/billing/bude/syscomp/src/flowline/engine/src/flowline/Definition.cpp,v 1.1 2011/04/15 07:39:40 sunam Exp $
* $Log: Definition.cpp,v $
* Revision 1.1  2011/04/15 07:39:40  sunam
* *** empty log message ***
*
* Revision 1.1  2011/03/24 12:01:02  hanyu
* *** empty log message ***
*
* Revision 1.1  2009/09/08 15:56:36  shenglj
* *** empty log message ***
*
* Revision 1.1  2006/01/19 06:37:26  kongdb
* new flowline from yuaneg
*
* Revision 1.2  2005/06/13 08:29:29  luzl
* 修改context错误
*
* Revision 1.1  2005/05/08 11:57:49  luzl
* *** empty log message ***
*
* Revision 1.4  2005/04/27 14:59:28  luzl
* *** empty log message ***
*
* Revision 1.3  2005/03/23 06:15:16  luzl
* *** empty log message ***
*
* Revision 1.2  2005/03/14 12:55:45  luzl
* *** empty log message ***
*
* Revision 1.1  2005/03/07 05:58:10  luzl
* 改名dpp为flowline
*
* Revision 1.1  2005/03/04 10:24:25  luzl
* init
* 
*/

#include "Definition.h"
#include "flowline/Context.h"
#include <unistd.h>
#include <sstream>
namespace flowline {
USING_NAMESPACE(std);	
const size_t DEFAULT_BLOCK_SIZE = 1000;
const size_t DEFAULT_BATCH_CYCLE = 60;
Definition::Definition() : m_validFrom(0), m_validTo(0), m_batchCycle(DEFAULT_BATCH_CYCLE), m_blockSize(DEFAULT_BLOCK_SIZE){}
Definition::~Definition() {
    vector < Node* >::iterator it = m_nodes.begin();
    for ( ; it != m_nodes.end(); it++ ) {
        delete *it;
    }
}
void Definition::init(FlowLineDao& dao) {
    vector < Node* >::iterator it = m_nodes.begin();
    for ( ; it != m_nodes.end(); it++ ) {
        ( *it ) ->init(dao);
        if ( ( *it ) ->getPreviousNodeCount() == 0 ) {
            m_bootNodes.push_back( *it );
        }
        Executor* e = dynamic_cast<Executor*>(*it);
        if(e && e->isDelivery()) {
        	 vector <Node * >& pnodes =  e->getPreviousNodes();
        	 for(int i=0; i<pnodes.size(); i++) {
        	 		//没有前置节点, 认为是引导节点
        	 	  if(pnodes[i]->getPreviousNodeCount() == 0) {
        	 	  	 	m_deliveryBootNodes.push_back(pnodes[i]);        	 	  	 	
        	 	  } else {
        	 	  	  //如果前置节点都是批处理的, 也认为是引导节点
        	 	  		vector <Node * >& ppnodes =  pnodes[i]->getPreviousNodes();
        	 	  		
        	 	  		for(int j=0; j<ppnodes.size(); j++) {
        	 	  			 Executor* pe = dynamic_cast<Executor*>(ppnodes[j]);
        	 	  			 if(!pe->isDelivery()) {
        	 	  			 		m_deliveryBootNodes.push_back(pnodes[i]);        	 	  	 	
        	 	  			 		break;
        	 	  			 	  
        	 	  			  }
        	 	  		}
        	 	  }	
        	 }
        }
    }
}

class DefContext : public Context {
	Definition* m_definition;
	bool m_inDelivery;
public:
	
	DefContext(Context* parent, Definition* def, bool isDelivery) : Context(parent), m_definition(def), m_inDelivery(isDelivery) {
	}
	
	virtual bool canEnter(Node& node) {
		Executor* e = dynamic_cast<Executor*>(&node);
		if(e != 0) {
			return e->isDelivery() == m_inDelivery;
		} else {
			return true;
		}
	}
};

size_t Definition::getBusyExecutors(bool isDelivery) {
	//等待所有生产节点的完成
	vector < Node* >::iterator it = m_nodes.begin();
	size_t count = 0;
    for(; it != m_nodes.end(); it++) {
    	Executor* e = dynamic_cast<Executor*>(*it);
    	if(e != 0) {
    		if(e->isDelivery() == isDelivery) {
    			if(!e->isAllFinished()) {
    				count ++;
    			}
    		}
    	}
    }
    return count;
}

void Definition::execute(Context& ctx, bool isDelivery) {
	vector < Node* >& nodes = isDelivery? m_deliveryBootNodes:m_bootNodes;
	FL_TRACE_P1("boot nodes = %d", nodes.size());
	vector < Node* >::iterator it = nodes.begin();
    for ( ; it != nodes.end(); it++ ) {
        ( * it ) ->enter( ctx, 0 );
    }
    //TODO 有bug, 不过在单线程下, 可以不检查
    //while(getBusyExecutors(isDelivery) != 0) {
	    	//defctx.wait();
	  //  	sleep(1);
	//}
}

bool Definition::checkDeliveryCondition(Context& ctx) {
	vector <Checker* >::iterator it = m_deliveryCheckers.begin();
	for(; it!= m_deliveryCheckers.end(); it++) {
		if(!(*it)->check(ctx))
			return false;
	}
	return true;
}

//0 正常, -1 交付条件不满足, -2 交付条件超时
int Definition::execute( Context & ctx ) {
	DefContext defctx0(&ctx, this, false);
	
	execute(defctx0, false);
	FL_TRACE("after batch");
	if(defctx0.getCycleInstance().getEnd() + getDeliveryDelay() <= time(0)) { 
		cout << "end = " << defctx0.getCycleInstance().getEnd() << endl;
		if(!checkDeliveryCondition(defctx0)) {			
            cout<<"-------checkDelivery default-----"<<endl;
			if(m_conditionTimeOut != 0 && 
			   time(0) - defctx0.getCycleInstance().getEnd() > m_conditionTimeOut) {
			   	//等待条件成立超时
				return -2;
			}
			return -1;
		}
		//执行交付
		FL_TRACE("执行交付");
		DefContext defctx1(&ctx, this, true);
		execute(defctx1, true);
		return 1;		
	}
	return 0;
}

void Definition::reset() {
    vector < Node* >::iterator it = m_nodes.begin();
    for ( ; it != m_nodes.end(); it++ ) {
        ( *it ) ->reset();
    }
}

void Definition::addNode( Node* node ) {
    m_nodes.push_back( node );
}

Node* Definition::getNodeByName( const string& name ) {
    vector < Node* >::iterator it = m_nodes.begin();
    for ( ; it != m_nodes.end(); it++ ) {
        if ( ( *it ) ->getName() == name ) {
            return * it;
        }
    }
    return 0;
}

Node* Definition::getNodeById( int id ) {
    vector < Node* >::iterator it = m_nodes.begin();
    for ( ; it != m_nodes.end(); it++ ) {
        if ( ( *it ) ->getId() == id ) {
            return * it;
        }
    }
    return 0;
}

size_t Definition::getNodeCount() const {
    return m_nodes.size();
}

Node* Definition::getNode( size_t index ) {
    return m_nodes[ index ];
}

void Definition::addDeliveryChecker(Checker* checker) {
	m_deliveryCheckers.push_back(checker);
}
        
//size_t Definition::getDeliveryCheckerCount() const{
//	return m_deliveryCheckers.size();
//}
//
//Delivery* Definition::getDeliveryChecker(size_t index){
//	return m_deliveryCheckers[index];
//}

const Cycle& Definition::getDeliveryCycle() const {
	return m_deliveryCycle;
}
void Definition::setDeliveryCycle(const Cycle& cycle) {
	m_deliveryCycle = cycle;
}

void Definition::setValidFrom(time_t validFrom) {
	m_validFrom = validFrom;
}
void Definition::setValidTo(time_t validTo) {
	m_validTo = validTo;
}
void Definition::setBatchCycle(int batchCycle) {
	if(batchCycle > 0)
		m_batchCycle = batchCycle;
}
void Definition::setConditionTimeOut(int conditionTimeOut) {
	m_conditionTimeOut = conditionTimeOut;	
}
void Definition::setBlockSize(size_t blockSize) {
	if(blockSize > 0)
		m_blockSize = blockSize;
} 
time_t Definition::getValidFrom() const{
	return m_validFrom;
}
time_t Definition::getValidTo()const{
	return m_validTo;
}
int Definition::getBatchCycle() const {
	return m_batchCycle;
}
int Definition::getConditionTimeOut() const{
	return m_conditionTimeOut;
}
size_t Definition::getBlockSize() const {
	return m_blockSize;
}

void Definition::setDeliveryDelay(int delay) {
	m_deliveryDelay = delay;	
}
		
int Definition::getDeliveryDelay() const{
	return m_deliveryDelay;
}

};

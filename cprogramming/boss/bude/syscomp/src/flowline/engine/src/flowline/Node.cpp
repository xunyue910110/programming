/*
* Copyright (c) 2005, 联创科技
* All rights reserved.
* 
* $Header: /cvsroot/BSS4.2/BSS4.2/billing/bude/syscomp/src/flowline/engine/src/flowline/Node.cpp,v 1.1 2011/04/15 07:39:42 sunam Exp $
* $Log: Node.cpp,v $
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
* Revision 1.3  2005/04/27 14:59:29  luzl
* *** empty log message ***
*
* Revision 1.2  2005/03/14 12:55:45  luzl
* *** empty log message ***
*
* Revision 1.1  2005/03/07 05:58:10  luzl
* 改名dpp为flowline
*
* Revision 1.1  2005/03/04 10:24:27  luzl
* init
* 
*/

#include "Node.h"
#include "Context.h"
#include "flowline/FlowLineException.h"
#include "flowline/Adaptable.h"
namespace flowline {

Node::Node() : m_allFinished(false) {}

Node::~Node() {}

void Node::addPreviousNode(Node * node) {
    m_previousNodes.push_back(node);
    node->m_subsequentNodes.push_back(this);
}
/** 取得转移, 如果列出名称, 则必须只有一个转移, 否则异常 */
Node * Node::getPreviousNode(const string& name)  {
    if(name.empty()) {
        if (m_previousNodes.size() != 1) {
            THROW_P2(DefinitionException, "定义错误, 节点'%s'的前置节点数量为%d", 
            	getName().c_str(), m_previousNodes.size()); 
        } else {
            return m_previousNodes[0];
        }
    } else {
        for(size_t i=0; i<m_previousNodes.size(); i++) {
            if(m_previousNodes[i]->getName() == name) {
                return m_previousNodes[i];
            }
        }
        THROW_P2(DefinitionException, "定义错误, 节点'%s'找不到前置节点'%s'", 
           	getName().c_str(), name.c_str()); 

    }
}
Node * Node::getSubsequentNode(const string& name)  {
    if(name.empty()) {
        if (m_subsequentNodes.size() != 1) {
            THROW_P2(DefinitionException, "定义错误, 节点'%s'的后续节点数量为%d", 
            	getName().c_str(), m_subsequentNodes.size()); 
        } else {
            return m_subsequentNodes[0];
        }
    } else {
        for(size_t i=0; i<m_subsequentNodes.size(); i++) {
            if(m_subsequentNodes[i]->getName() == name) {
                return m_subsequentNodes[i];
            }
        }
        THROW_P2(DefinitionException, "定义错误, 节点'%s'找不到后续节点'%s'", 
           	getName().c_str(), name.c_str()); 
    }
}
vector < Node *>& Node::getPreviousNodes() {
    return m_previousNodes;
}
vector < Node * > & Node::getSubsequentNodes()  {
    return m_subsequentNodes;
}
size_t Node::getPreviousNodeCount() const {
    return m_previousNodes.size();
}
size_t Node::getSubsequentNodeCount()const {
    return m_subsequentNodes.size();
}

void Node::init(FlowLineDao& dao) {
	Adaptable* adaptable = dynamic_cast<Adaptable*>(this);
	if(adaptable)
		adaptable->getAdapter()->init(dao);
    doInit(dao);
}

void Node::doInit(FlowLineDao& dao) {}

//    bool Node::isInDeliverySection() const {
//    	return m_inDeliverySection;
//    }
//    
//    void Node::setInDeliverySection(bool flag) {
//    	m_inDeliverySection = flag;
//    }

void Node::enter(Context & ctx, Node * from) {
  //  if(m_inDeliverySection && !ctx.isAtTerm())
   // 	return;
    	 
	if (from)
    	m_enteredNodes.insert(from);
	if (ctx.canEnter(*this))
	doEnter(ctx, from);
    
}
bool Node::isAllFinished() const {
    return m_allFinished;
}
void Node::reset() {
    doReset();
    m_enteredNodes.clear();
}
void Node::doReset() {}
void Node::setAllFinished(bool finished) {
    m_allFinished = finished;
}

};

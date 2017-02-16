/*
* Copyright (c) 2005, 联创科技
* All rights reserved.
* 
* $Header: /cvsroot/BSS4.2/BSS4.2/billing/bude/syscomp/src/flowline/engine/src/flowline/Node.h,v 1.1 2011/04/15 07:39:42 sunam Exp $
* $Log: Node.h,v $
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
* Revision 1.1  2005/05/08 11:57:49  luzl
* *** empty log message ***
*
* Revision 1.4  2005/04/27 14:59:30  luzl
* *** empty log message ***
*
* Revision 1.3  2005/03/23 06:15:05  luzl
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

#ifndef _NODE_H_
#define _NODE_H_
#include "config-all.h"
#include "flowline/Element.h"
#include "flowline/Metadata.h"
#include <string>
#include <vector>
#include <set>

USING_NAMESPACE(std);

namespace flowline {
class Context;
class FlowLineDao;
/** 流程的一个节点 */
class Node : virtual public Element {
public:
    Node();
    virtual ~Node();
    
    void addPreviousNode(Node * node);

    /** 取得转移, 如果列出名称, 则必须只有一个转移, 否则异常 */
    Node * getPreviousNode(const string& name = "") ;
    Node * getSubsequentNode(const string& name = "") ;

    vector <Node * > & getPreviousNodes();
    vector <Node *> & getSubsequentNodes();

    size_t getPreviousNodeCount() const;
    size_t getSubsequentNodeCount() const;

    void init(FlowLineDao& dao);
    virtual void doInit(FlowLineDao& dao);

    void enter(Context & ctx, Node * from);
    virtual void doEnter(Context & ctx, Node * from) = 0;

    bool isAllFinished() const;

    void reset();
    virtual void doReset();

protected:

    void setAllFinished(bool finished);

    /** 所有流转出去的转移 */
    vector <Node * > m_previousNodes;
    vector <Node * > m_subsequentNodes;

    set< Node * > m_enteredNodes;


    bool m_allFinished;
};

class Storage : public Node {
public:	
	Storage():m_metadata(0) {}
	
	void setMetadata(const Metadata* metadata) {
    	m_metadata = metadata;
    }
    virtual const Metadata* getMetadata() {
    	return m_metadata;
    }
private:
    const Metadata* m_metadata;	
};

class Executor : public Node {
public:	
	Executor() : m_delivery(false) {}
	
	bool isDelivery() const {
		return m_delivery;
	}
    void setDelivery(bool flag) {
    	m_delivery = flag;
    }
private:
    bool m_delivery; 
};

}
#endif //_NODE_H_

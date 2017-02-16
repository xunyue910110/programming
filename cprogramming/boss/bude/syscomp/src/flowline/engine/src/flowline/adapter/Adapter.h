/*
* Copyright (c) 2005, 联创科技
* All rights reserved.
* 
* $Header: /cvsroot/BSS4.2/BSS4.2/billing/bude/syscomp/src/flowline/engine/src/flowline/adapter/Adapter.h,v 1.1 2011/04/15 07:39:45 sunam Exp $
* $Log: Adapter.h,v $
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
* Revision 1.3  2005/04/27 14:59:27  luzl
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
#ifndef _ADAPTERS_H_
#define _ADAPTERS_H_
#include "config-all.h"
#include "flowline/Element.h"
#include "base/ClassRegister.h"
#include <string>

USING_NAMESPACE(std);
namespace flowline {
class FlowLineDao;

namespace adapter {
class AdapterFactory;


/** 系统调用适配器 */
class Adapter {
public:
    virtual ~Adapter(){};
    
    virtual void init(FlowLineDao& dao) = 0;
	
    void setFactory(AdapterFactory* factory) {
    	m_factory = factory;
    }

    AdapterFactory* getFactory() {
    	return m_factory;
    }

private:
    AdapterFactory *m_factory;
};

DECLARE_CLASS_REGISTER(Adapter)

/** 系统调用适配器 */
class ElementAdapter : public Adapter {
public:
	/**
	 * 设置适配的对象
	 */ 
    void setTarget(Element* element) {
    	m_target = element;
    }
	/**
	 * 获取适配的对象
	 */ 
    Element* getTarget() {
    	return m_target;
    }

private:
    Element *m_target;
};
}
}

#endif //_ADAPTERS_H_


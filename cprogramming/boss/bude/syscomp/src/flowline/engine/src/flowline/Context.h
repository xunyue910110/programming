/*
* Copyright (c) 2005, 联创科技
* All rights reserved.
* 
* $Header: /cvsroot/BSS4.2/BSS4.2/billing/bude/syscomp/src/flowline/engine/src/flowline/Context.h,v 1.1 2011/04/15 07:39:40 sunam Exp $
* $Log: Context.h,v $
* Revision 1.1  2011/04/15 07:39:40  sunam
* *** empty log message ***
*
* Revision 1.1  2011/03/24 12:01:00  hanyu
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
* Revision 1.5  2005/04/27 14:59:28  luzl
* *** empty log message ***
*
* Revision 1.4  2005/03/23 06:04:04  luzl
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

#ifndef _CONTEXT_H_
#define _CONTEXT_H_
#include "config-all.h"
#include "flowline/Resource.h"
#include "flowline/Context.h"
#include "flowline/Cycle.h"
#include "log4cpp/Logger.h"
#include "flowline/SystemInfo.h"
#include "flowline/FlowLineDao.h"
#include "base/Properties.h"
#include <string>
#include <map>


USING_NAMESPACE(std);
using namespace log4cpp;

namespace flowline {   
class	Node;

/**
 * FlowLine的运行时刻上下文
 */ 
class Context {
protected:	
	Context* m_parent;
	map<string, string> m_attributes;
public:
	/**
	 * 构造
	 */ 
    Context(Context* parent) : m_parent(parent) {
    }
    virtual Definition& getDefinition() {
    	if(m_parent != 0) {
			return m_parent->getDefinition();
		}
		ASSERT(false);
    }
    
    virtual const Properties& getEnvironment()  {
    	if(m_parent != 0) {
    		return m_parent->getEnvironment();
    	}
    	ASSERT(false);
    }
    /**
     * 是否能进入该节点
     */ 
	virtual bool canEnter(Node& node) {
		if(m_parent != 0) {
			return m_parent->canEnter(node);
		}
		ASSERT(false);
	}
	
	/**
	 * 获取DAO
	 */ 
	virtual FlowLineDao& getFlowLineDao() {
		if(m_parent !=0) {
			return m_parent->getFlowLineDao();
		}
		ASSERT(false);
	}

	/**
	 * 获取周期日志ID
	 */ 
	virtual int getCycleLogId() {
		if(m_parent !=0) {
		  return m_parent->getCycleLogId();
		}
		ASSERT(false);
	}

	/**
	 * 获取批处理ID
	 */ 
	virtual int getBatchLogId() {
		if(m_parent !=0) {
			return m_parent->getBatchLogId();
		}
		ASSERT(false);
	}
	
//	通过DAO获取
//	virtual int log(Node& actor, time_t begin, time_t end) {
//		if(m_parent !=0) {
//			return m_parent->log(actor, begin, end);
//		}
//		ASSERT(false);
//	}
//  通过DAO获取	
//	virtual int nextId(const string& key) {
//		if(m_parent !=0) {
//		  return m_parent->nextId(key);
//		}
//		ASSERT(false);
//	}
//	通过DAO获取
//	virtual int log(Node& actor, vector<pair<PResource, int> >& input, vector<PResource>& output, time_t begin, time_t end) {
//		if(m_parent !=0) {
//			return m_parent->log(actor,input, output, begin, end);
//		}
//		ASSERT(false);
//	}

	/**
	 * 获取周期详细信息
	 */ 
	virtual const Cycle::Instance& getCycleInstance() {
		if(m_parent !=0) {
			return m_parent->getCycleInstance();
		}
		ASSERT(false);
	}
	
	/**
	 * 获取日志句柄
	 */ 	
	virtual Logger& getLogger() {
		if(m_parent != 0) {
			return m_parent->getLogger();
		}
		ASSERT(false);
	}
//	通过DAO获取
//	virtual const Host& getHost(int id) {
//		if(m_parent != 0) {
//			return m_parent->getHost(id);
//		}
//		ASSERT(false);
//	}
//	通过DAO获取
//	virtual const Database& getDatabase(int id) {
//		if(m_parent != 0) {
//			return m_parent->getDatabase(id);
//		}
//		ASSERT(false);
//	}
	
	/**
	 * 设置一个属性
	 */ 
    virtual void setAttribute(const string& name, const string& value) {
    	m_attributes[name] = value;
    }
    
    /**
     * 获取一个属性, 如果在本context找不到, 就到parent找
     */ 
    virtual string getAttribute(const string& name) const {
    	map<string, string>::const_iterator it = m_attributes.find(name);
    	if(it != m_attributes.end()) {
    		return it->second;
    	}
    	if(m_parent != 0)
    		return m_parent->getAttribute(name);
    	return string();
    }
    
    /**
     * 析构
     */ 
	virtual ~Context() {}
};

}

#endif //_CONTEXT_H_


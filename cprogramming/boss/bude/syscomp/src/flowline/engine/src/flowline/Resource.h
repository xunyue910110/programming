/*
* Copyright (c) 2005, 联创科技
* All rights reserved.
* 
* $Header: /cvsroot/BSS4.2/BSS4.2/billing/bude/syscomp/src/flowline/engine/src/flowline/Resource.h,v 1.1 2011/04/15 07:39:42 sunam Exp $
* $Log: Resource.h,v $
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
* Revision 1.6  2005/04/27 14:59:29  luzl
* *** empty log message ***
*
* Revision 1.5  2005/03/23 05:58:11  luzl
* 不需要write返回int, 如果出错就异常
*
* Revision 1.4  2005/03/17 07:37:40  juyf
* *** empty log message ***
*
* Revision 1.3  2005/03/14 12:53:23  luzl
* 使用DataSet
*
* Revision 1.2  2005/03/12 03:32:13  juyf
* int write(ctx,Data);
*
* Revision 1.1  2005/03/07 05:58:11  luzl
* 改名dpp为flowline
*
* Revision 1.1  2005/03/04 10:24:25  luzl
* init
* 
*/
#ifndef _RESOURCE_H_
#define _RESOURCE_H_
#include "config-all.h"
#include <string>
#include "flowline/DataSet.h"
#include "base/SmartPtr.h"

USING_NAMESPACE(std);

namespace flowline {


class ResourceRepository;
class Context;	
/**
 * 即一个特定的实体, 物理实体
 */
class Resource {

public:
    virtual ~Resource() {}
    virtual const string& getName() const= 0;

    virtual int getMaxRecordCount() const = 0;
	/*
	 * 取得处理的记录数, 如果此资源没有经过读写可以直接返回-1
	 */
	virtual int getRecordCount() const= 0;
    /**
     * 知道是NULL, 表示读完毕
     */
    virtual PDataSet read(Context& ctx) = 0;
    virtual void write(Context& ctx, PDataSet data) = 0;
    virtual void close() = 0;
    //
    ResourceRepository* getRepository() { 
    	return m_repository; 
    }
    void setRepository(ResourceRepository* repository) {
    	m_repository = repository; 
    }
private:
    ResourceRepository* m_repository;
};
typedef counted_ptr<Resource> PResource;
}
#endif //_RESOURCE_H_

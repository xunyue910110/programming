/*
* Copyright (c) 2005, 联创科技
* All rights reserved.
* 
* $Header: /cvsroot/BSS4.2/BSS4.2/billing/bude/syscomp/src/flowline/engine/src/flowline/adapter/AdapterFactory.h,v 1.1 2011/04/15 07:39:45 sunam Exp $
* $Log: AdapterFactory.h,v $
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
* Revision 1.4  2005/04/27 14:59:28  luzl
* *** empty log message ***
*
* Revision 1.3  2005/03/23 06:15:26  luzl
* *** empty log message ***
*
* Revision 1.2  2005/03/14 12:53:37  luzl
* *** empty log message ***
*
* Revision 1.1  2005/03/07 05:58:14  luzl
* 改名dpp为flowline
*
* Revision 1.1  2005/03/04 10:24:27  luzl
* init
* 
*/
#ifndef _ADAPTER_FACTORY_H_
#define _ADAPTER_FACTORY_H_
#include "config-all.h"
#include "flowline/adapter/Adapter.h"
#include "base/Properties.h"

#include <string>
#include <vector>
USING_NAMESPACE(std);

namespace flowline{
namespace adapter{

	
/** 句柄工厂 */
class AdapterFactory {
public:
    
    Adapter * create(const string& className) {
    	Adapter* adapter = doCreate(className);
    	if(adapter) {
    		adapter->setFactory(this);
    	}
    	return adapter;
    }
    virtual void init(const Properties* env) {}
    /**
     * 创建一个句柄, 生命周期有客户负责(客户程序负责delete该对象)
     * @param 句柄的类型名称
     * @return 句柄
     * @exception execution_error 执行期间出现异常
     */
    virtual Adapter * doCreate(const string & className) = 0;
    virtual void getClassNames(vector<string>& classNames) const = 0;
    virtual ~AdapterFactory(){}
};
}
}    
#endif //_ADAPTER_FACTORY_H_

#ifndef _FILETASKADAPTER_H_
#define _FILETASKADAPTER_H_

#include "flowline/Context.h"
#include "flowline/adapter/TaskAdapter.h"
#include "fileadapter/Util.h"
#include "flowline/flbase.h"
#include "flowline/FlowLineDao.h"

#include "flowline/Context.h"
#include "file/File.h"
#include "flowline/flbase.h"
#include "base/ClassRegister.h"
#include "flowline/flbase.h"
#include "flowline/SystemInfo.h"


#include <map>
#include <string>
#include <vector>

using namespace flowline;
using namespace flowline::adapter;

namespace fileadapter
{

class ShellTaskAdapter : public flowline::adapter::TaskAdapter
{
public:
	ShellTaskAdapter(){};
	    
    void init(FlowLineDao & dao){
       shell = this->getTarget()->getExtendedAttribute("cmd");
    }
	/**
     * 执行一个动作
     * @param ctx 流程的上下文
     * @exception execution_error 执行期间出现异常
     */
    virtual void execute(Context & ctx){   
	    //Logger& log = Logger::getLogger("fileadapter.FileTaskAdapter");
	    //执行根据配置执行	   
	    string sh  = this->shell;
	    //File file(this->shell);
	    //if(sh != "" && file.isFile() && file.exists() && file.canRead() ){
	        FL_TRACE_P1("[flowline]开始执行shell:%s",sh.c_str());
	        system(sh.c_str());
	        FL_TRACE_P1("[flowline]shell执行完毕:%s",sh.c_str());
	    //}else {
	    //    FL_TRACE_P1("[flowline]没有可以执行shell:%s",sh.c_str());
	    //}    
	}
      
	private :	
    	string shell;
};
REGISTER_CLASS("ShellAdapter", ShellTaskAdapter,Adapter);
};


#endif //_FILETASKADAPTER_H_

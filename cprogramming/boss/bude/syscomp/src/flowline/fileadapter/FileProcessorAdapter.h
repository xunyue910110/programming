#ifndef _FILEPROCESSORADAPTER_H_
#define _FILEPROCESSORADAPTER_H_

#include "flowline/Resource.h"
#include "flowline/Context.h"
//#include "fileadapter/FileMetadata.h"
#include "flowline/adapter/ResourceProcessorAdapter.h"
#include "flowline/FlowLineDao.h"

#include <string> 

using namespace flowline;
using namespace flowline::adapter;

namespace fileadapter
{

class FileProcessorAdapter : public flowline::adapter::ResourceProcessorAdapter
{
public:
	FileProcessorAdapter();
	virtual ~FileProcessorAdapter();
    virtual void init(FlowLineDao& dao){};
	
	 /**
     * 执行一个动作
     * @param ctx 流程的上下文
     * @exception execution_error 执行期间出现异常
     */
    virtual void process( Context & ctx, PResource input, vector<PResource>& output);
    virtual void finish( Context& ctx, vector<PResource>& output );
private :
    string configuration;
 
    
};

};

#endif //_FILEPROCESSORADAPTER_H_

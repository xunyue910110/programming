#ifndef _DATASETDISPATCHADAPTER_H_
#define _DATASETDISPATCHADAPTER_H_

#include "flowline/adapter/DataProcessorAdapter.h"

#include "flowline/adapter/DataProcessorAdapter.h"
#include "flowline/Context.h"
#include "flowline/FlowLineDao.h"
#include "flowline/DataSet.h"
#include "flowline/scripts.h"
#include "flowline/flbase.h"
#include <string>
#include <vector>


namespace flowline
{

namespace dataprocessor
{

using namespace flowline::adapter;
using namespace flowline;

#define _MAPPING_ADAPTER_DETACH_SCRIPT_  "dispatch_script"

class DataSetDispatchAdapter : public DataProcessorAdapter
{
public:
	DataSetDispatchAdapter():rowjavaScript(0){
	}
	virtual ~DataSetDispatchAdapter();
	
	//初始化得到目标DataSet的数据结构
    void init(FlowLineDao& dao);
        
	//分解处理
    void process(Context & ctx, pair<string, PDataSet>& srcdata, map<string, PDataSet>& result);
    //分解完成    
    void finish(Context& ctx, bool isAllFinished, map<string, PDataSet>& result );
    
    //数据结构相同，复制结构
    PDataSet copyMetadata(PDataSet src){
		PDataSet data(new DataSet());
		int fcount = src->fieldCount();
		for( int i = 0;i < fcount ;i++){
			data->addField( dataset::Field(src->getField(i).getFieldName(),
							src->getField(i).getDataType(),
							src->getField(i).getLength()));
		}  
		return data;
    }
    
    string getDispatchScript(){
    	return this->getTarget()->getExtendedAttribute(_MAPPING_ADAPTER_DETACH_SCRIPT_);
    }
private :    
     RowJSContainer container;  
	   RowJavaScript* rowjavaScript;
};

}

}

#endif //_DATASETDISPATCHADAPTER_H_

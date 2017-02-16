
#include "base/ClassRegister.h"
#include "flowline/Context.h"
#include "flowline/FlowLineDao.h"
#include "flowline/DataSet.h"
#include "flowline/scripts.h"
#include "flowline/adapter/DataProcessorAdapter.h"

namespace flowline {	
namespace dataprocessor{
using namespace flowline::adapter;
using namespace flowline;

class FilterAdapter : public DataProcessorAdapter {
	string m_filter_script;
	
  RowJSContainer container;  
  RowJavaScript  *rowjavaScript;      
  
public:	
	virtual void init(flowline::FlowLineDao&) {
		 m_filter_script = this->getTarget()->getExtendedAttribute("filter_script");	  		 
	}
    
	virtual void process( Context & ctx, pair<string, PDataSet>& data, map<string, PDataSet>& result ) {
		//result[string()] = data.second;
		if(m_filter_script.empty()){
			//不知道要过滤什么，就原样输出吧
			result.insert(pair<string, PDataSet>(data));
			return;
		}
	
	     string srcname = data.first;
       PDataSet pFrom = data.second; 
       DataSet::iterator it = pFrom->begin(); //源头的叠代指针 
       int fcount = pFrom->fieldCount();
        
       RowJavaScript& rowjavaScript = container.get(m_filter_script.c_str());       
       	
       container.setGlobalObject(srcname.c_str(), RowJavaScript::rowClass(), &it); 
       container.setGlobalObject("ctx", RowJavaScript::ctxClass(), &ctx); 
      
       string name = "";
      
       map<string, PDataSet>::iterator mapit;
       PDataSet pTo = this->copyMetadata(pFrom);
		long count = 0;
		long maxcount = 0;
		for(; it!= pFrom->end(); it++){
			rowjavaScript.execute(name);
			if(name == srcname){    
				DataSet::iterator itTo = pTo->pushBack();//目标的叠代指针     	       	   
				for( int i = 0; i < fcount ;i++){
					(*itTo)[i] = (*it)[i];
				}  
				count++;
			} 
			maxcount++;  	   
		}    
    	FL_TRACE_P2("过滤完毕,总记录:%d条,过滤后剩:%d条",maxcount,count);	
    	
        result.insert(pair<string, PDataSet>(string(),pTo));	
	}
    
    
	virtual void finish( Context& ctx, bool isAllFinished, map<string, PDataSet>& result ) {
	}	
	
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
};

REGISTER_CLASS("FilterAdapter", FilterAdapter, Adapter);
};
};

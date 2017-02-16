#include "DataSetDispatchAdapter.h"

#include "flowline/adapter/DataProcessorAdapter.h"
#include "base/ClassRegister.h"
#include "flowline/Context.h"
#include "flowline/FlowLineDao.h"
#include "flowline/DataSet.h"
//#include "flowline/RepositoryDefinition.h"
#include "flowline/Element.h"
#include "flowline/flbase.h"

#include <string>
#include <vector>
#include <map>
namespace flowline
{
namespace dataprocessor
{

DataSetDispatchAdapter::~DataSetDispatchAdapter()
{
	
}

void DataSetDispatchAdapter::init(FlowLineDao& dao)
{       
       string script = this->getDispatchScript(); 
       
       rowjavaScript = &container.get(script.c_str()); 
       
}

void DataSetDispatchAdapter::process(Context & ctx, pair<string, PDataSet>& srcdata, map<string, PDataSet>& result){
       ASSERT(rowjavaScript!=0);//zzc
       string srcname = srcdata.first;
       PDataSet pFrom = srcdata.second; 
       DataSet::iterator it = pFrom->begin(); //源头的叠代指针 
       int fcount = pFrom->fieldCount();
       string name;
       //把指针传给Script   
       container.setGlobalObject(srcname.c_str(), RowJavaScript::rowClass(), &it); 
       container.setGlobalObject("ctx", RowJavaScript::ctxClass(), &ctx); 
   
       if(getDispatchScript().find(srcname) == string::npos){//判断是否通过内容分拣
           rowjavaScript->execute(name);
           result[name] = pFrom;
       //-------------------------------------------------------------
      }else{ //通过字体段分类文件
	       map<string, PDataSet>::iterator mapit;
	       PDataSet pTo;
	       for(; it!= pFrom->end(); it++){
	       	   rowjavaScript->execute(name);
		       mapit = result.find(name);
		       //cout<<"mapit value is :  "<<"/"<<name<<endl;
		       if( mapit == result.end()){//没有找到
		       	   pTo = this->copyMetadata(pFrom); 
		       	   result.insert(pair<string, PDataSet>(name,pTo));
		       }else{
		           pTo = result[name];
		       }                 
		       DataSet::iterator itTo = pTo->pushBack();//目标的叠代指针     	       	   
	           for( int i = 0; i < fcount ;i++){
					(*itTo)[i] = (*it)[i];
	     	   }     	   
	    	}    
      }
      FL_TRACE_P1("分解完毕,SIZE:%d",result.size());	
}      
    
    
void DataSetDispatchAdapter::finish(Context& ctx, bool isAllFinished, map<string, PDataSet>& result )
{
   FL_TRACE_P1("DataSetDispatchAdapter::finish,SIZE:%d",result.size());	
}

REGISTER_CLASS("Dispatch", DataSetDispatchAdapter, Adapter);

}

}

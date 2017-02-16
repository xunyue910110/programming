#include "flowline/adapter/DataProcessorAdapter.h"
#include "base/ClassRegister.h"

#include "flowline/adapter/DataProcessorAdapter.h"
#include "flowline/Context.h"
#include "flowline/FlowLineDao.h"
#include "flowline/DataSet.h"
#include "flowline/Node.h"

//#include "flowline/RepositoryDefinition.h"
#include "flowline/scripts.h"
#include "flowline/flbase.h"
#include <string>
#include <vector>

namespace flowline {	
namespace dataprocessor{
using namespace flowline::adapter;
using namespace flowline;

class DuplicatorAdapter : public DataProcessorAdapter {
	vector<string> trgnames;
public:	
	void init(flowline::FlowLineDao&) {	
	}
	//查出一共要复制多少份，每个名字是什么
    void initTargets(Context & ctx){
       trgnames.clear();
       Node*  thisNode =  dynamic_cast< Node* >(getTarget());
       vector<Node* > nexts = thisNode->getSubsequentNodes();
       Node* nextone;
       string nextname;
       vector<Node* >::iterator it = nexts.begin();
       for(;it!=nexts.end();it++){
          nextone  = *it;
          nextname = nextone->getName();
          FL_TRACE_P1("找到下一个buffer节点的名字:%s",nextname.c_str());
          trgnames.push_back(nextname);
       }       
    };
    
	virtual void process( Context & ctx, pair<string, PDataSet>& data, map<string, PDataSet>& result ) {
		FL_TRACE_P1("复制器复制开始，复制器名:%s",getTarget()->getName().c_str());	  
		initTargets(ctx);		
		vector<string>::iterator it = trgnames.begin();
		string trgstr;
		int count=0;
		for(;it!=trgnames.end();it++){
            trgstr = *it;
		    result[trgstr] = data.second;
		    //实在不行就用insert
		   // result.insert(pair<string, PDataSet>(trgstr, data.second));
		    count++;
		}
	    FL_TRACE_P1("复制器复制完成，总复制份数:%d",count);	    	

	}

	virtual void finish( Context& ctx, bool isAllFinished, map<string, PDataSet>& result ) {
	}
};

REGISTER_CLASS("DuplicatorAdapter", DuplicatorAdapter, Adapter);
}
}

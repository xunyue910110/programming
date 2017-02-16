#include "flowline/adapter/DataProcessorAdapter.h"
#include "base/ClassRegister.h"

namespace flowline {	
namespace dataprocessor{
using namespace flowline::adapter;
using namespace flowline;

class DataTransportAdapter : public DataProcessorAdapter {
public:	
	virtual void init(flowline::FlowLineDao&) {
	}

	virtual void process( Context & ctx, pair<string, PDataSet>& data, map<string, PDataSet>& result ) {
		result[string()] = data.second;
	}

	virtual void finish( Context& ctx, bool isAllFinished, map<string, PDataSet>& result ) {
	}			
};

REGISTER_CLASS("DataTransportAdapter", DataTransportAdapter, Adapter);
}
}

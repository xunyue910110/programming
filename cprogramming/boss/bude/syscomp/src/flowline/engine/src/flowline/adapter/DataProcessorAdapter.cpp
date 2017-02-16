#include "DataProcessorAdapter.h"
#include "flowline/DataProcessor.h"
namespace flowline {
namespace adapter {	
const Metadata* DataProcessorAdapter::getMetadata(const string& bufferName) {
	DataProcessor * p = dynamic_cast<DataProcessor*>( getTarget() );
    Node* n = p->getSubsequentNode( bufferName );
    return dynamic_cast<Storage*>( n ) ->getMetadata();
}
void DataProcessorAdapter::locateReference(Context& ctx, DataSet::iterator& it, map<string, DataSet::iterator>& result) {
	DataProcessor * p = dynamic_cast<DataProcessor*>( getTarget() );
    p->locateReference(ctx, it, result);    
}
void DataProcessorAdapter::getReferenceNames(vector<string>& names) {
	DataProcessor * p = dynamic_cast<DataProcessor*>( getTarget() );
    p->getReferenceNames(names);    
}
}
}


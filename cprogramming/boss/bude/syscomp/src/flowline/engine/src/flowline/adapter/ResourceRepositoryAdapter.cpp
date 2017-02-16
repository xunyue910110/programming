#include "ResourceRepositoryAdapter.h"
#include "flowline/ResourceRepository.h"
namespace flowline {
namespace adapter {	
const Metadata* ResourceRepositoryAdapter::getMetadata() {
	return dynamic_cast<ResourceRepository*>(getTarget())->getMetadata();
}
}
}

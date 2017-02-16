#include "RootContext.h"

namespace flowline {

const string FL_LOGGER_PREFIX = "bude.flowline.";

RootContext::RootContext() : Context(0) {}

RootContext::~RootContext() {}

void RootContext::setFlowLineDao( FlowLineDao* flowlineDao ) {
    m_flowLineDao = flowlineDao;
}

FlowLineDao& RootContext::getFlowLineDao() {
	return *m_flowLineDao;
}

void RootContext::setBatchLogId( int batchLogId ) {
    m_batchLogId = batchLogId;
}

int RootContext::getBatchLogId() {
	return m_batchLogId;
}

void RootContext::setDefinition(Definition* def) {
	m_definition = def;
}

Definition& RootContext::getDefinition() {
	return *m_definition;
}
//int RootContext::nextId( const string& key ) {
//    ASSERT( m_flowLineDao != 0 );
//    return m_flowLineDao->nextId( key );
//}


Logger& RootContext::getLogger() {
	return Logger::getLogger(FL_LOGGER_PREFIX + m_definition->getName());
}

const Cycle::Instance& RootContext::getCycleInstance() {
    return m_cycleInstance;
}

void RootContext::setCycleInstance( const Cycle::Instance& cins ) {
    m_cycleInstance = cins;
}

void RootContext::setCycleLogId(int cycleLogId) {
	m_cycleLogId = cycleLogId;
}

int RootContext::getCycleLogId() {
	return m_cycleLogId;
}

string RootContext::getAttribute( const string& name ) const {
    if ( name == "cycleId" ) { //TODO remove
        return m_cycleInstance.getId();
    }
    return Context::getAttribute( name );
}

const Properties& RootContext::getEnvironment()  {
	return *m_env;
}

void RootContext::setEnvironment(const Properties* prop) {
	m_env = prop;
}

//const Host& RootContext::getHost(int id) {
//	return m_flowLineDao->getHost(id);
//}
//	
//const Database& RootContext::getDatabase(int id) {
//	return m_flowLineDao->getDatabase(id);
//}

}

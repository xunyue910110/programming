#include "lcu/lcu.h" 

#include <sys/types.h>
#include <iostream>
#include "log4cpp/Logger.h"
USING_NAMESPACE(std);
using namespace log4cpp;
using namespace ebpm4c;

void Context::log(Node* node) {
  
   //cout << "----------log Node-----------" << node->getName() <<endl;
   if(dynamic_cast<ProcessState*>(node)) {    
      logs.insert(logs.end(), pair<Node*, int>(node, (*count) + 1));      
   } else {
      (*logstep)++;
      logs.insert(logs.end(), pair<Node*, int>(node, -1));
   }
  
}

string Context::getVariant(const string& vname){
   return "0";
}
void Context::setEndState(EndState* end){
}
const vector< pair<Node*, int> > & Context::getLogs() const {
  return logs;
}


Element::Element() : id(0) {
}

int64_t Element::getId() const {
    return id;
}

const string & Element::getName() const {
    return this->name;
}

const string & Element::getDescription() const {
    return description;
}

void Element::setId(int64_t _id) {
    this->id = _id;
}

void Element::setName(const string & _name) {
    this->name = _name;
}

void Element::setDescription(const string & _description) {
    this->description = _description;
}

string Element::getExtendedAttribute(const string & name_) const{
	map < string, string >::const_iterator it = extendedAttributes.find(name_);
    if (it == extendedAttributes.end()){ 
    	//throw definition_error(string("没有找到指定的扩展属性:") + name);
    	return "";
    }
    return it->second;
}

void Element::setExtendedAttribute(const string & name_, const string& value){
	extendedAttributes.insert(pair<string ,string >(name_,value));
}

Transition::Transition() {
}

Transition::Transition(Node* _to) {
    setTo(_to);
}

/**
 * 接受线程
 * @param ctx 流程的上下文
 */
void Transition::acceptToken(Context & ctx) const {
    //ctx.log(this->to);//在此处写Log
    //cout << "[LCU]acceptToken. Transition.name=" << this->getName() <<endl;
    Logger& log = Logger::getLogger("gboss.lcu.trace");
    LOG_TRACE_P1(log,"[LCU]acceptToken. Transition.name=%s",this->getName().c_str());
    if(this->to) this->to->acceptToken(ctx);
}

/**
 * 设置终点
 * @param to 终点
 */
void Transition::setTo(Node* _to) {
    this->to = _to;
}

Delegation::Delegation() {
}

const int64_t Delegation::getId() const {
    return id;
}

void Delegation::setId(const int64_t _id) {
    this->id = _id;
}

const string & Delegation::getClassName() const {
    return className;
}

void Delegation::setClassName(const string & _className) {
    this->className = _className;
}

const string & Delegation::getConfiguration() const {
    return configuration;
}

void Delegation::setConfiguration(const string & _configuration) {
    this->configuration = _configuration;
}

void Delegation::setHandleFactory(PHandleFactory _handleFactory) {
    this->handleFactory = _handleFactory;
}
/**
 * 获得回调句柄
 * @return 回调句柄
 */
const PHandle & Delegation::getHandle() {	
	if(!handle){	  
	   handle = this->handleFactory->create(this->getClassName());
	   handle->configure(configuration);
	}
	if(!handle){	 
	    THROW(LCUDefinitionException,"句柄工厂创建回调句柄异常：ClassName="+this->getClassName());
	}	
    return handle;
}

Delegation::~Delegation() {
}

Action::Action(const PDelegation & _delegation) {
    this->delegation = _delegation;
}

/**
 * 通过委托取得外部动作调用句柄, 进行外部调用
 * @param ctx 流传的上下文
 */
void Action::execute(Context & ctx) { //throw (ActionException)
    PHandle h = delegation->getHandle();
    PActionHandle handle = dynamic_pointer_cast < ActionHandle > (h);
    handle->execute(ctx);   
}

State::State() {
}

/**
 * 接受一个线程
 * @param ctx 流程的上下文
 */
void State::acceptToken(Context & ctx) const {
   
    //if(typeid(this)!=typeid(StartState*))//过滤不了开始节点
    ctx.log((Node*)this);
	//cout << "[LCU]acceptToken. Node.name=" << this->getName() << endl;
	Logger& log = Logger::getLogger("gboss.lcu.trace");
    LOG_TRACE_P1(log,"[LCU]acceptToken. Node.name=%s",this->getName().c_str());
    
    vector < PAction >::const_iterator it = actions.begin();
    for ( ; it != actions.end(); it++) {
        (* it)->execute(ctx);
    }
   
    if(ctx.getErrorCode()!=0){
      if (leavingTransition){ 
         if(leavingTransition->getName() == "errortransition"){
            leavingTransition->acceptToken(ctx);
            //return;
         }else{
            //发现节点执行有业务异常，但此节点并没有配置业务异常判断节点，则抛出异常          
            THROW_C(LCUWrapperException,ctx.getErrorCode(),ctx.getErrorInfo());
         }  
      }
    }
    else if (leavingTransition) {
	    	//cout << "[LCU]acceptToken. from Node.id:"<< this->getId()<<"/" << this->getName() << " to Transition.name=" << leavingTransition->getName() <<endl;
	    	leavingTransition->acceptToken(ctx);
	}
    
}

/**
 * 添加一个流转出的转移.
 * @param transition 转移
 */
void State::addLeavingTransition(const PTransition & transition) {
    //TODO 检查是已经有连线
    this->leavingTransition = transition;
}

/** 添加该状态的动作 */
void State::addAction(const PAction & action) {
    actions.push_back(action);
}

void StartState::acceptToken(Context & ctx) const{
   //cout << "[LCU]acceptToken. StartState.name=" << this->getName()<<endl;   
   Logger& log = Logger::getLogger("gboss.lcu.trace");
   LOG_TRACE_P1(log,"[LCU]acceptToken. StartState.name=%s",this->getName().c_str());
   if(leavingTransition) leavingTransition->acceptToken(ctx);
}  

void EndState::acceptToken(Context & ctx) const{
     //ctx.log((Node*)this);
     //cout << "[LCU]acceptToken. EndState.name=" << this->getName()<< " endCode="<<this->endCode << " alert=" << this->alert << " commit=" << commit <<endl;
     Logger& log = Logger::getLogger("gboss.lcu.trace");
     LOG_TRACE_P4(log,"[LCU]acceptToken. EndState.name=%s ,endCode=%d,alert=%d,commit=%d",this->getName().c_str(), this->endCode , this->alert, this->commit);
     ctx.setErrorCode(this->endCode);
     string info = this->getName() + "\n" + ctx.getErrorInfo();//去掉这个info 前台就不再会有英文异常输出了
     ctx.setErrorInfo(info);  
    
     //cout << "[LCU]acceptToken. EndState.name="<< this->getName() << " 执行完毕." <<endl;
     ctx.setEndState((EndState*)this);
}
void EndState::setEndCode(int endcode){
     this->endCode = endcode;  
}

ProcessState::ProcessState(const PDelegation & _delegation) {
    this->delegation = _delegation;
}

/** 接受线程 */
void ProcessState::acceptToken(Context & ctx) const {
    ctx.log((Node*)this);
    //cout << "[LCU]acceptToken. subProcessState.name=" << this->getName()<<endl;
	Logger& log = Logger::getLogger("gboss.lcu.trace");
	LOG_TRACE_P1(log,"[LCU]acceptToken. SubProcessState.name=%s",this->getName().c_str());
     
    PHandle h = delegation->getHandle();
    PProcessInvocationHandle handle = dynamic_pointer_cast < ProcessInvocationHandle > (h);    
    handle->startSubProcess(ctx);      
    
    //cout << "state getErrorCode():"<<ctx.getErrorCode()<<endl;
    if(ctx.getErrorCode()!=0){
      if (leavingTransition){ 
         if(leavingTransition->getName() == "errortransition"){
            leavingTransition->acceptToken(ctx);
         }else{
            //发现节点执行有业务异常，但此节点并没有配置业务异常判断节点，则抛出异常
            THROW_C(LCUWrapperException,ctx.getErrorCode(),ctx.getErrorInfo());
         }  
      }
    }      
    else if (leavingTransition) leavingTransition->acceptToken(ctx);  
}

Decision::Decision(const PDelegation & _delegation) {
    this->delegation = _delegation;
}

/** 接受线程 */
void Decision::acceptToken(Context & ctx) const{
    //cout << "[LCU]acceptToken. TransDecision.name=" << this->getName()<<endl;
   	Logger& log = Logger::getLogger("gboss.lcu.trace");
	LOG_TRACE_P1(log,"[LCU]acceptToken. TransDecision.name=%s",this->getName().c_str());
     
    PHandle h = delegation->getHandle();
    PDecisionHandle handle = dynamic_pointer_cast < DecisionHandle > (h);
    
    //handle->configure(delegation->getConfiguration());    
    string trans = handle->decide(ctx); 
    
    map < string, PTransition >::const_iterator it = leavingTransitions.find(trans);
    if (it == leavingTransitions.end()){ 
		//throw definition_error(string("没有找到指定的转移:") + trans);
		THROW_C(LCUDefinitionException,-100,string("没有找到指定的转移:") + trans); 
    }
    it->second->acceptToken(ctx);
}

/** 添加一个流转出去的转移 */
void Decision::addLeavingTransition(const PTransition & transition) {	
    leavingTransitions[transition->getName()] = transition;
}

const string & Variable::getName() const {
    return name;
}

void Variable::setName(const string & _name) {
    this->name = _name;
}

const string & Type::getClassName() const {
    return className;
}

void Type::setClassName(const string _className) {
    this->className = _className;
}

void ProcessDefinition::setStartState(PStartState  _startState) {
    this->startState = _startState;
}

PStartState ProcessDefinition::getStartState() {
    return startState;
}      

void  ProcessDefinition::execute(Context & ctx) const {
   if(this->startState){       
       this->startState->acceptToken(ctx);
   } else {
       //throw definition_error("流程定义没有的开始节点");  
       THROW_C(LCUDefinitionException,-100,"流程定义没有的开始节点");   
   }    
} 

PNode ProcessDefinition::getNodeByID(const int64_t& nodeid){
     map_PNode::const_iterator it = nodes.find(nodeid);
     if (it != nodes.end()) {
		  return it->second;
     } else {
        //throw definition_error("读流程中没有定义的节点");
        THROW_C(LCUDefinitionException,-100,"读流程中没有定义的节点");
     }     
     //return 0;
}
void  ProcessDefinition::addNode(PNode node){
    //cout <<"Def:"<< this->getName() << "addNode():id=" << node->getId()<< " name=" << node->getName() <<endl;
    nodes.insert(pair<int64_t,PNode>(node->getId(),node));
}

//Engine::Engine(PHandleFactory  _factory, PDefinitionLoader  _loader) : factory(_factory), loader(_loader) {
//}
//
//void Engine::reloadDefinitions() {
//    definitions = loader->load(factory);
//}
//
//void Engine::execute(const string & name, Context & ctx) {
//    if (definitions == 0) reloadDefinitions();
//    ProcessDefinitions::iterator it = definitions->find(name);
//    if (it != definitions->end()) {
//        it->second->getStartState()->acceptToken(ctx);
//    } else {
//        throw execution_error(name + " - 流程没有被定义");
//    }
//}
//
//Engine::~Engine() {
//}
//
//string Engine::getProcessExtendedAttribute(const string& name,const string& attrkey) {
//	if (definitions == 0) reloadDefinitions();
//    ProcessDefinitions::iterator it = definitions->find(name);
//    if (it != definitions->end()) {
//		  return it->second->getExtendedAttribute(attrkey);
//    } else {
//        throw execution_error(name + " - 流程定义不存在或没有被加载");
//    }
//}

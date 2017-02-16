#ifndef EBPM4C_H 
#define EBPM4C_H

#include <map>
#include <vector>
#include <sys/types.h>
#include "base/SmartPtr.h"
#include "lcu/plugins.h"
#include "lcu/lcuexceptions.h"

USING_NAMESPACE(std);

/** 流程引擎内核 */
namespace ebpm4c {
    class Engine;
    typedef counted_ptr < Engine > PEngine;
    
    class Element;
    typedef counted_ptr < Element > PElement;
    class Node;
    typedef counted_ptr < Node > PNode;
    class Transition;
    typedef counted_ptr < Transition > PTransition;
    class Delegation;
    typedef counted_ptr < Delegation > PDelegation;
    class Action;
    typedef counted_ptr < Action > PAction;
    class State;
    typedef counted_ptr < State > PState;
    class StartState;
    typedef counted_ptr < StartState > PStartState;
    class EndState;
    typedef counted_ptr < EndState > PEndState;
    class ProcessState;
    typedef counted_ptr < ProcessState > PProcessState;
    class Decision;
    typedef counted_ptr < Decision > PDecision;
    class Variable;
    typedef counted_ptr < Variable > PVariable;
    class Type;
    typedef counted_ptr < Type > PType;
    class ProcessDefinition;
    typedef counted_ptr < ProcessDefinition > PProcessDefinition;
     /** 定义流程定义集合(key为流程名称, value为流程定义) */
    typedef map < string, PProcessDefinition > ProcessDefinitions;   
    typedef counted_ptr < ProcessDefinitions > PProcessDefinitions;
    
    //class DefinitionLoader;      
    //typedef counted_ptr < DefinitionLoader > PDefinitionLoader;
    class DefinitionLoaderDAO;  
    typedef map < string, PProcessDefinition >  base_ProcessDefinitions;  
    typedef map < int64_t, PProcessDefinition > map_ProcessDefinitions;
    
    typedef map < int64_t, PDelegation > map_PDelegation;
	
    typedef map < int64_t, PNode > map_PNode;
  
        
    /**上下文信息*/
    class Context {
      Context(const Context&){
      }
      Context& operator = (const Context&) {
         return *this;
      }
	 protected:
		Context* parent;//父流程的上下文
		
		int processid;//子流程序号 id=0为主流程
		counted_ptr<int> count;//子流程计数器
		counted_ptr<int> logstep;//调用计数器
		vector <pair<Node*, int> > logs;//调用轨迹 <节点,子流程号>      
		int errorCode;   //错误码
		string errorInfo;//错误信息
      
    public:   
		Context() : count(new int(0)), logstep(new int(0)),processid(0), parent(0) {  
			errorCode = 0;
			errorInfo = "";        
		} 
		Context(Context& _parent): parent(&_parent), count(_parent.count),logstep(_parent.logstep){
			processid = (*count) + 1;
			*count    = *count + 1;  
			errorCode = 0;
			errorInfo = "";
		}      
       
		int  getProcessId() {return this->processid;}
		void setProcessId(const int& id){ this->processid = id;} 
		  
		void setErrorCode(const int& code){ this->errorCode = code;} 
		int  getErrorCode(){return  this->errorCode;}
		
		void setErrorInfo(const string& info){ this->errorInfo = info;}
		string getErrorInfo(){return this->errorInfo;}   
		
		void setLogStep(const int& step){*logstep = step;}
		int getLogStep(){return  *logstep;}
		
		void log(Node* e);       
		const vector <pair<Node*, int> > & getLogs() const;
		//虚函数得到变量的值      
		virtual string getVariant(const string& vname);
		virtual void setEndState(EndState* end) = 0;
    };
   
    /** 流程模型的基本图形元素 */
    class Element {
    public:
        Element();

        int64_t getId() const;
        void setId(int64_t _id);

        const string & getName() const;
        
        void setName(const string & _name);

        const string & getDescription() const;
        
        void setDescription(const string & _description);
        
        string getExtendedAttribute(const string & name) const;
        
        void setExtendedAttribute(const string & name, const string& value);
    private:
        /** 元素的ID号, 每个版本的流程, 都有自己的ID */
        int64_t id;

        /** 元素的名称 */
        string name;

        /** 描述信息 */
        string description;
        /** 扩展属性*/
        map<string, string> extendedAttributes;        
    };



    /** 流程的一个节点 */
    class Node : public Element {
    public:
        /**
         * 让一个节点接受线程
         * @param ctx 流程运行的上下文
         */
        virtual void acceptToken(Context & ctx) const = 0;

        /** 添加一个流转出去的转移. */
        virtual void addLeavingTransition(const PTransition & transition) = 0;
    };



    /** 流程的转移 */
    class Transition : public Element {
    public:
        Transition();
        Transition(Node* to);

        /**
         * 接受线程
         * @param ctx 流程的上下文
         */
        void acceptToken(Context & ctx) const;

        /**
         * 设置终点
         * @param to 终点
         */
        void setTo(Node*  _to);      
      
    private:

        /**
         * 起始节点
         * @supplierCardinality 1
         */
        PNode from;

        /**
         * 终止节点
         * @supplierCardinality 1
         */
        Node* to;
      
    };



    /** 定义委托类, 通过它可以获得相应的回调句柄对象(Handle) */
    class Delegation {
    public:
        Delegation();
        virtual ~Delegation();

        const int64_t getId() const;
        void setId(const int64_t id);

        const string & getClassName() const;
        void setClassName(const string & className);

        const string & getConfiguration() const;
        void setConfiguration(const string & configuration);

        /**
         * 获得回调句柄
         * @return 回调句柄
         */
        const PHandle & getHandle() ;
         void setHandleFactory(PHandleFactory handleFactory);

    private:
        /** 委托的ID */
        int64_t id;

        /** 委托所使用的类名或模块名等, 只是一个标识. */
        string className;

        /** 回调句柄所使用的配置 */
        string configuration;

        PHandle handle;
        
        PHandleFactory handleFactory;        
    };



    /** 一个动作定义 */
    class Action {
    public:

        Action(const PDelegation & delegation);

        /**
         * 通过委托取得外部动作调用句柄, 进行外部调用
         * @param ctx 流传的上下文
         */
        void execute(Context & ctx);

    private:

        /**
         * 委托
         * @supplierCardinality 1
         * @directed
         * @link aggregationByValue
         */
        PDelegation delegation;
    };



    /** 流程的一个状态 */
    class State : public Node {
    public:
        State();

        /**
         * 接受一个线程
         * @param ctx 流程的上下文
         */
        virtual void acceptToken(Context & ctx) const;

        /**
         * 添加一个流转出的转移.
         * @param transition 转移
         */
        virtual void addLeavingTransition(const PTransition & transition);

        /** 添加该状态的动作 */
        void addAction(const PAction & action);
        const vector < PAction >& getActions() const{
           return actions;
        }
    protected:

        /**
         * 该状态配置的动作
         * @supplierCardinality 1
         * @directed
         * @link aggregationByValue
         */
        vector < PAction > actions;

        /** @supplierCardinality 0..1 */
        PTransition leavingTransition;
    };


    /** 启动状态 */
    class StartState : public State {   
        public:
       /**
         * 接受一个线程
         * @param ctx 流程的上下文
         */
        virtual void acceptToken(Context & ctx) const;    
    };


    /** 结束状态 */
    class EndState : public State {
        public:
           EndState(){
           		endCode = 0;alert=-1;commit = true;
           }
           /** 接受线程 */
           virtual void acceptToken(Context & ctx) const;
           void setEndCode(int endcode);
           void setAlertCode(int alertcode){
               alert = alertcode ;   	
           }
           int getAlertCode (){
           	   return alert;
           }
           void setCommit(string attr){
               commit = (attr!="false");                  
           }
           bool isCommit(){
           		return commit;
           };
        private :
           int endCode;  
           int alert;
           bool commit; 
    };
    


    /** 子流程状态 */
    class ProcessState : public State {
    public:
        ProcessState(const PDelegation & delegation);

        /** 接受线程 */
        virtual void acceptToken(Context & ctx) const;
        PDelegation getDelegation(){
            return delegation;
        }
    private:
        /**
         * 委托
         * @supplierCardinality 1
         * @directed
         * @link aggregationByValue
         */
        PDelegation delegation;
    };


    /** 定义判断节点 */
    class Decision : public Node {
    public:
        Decision(const PDelegation & delegation);

        /** 接受线程 */
        virtual void acceptToken(Context & ctx) const;

        /** 添加一个流转出去的转移 */
        virtual void addLeavingTransition(const PTransition & transition);

    private:

        /**
         * 判断委托
         * @link aggregationByValue
         * @supplierCardinality 1
         * @directed
         */
        PDelegation delegation;


        /** 所有流转出去的转移 */
        map < string, PTransition > leavingTransitions;
    };



    class Variable {
    public:
        const string & getName() const;
        void setName(const string & name);

    private:
        string name;
        int attribute1;
    };



    class Type {
    public:
        const string & getClassName() const;
        void setClassName(const string className);

    private:
        string className;

        /**
         *@link aggregationByValue
         *@supplierCardinality 1..n
         */
        vector < PVariable > variables;
    };

 

    /** 流程定义 */
    class ProcessDefinition : public Element {
    
    public:
        ProcessDefinition(){ }
        virtual ~ProcessDefinition(){}
        
        PStartState getStartState();
       
        void  setStartState(PStartState startState);
        
        PNode getNodeByID(const int64_t & nodeid);
        
        void addNode(PNode node);
       
        void  execute(Context & ctx) const; 
         
    private:
        map_PNode nodes;
        /**
         * 启动节点
         * @link aggregationByValue
         * @supplierCardinality 1
         */
        PStartState startState;
    };

 
    /** 流程定义数据库加载器 */
    class DefinitionLoaderDao{
       
    public:
         DefinitionLoaderDao(){};
         virtual ~DefinitionLoaderDao(){};
         /**
          *刷新重新载读所有的流程定义
          */
         void refresh();
         /**
          *刷新载读流程定义根据流程ID
          */
         PProcessDefinition refreshDefinitionById(const string & id);
         PProcessDefinition refreshDefinitionByName(const string & id);
         /**
          * 获得到流程定义根据流程ID
          */
         PProcessDefinition getDefinitionById(const string & id);
         PProcessDefinition getDefinitionByName(const string & name);
                  
         void setHandleFactory(PHandleFactory  factory);
      
    protected:
         /**
          * 加载所有的流程定义, 流程定义由客户程序负责销毁
          * @return 流程定义集合
          */
         virtual void load(PHandleFactory  factory) = 0;       
    };
    
    typedef counted_ptr <DefinitionLoaderDao> PDefinitionLoaderDao;
  
    
}; //namespace ebpm4c
#endif //EBPM4C_H

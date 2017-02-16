#ifndef DELEGATION_H
#define DELEGATION_H
#include "base/config-all.h"
#include <string>
#include "base/SmartPtr.h"

USING_NAMESPACE(std);

namespace ebpm4c { 
	
	class Context;     
    
    /** 回调句柄对象 */
    class Handle {
    public:
        /**
         * 通过字符串配置句柄
         * @param configuration 配置串
         * @exception execution_error 执行期间出现异常
         * @exception difinition_error 配置串定义不正确
         */
        virtual void configure(const string & configuration) = 0;

        virtual ~Handle() {
        }
    };


    typedef counted_ptr < Handle > PHandle;

    /** 外部动作调用句柄 */
    class ActionHandle : public Handle {
    public:
        /**
         * 执行一个动作
         * @param ctx 流程的上下文
         * @exception execution_error 执行期间出现异常
         */
        virtual void execute(Context & ctx) = 0;

        virtual ~ActionHandle() {
        }
    };


    typedef counted_ptr < ActionHandle > PActionHandle;

    /** 外部判断的调用句柄 */
    class DecisionHandle : public Handle {
    public:
        /**
         * 执行一个判断
         * @param 流程的上下文
         * @return 转移的名称
         * @exception execution_error 执行期间出现异常
         */
        virtual string decide(Context & ctx) = 0;

        virtual ~DecisionHandle() {
        }
    };


    typedef counted_ptr < DecisionHandle > PDecisionHandle;


   class ProcessInvocationHandle : public Handle {
 
     public:
        /**
         * 启动一个子流程
         * @param 流程的上下文
         * @return 转移的名称
         * @exception execution_error 执行期间出现异常
         */
        virtual void startSubProcess( Context& ctx ) = 0;

        virtual ~ProcessInvocationHandle(){
        }       
   };
   typedef counted_ptr < ProcessInvocationHandle > PProcessInvocationHandle;

    /** 句柄工厂 */
    class HandleFactory {
    public:
        /**
         * 创建一个句柄, 生命周期有客户负责(客户程序负责delete该对象)
         * @param 句柄的类型名称
         * @return 句柄
         * @exception execution_error 执行期间出现异常
         */
        virtual counted_ptr < Handle > create(const string & className) = 0;
    };
   typedef counted_ptr < HandleFactory > PHandleFactory; 
}; //namesapce ebpm4c

#endif //DELEGATION_H

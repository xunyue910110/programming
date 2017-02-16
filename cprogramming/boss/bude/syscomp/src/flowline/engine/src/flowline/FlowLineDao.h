#ifndef _FLOWLINEDAO_H_
#define _FLOWLINEDAO_H_
#include "flowline/flbase.h"
#include "flowline/Definition.h"
#include "flowline/adapter/AdapterFactory.h"
#include "flowline/Resource.h"
#include "flowline/SystemInfo.h"
#include "flowline/Metadata.h"
#include <vector>

USING_NAMESPACE(std);
class Properties;
using namespace flowline::adapter;
namespace flowline{
	
/**
 * FlowLine访问持久数据服务接口
 */ 	
class FlowLineDao {
public:	
	/**
	 * 初始化
	 */ 
	virtual void init(const Properties* env)  = 0;
	
	/**
	 * 设置工厂
	 */ 
    virtual void setAdapterFactory( AdapterFactory* factory ) = 0;
    /**
     * 外部不需要释放, 由dao管理.
     */
    virtual Definition& getDefinition( const string name ) = 0;
	
	/**
	 * 取得下个流水号
	 */ 
    virtual int nextId( const string& key ) = 0;
	/**
	 * 记录执行日志
	 * @param cycleLogId 周期日志ID
	 * @param batchLogId 批处理日志ID
	 * @param executor 执行体
	 * @param begin 开始执行时间
	 * @param end 执行结束时间
	 * @return 日志的ID
	 */ 
    virtual int logExecution( int cycleLogId ,int batchLogId, Executor& executor, 
    	time_t begin, time_t end )  = 0;

	/**
	 * 记录执行日志, 带有输入输出
	 * @param cycleLogId 周期日志ID
	 * @param batchLogId 批处理日志ID
	 * @param executor 执行体
	 * @param inputs 输入<资源, 处理的毫秒数>
	 * @param outputs 输出
	 * @param begin 开始执行时间
	 * @param end 执行结束时间
	 * @return 日志的ID
	 */ 
    virtual int logExecution( int cycleLogId , int batchLogId, Executor& executor, 
    	vector<pair<PResource, int> >& inputs, vector<PResource>& outputs, 
    	time_t begin, time_t end )  = 0;
    	
    /**
     * 标记已经处理过的资源
     * @param executor 执行体
     * @param resource 需要标记的资源, 如果已经处理过, 在pair的second置为true, 否则false
     * @param after 从当前时间完全多长时间之内查询 >= current - ago
     */ 	
    virtual void signProcessedResources(Executor& executor, 
    	vector<pair<PResource, bool> >& resources, time_t ago) = 0;
    	
    /**
     * 获取资源定义
     */ 
    virtual const Metadata& getMetadata(int id) = 0;
    /**
     * 查询主机
     */ 
	virtual const Host& getHost(int accessId) = 0; 
	/**
	 * 查询数据库
	 */ 
	virtual const Database& getDatabase(int accessId) = 0;
	/**
	 * 记录开始一次批处理
	 */ 
    virtual int logOpenBatch( int cycleLogId ) = 0;
	/**
	 * 记录结束批处理
	 */ 
    virtual void logCloseBatchs( int cycleLogId ) = 0;
    /**
     * 记录开始一个周期
     * @return 返回周期日志的ID
     */ 
    virtual int logOpenCycle( Definition& def, const Cycle::Instance& ci ) = 0;
	/**
	 * 记录关闭一个周期
	 */ 
    virtual void logCloseCycle( int cycleLogId ) = 0;
    
    virtual bool hasPassed(int cycleLogId, int checkerId) = 0;
    
    virtual void logPassed(int cycleLogId, int checkerId) = 0;

	/**
	 * 获取未完成的周期
	 * @param def 流程定义(输入)
	 * @param logid_cycleinstance 周期日志ID和周期实例信息的vector(输出)
	 */ 
    virtual void getUnfinishedCycleInstances( Definition& def, 
    	vector<pair<int, Cycle::Instance> >& logid_cycleinstance )  = 0;
};
}
#endif //_FLOWLINEDAO_H_

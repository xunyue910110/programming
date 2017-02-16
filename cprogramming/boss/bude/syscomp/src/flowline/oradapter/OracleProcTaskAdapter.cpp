#include "config-all.h"
#include "flowline/Context.h"
#include "flowline/adapter/TaskAdapter.h"
#include "flowline/flbase.h"
#include "flowline/FlowLineDao.h"
#include "base/OcciUtil.h"
#include "base/ClassRegister.h"
#include "base/StringUtil.h"
#include "flowline/FlowLineException.h"
#include "log4cpp/Logger.h"

#include "OracleProcCaller.h"

#include <map>
#include <string>
#include <vector>

using namespace flowline;
using namespace flowline::adapter;

USING_NAMESPACE(std);

namespace flowline {
namespace oradapter {
	
class OracleProcTaskAdapter : public flowline::adapter::TaskAdapter
{
	Environment* m_env;
	Connection* m_connection;
	const Database* m_database;
	string m_parameters;
	string m_functionName;
	string m_dateformat;
	
public:
	ProcedureTaskAdapter(){
		m_env =0;
		m_connection =0;		
	};
	       
    virtual void init(FlowLineDao& dao) {    	
        m_functionName = getTarget()->getExtendedAttribute( "functionName");		
		m_parameters = getTarget()->getExtendedAttribute( "parameters");
		m_dateformat = getTarget()->getExtendedAttribute( "dateformat" );
		
		//初始化连接
		string id = getTarget()->getExtendedAttribute( "accessId" );
        m_database = &dao.getDatabase( atoi( id.c_str() ) );//得到表中 td_fl_sys_access
    
        m_env = Environment::createEnvironment ( Environment::DEFAULT );
        FL_TRACE_P2("连接数据库DB=%s,USER=%s", m_database->name.c_str(), m_database->username.c_str());
        m_connection = m_env->createConnection ( m_database->username, m_database->password, m_database->name );
       
        static const string sqlAlter = "alter session set nls_date_format ='YYYYMMDDHH24MISS'";
        StatementVar stmt( m_connection, sqlAlter );	
        stmt->executeUpdate();
        //catch(exception & e){}
        //catch(...){}
        //估计是用来检验连接的
    }
    
    //日期的转换
    string getOptionDate(Context & ctx,const string& dateformat){
    	//日期的转换以后做
    	//或者日期转换在数据库里面完成
        return dateformat;
    };
    
	/**
     * 执行一个动作
     * @param ctx 流程的上下文
     * @exception execution_error 执行期间出现异常
     */
    virtual void execute(Context & ctx){
     
       int resultcode = 0;
       string resultinfo;
       
       string opdate = getOptionDate(ctx,m_dateformat);
       
       OracleProcCaller pcall(m_connection);
       int up = pcall.callProcedure(m_functionName,m_parameters,opdate,resultcode,resultinfo);
       
       if(resultcode != 0){
          	FL_TRACE_P2("存储过程执行业务异常:%d,%s",resultcode,resultinfo.c_str()); 
			THROW_C(ExecutionException,resultcode,resultinfo);       
       }
       FL_TRACE_P1("存储过程执行成功:%s",resultinfo);       
    }
    
  
};//class

REGISTER_CLASS("SP_ExecuteTaskAdapter", OracleProcTaskAdapter, Adapter);

};//name
};//name


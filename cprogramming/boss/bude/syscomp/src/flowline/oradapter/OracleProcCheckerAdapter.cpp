#include "config-all.h"
#include "flowline/Context.h"
#include "flowline/adapter/CheckerAdapter.h"
#include "flowline/flbase.h"
#include "flowline/FlowLineDao.h"
#include "base/OcciUtil.h"
#include "base/ClassRegister.h"
#include "base/StringUtil.h"
#include "flowline/FlowLineException.h"
#include "log4cpp/Logger.h"

#include "OracleProcCaller.h"

namespace flowline {
namespace oradapter {
class OracleProcCheckerAdapter: public flowline::adapter::CheckerAdapter
{
private :
	Environment* m_env;
	Connection* m_connection;
	const Database* m_database;
	string m_parameters;
	string m_functionName;
	string m_dateformat;
public:
	OracleProcCheckerAdapter(){};
	
	void init(FlowLineDao& dao) {    	
        m_functionName = getTarget()->getExtendedAttribute( "functionName");		
		m_parameters = getTarget()->getExtendedAttribute( "parameters");
		m_dateformat = getTarget()->getExtendedAttribute( "dateformat" );
		
		//初始化连接
		string id = getTarget()->getExtendedAttribute( "accessId" );
        m_database = &dao.getDatabase( atoi( id.c_str() ) );

        m_env = Environment::createEnvironment ( Environment::DEFAULT );
        FL_TRACE_P2("连接数据库DB=%s,USER=%s", m_database->name.c_str(), m_database->username.c_str());
        m_connection = m_env->createConnection ( m_database->username, m_database->password, m_database->name );

        static const string sqlAlter = "alter session set nls_date_format ='YYYYMMDDHH24MISS'";
        StatementVar stmt( m_connection, sqlAlter );	
        stmt->executeUpdate();
        //估计是用来检验连接的
    }
    //转换时间
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
    bool check(Context & ctx){  
       init(ctx.getFlowLineDao());//////
       int resultcode = 0;
       string resultinfo="TradeError!";
       
       string opdate = this->getOptionDate(ctx,m_dateformat);
       FL_TRACE_P3("存储过程参数:m_functionName=%s,m_parameters=%s,opdate=%s",m_functionName.c_str(),m_parameters.c_str(),opdate.c_str());
       OracleProcCaller pcall(m_connection);
       int up = pcall.callProcedure(m_functionName,m_parameters,opdate,resultcode,resultinfo);
       
       if(resultcode != 0){
          	FL_TRACE_P2("存储过程检查执行完成，检查不通过，返回:%d,%s",resultcode,resultinfo.c_str()); 			
       }else{
			FL_TRACE_P1("存储过程检查执行成功:%s",resultinfo);
       }
       if(resultcode != 0){
            return false;
       }
       return true;
    }
};
REGISTER_CLASS("SP_CheckerAdapter", OracleProcCheckerAdapter, Adapter);
};  
    
};

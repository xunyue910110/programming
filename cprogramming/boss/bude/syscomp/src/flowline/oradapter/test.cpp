#include "base/OcciUtil.h"

int main() {
	//TODO 以后从连接中获取连接
    Environment* m_env = Environment::createEnvironment ( Environment::DEFAULT );
    Connection* m_conn = m_env->createConnection ( "budedev", "budedev123", "boss15" );
    static const string sqlAlter = "alter session set nls_date_format ='YYYYMMDDHH24MISS'";
    StatementVar stmt( m_conn, sqlAlter );
    stmt->executeUpdate();
    return 0;
}

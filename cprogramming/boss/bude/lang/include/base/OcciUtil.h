#ifndef _OCCIUTIL_H_
#define _OCCIUTIL_H_
#include "config-all.h"
#include <occi.h>
USING_NAMESPACE(std);
using namespace oracle::occi;
typedef Environment *EnvironmentPtr;
typedef Connection* ConnectionPtr;
typedef Statement* StatmentPtr;
typedef ResultSet* ResultSetPtr;



///////////////////////////////////////////////////////////////////////////
// StatementVar
///////////////////////////////////////////////////////////////////////////
class StatementVar {
    StatmentPtr m_statment;
    ConnectionPtr m_conn;
public:
    StatementVar( ConnectionPtr conn, const string& sql = "" ) : m_conn( conn ) {
#ifdef DEBUG_SQL
	cout << sql << endl;
#endif
        m_statment = conn->createStatement( sql );
	m_statment->setAutoCommit(false);
    }
    ~StatementVar() {
        m_conn->terminateStatement ( m_statment );
    }

    Statement& operator*() {
        return * m_statment;
    }

    StatmentPtr operator->() {
        return m_statment;
    }
    StatmentPtr get() {
        return m_statment;
    }
    operator bool () {
        return m_statment != 0;
    }
};

///////////////////////////////////////////////////////////////////////////
// ResultSetVar
///////////////////////////////////////////////////////////////////////////
class ResultSetVar {
    StatmentPtr m_statment;
    ResultSetPtr m_resultSet;
public:
    ResultSetVar( StatementVar& statment ) : m_statment( statment.get() ) {
        m_resultSet = statment->executeQuery ();
    }

    ~ResultSetVar() {
        m_statment->closeResultSet ( m_resultSet );
    }

    ResultSet& operator*() {
        return * m_resultSet;
    }

    ResultSetPtr operator->() {
        return m_resultSet;
    }
    ResultSetPtr get() {
        return m_resultSet;
    }
    operator bool () {
        return m_resultSet != 0;
    }
};


#endif //_OCCIUTIL_H_

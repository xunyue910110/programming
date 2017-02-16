#include <vector>
#include "Session.h"
#include "DataBase.h"
#include "MemManager.h"
#include "base/supdebug.h"
#include "common/InputParams.h"
#include "base/SysParam.h"

SysParam *pSysParam;

string    C;
string    name;
string    value;
#define GET_PARAM(X,Y)  name = Y; if (!pSysParam->getValue(X, name, value)) { cout << "pSysParam->getValue err!"; return -1; };

const string CONFIG_FILE_NAME="etc.cfg";

bool creatTable(Session *pSession)
{
	TableDef  t_tableDef;
	sprintf(t_tableDef.m_tableName,"%s","USER_LOCK");
	t_tableDef.m_tableType = DATA_TABLE;
	t_tableDef.m_columnNum = 3;
	sprintf(t_tableDef.m_columnList[0].m_name,"%s","USERID");
	t_tableDef.m_columnList[0].m_type=VAR_TYPE_LONG;
	t_tableDef.m_columnList[0].m_len = 8;
	
	sprintf(t_tableDef.m_columnList[1].m_name,"%s","TYPE");
	t_tableDef.m_columnList[1].m_type=VAR_TYPE_INT;
	t_tableDef.m_columnList[1].m_len = 4;
	
	sprintf(t_tableDef.m_columnList[2].m_name,"%s","CHANNELNO");
	t_tableDef.m_columnList[2].m_type=VAR_TYPE_INT;
	t_tableDef.m_columnList[2].m_len=4;
	
	t_tableDef.m_spaceNum=1;
	strcpy(t_tableDef.m_spaceList[0],"DataSpace01");
	//strcpy(t_tableDef.m_spaceList[1],"DataSpace02");
	t_tableDef.m_keyFlag=1;
	t_tableDef.m_keyClumnNum=1;
	sprintf(t_tableDef.m_keyColumnList[0],"%s","userId");
	t_tableDef.m_lockType=1;
	t_tableDef.m_logType=1;	
	
	pSession->createTable(t_tableDef);
	
	
	return true;
}

bool creatIndex(Session *pSession)
{
	
	IndexDef  t_indexDef;
	sprintf(t_indexDef.m_indexName,"%s","IDX_USER_LOCK");
	t_indexDef.m_indexType = HASH_INDEX;
	t_indexDef.m_hashSize  = 1000;
	sprintf(t_indexDef.m_tableName,"%s","USER_LOCK");
	t_indexDef.m_columnNum = 2;
	sprintf(t_indexDef.m_columnList[0],"%s","USERID");
	sprintf(t_indexDef.m_columnList[1],"%s","TYPE");
	
	t_indexDef.m_spaceNum=1;
	strcpy(t_indexDef.m_spaceList[0],"DataSpace01");
	t_indexDef.m_isUnique=1;	
	
	pSession->createIndex(t_indexDef);
	
	return true;	
}


int main()
{
	DataBase *pDB = NULL;
	Session *pSession = NULL;
	MemManager *pMemMgr = NULL;
	
	try
	{ 
		pDB = new DataBase("billing");
		//pDB->start();
		cout << "new DataBase(\"billing\");" << endl;
		
	    pSession = pDB->connect();
		
		cout << "pDB->connect();" << endl;
		
		//creatIndex(pSession);
		//creatIndex1(pSession);
		//creatIndex2(pSession);
		//creatIndex3(pSession);
		//creatIndex4(pSession);
		//creatIndex5(pSession);
		
		//pSession->dropTable("LOCK_tf_f_feepolicy");
		cout << "1" << endl;
		
		//pSession->dropTable("tf_f_feepolicy");
		cout << "2" << endl;
		
		pDB->disconnect(pSession);
	}
	catch(Mdb_Exception e)
	{
		e.toString();
		if( pDB != NULL )
		{
			if( pSession != NULL )
			{
				pDB->disconnect(pSession);
			}
			delete pDB;
		}
	}
	
	return 0;
}

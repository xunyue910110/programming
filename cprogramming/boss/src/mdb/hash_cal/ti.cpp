
#include "Session.h"
#include "DataBase.h"
#include "InputParams.h"
#include "base/SysParam.h"
#include "InstanceFactory.h"
#include "MdbConstDef.h"
#include "base/supdebug.h"
#include "Table.h"
#include "IndexHash.h"
#include "MemManager.h"

DataBase *pDB = NULL;
Session *pSession = NULL;
string g_dbName;

void showTableName(vector<TableDesc>& t_tableDescList)
{
	int j=0;
	cout<<"----------------------------------"<<endl;
	cout<<"["<<g_dbName<<"]下的所有表:"<<endl;
	for (int i=0;i<t_tableDescList.size();i++)
	{
		if (strncmp(t_tableDescList[i].m_tableDef.m_tableName,"LOCK_",5) == 0)
			continue;
		j++;
		printf("%-30s",t_tableDescList[i].m_tableDef.m_tableName);
		if (j % 4 == 0)
			printf("\n");
		//cout<<t_tableDescList[i].m_tableDef.m_tableName<<endl;
	}
	printf("\n");
}
int showIndexName(vector<TableDesc>& t_tableDescList,const string& t_tableName)
{
	int pos = -1;
	cout<<"----------------------------------"<<endl;
	cout<<"表:"<<t_tableName<<" 的所有索引:"<<endl;
	for (int i=0;i<t_tableDescList.size();i++)
	{
		if (strncmp(t_tableDescList[i].m_tableDef.m_tableName,"LOCK_",5) == 0)
			continue;
		if (strcasecmp(t_tableDescList[i].m_tableDef.m_tableName,t_tableName.c_str()) == 0)
		{
			pos=i;
			for (int j=0;j<t_tableDescList[i].m_indexNum;j++)
			{
				cout<<"   "<<t_tableDescList[i].m_indexNameList[j]<<endl;
			}
			break;
		}
	}
	return pos;
}
bool showHash()
{
	MemManager * pMemMgr=pDB->getMemMgrInstance();
	if (pMemMgr == NULL) return false;
	
	vector<TableDesc> t_tableDescList;
	pMemMgr->getTableDescList(t_tableDescList);
	while (1)
	{
		showTableName(t_tableDescList);
		cout<<"请输入表名(Q-退出):";
		string tableName;
		cin>>tableName;
		if (tableName[0]=='Q' || tableName[0]=='q')
			break;
		Table *table=pDB->findTableByName(tableName.c_str());
		if (table == NULL) 
		{
			cout<<"无效的表名！"<<endl;
			continue;
		}
		int pos=showIndexName(t_tableDescList,tableName);
		if (pos == -1) continue;

		for (int j=0;j<t_tableDescList[pos].m_indexNum;j++)
		{
			string indexName=t_tableDescList[pos].m_indexNameList[j];
			IndexDesc * indexDesc;
			pMemMgr->getIndexDescByName(indexName.c_str(),indexDesc);
			
			
			RecordConvert * convert=new RecordConvert(&t_tableDescList[pos]);
			IndexHash * index=new IndexHash(tableName.c_str(),indexName.c_str(),pMemMgr,convert);	
			if (index == NULL) 
			{
				delete convert;
				return false;
			}
			index->initialization();
			int iHashSize,iUsedHash,iRecordCount;
			map<int,int> mHashCal;
			index->getHashPercent(iHashSize,iUsedHash,iRecordCount,mHashCal);
			cout<<"--------- "<<indexName<<" ----------"<<endl;
			if (indexDesc->m_indexDef.m_isUnique)
				cout<<"[唯一索引]"<<endl;
			cout<<"[索引字段]  :";
			for (int k=0;k<indexDesc->m_indexDef.m_columnNum;k++)
			{
				cout<<indexDesc->m_indexDef.m_columnList[k];
				if (k != indexDesc->m_indexDef.m_columnNum - 1)
					cout<<",";
				else
					cout<<endl;
			}
			cout<<"Hash桶总数  :"<<iHashSize<<endl;
			cout<<"已使用Hash桶:"<<iUsedHash<<endl;
			cout<<"记录总数    :"<<iRecordCount<<endl;
			if (iRecordCount>0)
			{
				printf("Hash命中率  :%.2f%%\n\n",((float)iUsedHash/(float)iRecordCount)*100);
				cout<<"Hash桶内元素个数 该类型Hash桶的个数"<<endl;
				map<int,int>::iterator t_it=mHashCal.begin();
				for (;t_it != mHashCal.end();t_it++)
				{
					printf("%16d %18d\n",t_it->first,t_it->second);
				}
				printf("\n");
			}
			delete index;
			delete convert;
		}
	}
	return true;	
}
bool getDb()
{
	try
	{
		pDB = InstanceFactory::getDBInstance(MDB
			                                ,g_dbName.c_str()
			                                ,NULL
			                                ,0
			                                ,NULL
			                                ,NULL);
		if (pDB == NULL)
		{
			cout<<"get DBInstance "<<g_dbName<<" failed!"<<endl;
			return false;
		}
	}
	catch (Mdb_Exception &e)
	{
		e.toString();
		return false;
	}
	return true;	
}
bool connect()
{
	try
	{
		if (pSession == NULL)
		{
			pSession=pDB->connect();
			if (pSession == NULL)
			{
				cout<<"connect database failed!"<<endl;
				return false;
			}
		}
	}
	catch (Mdb_Exception & e)
	{
		e.toString();
		return false;
	}
	return true;
}
bool disconnect()
{
	try
	{
		if (pSession != NULL)
		{
			pDB->disconnect(pSession);
			pSession=NULL;
		}
	}
	catch (Mdb_Exception & e)
	{
		e.toString();
		return false;
	}
	return true;
}

int main(int argc,char **argv)
{
	cout<<"============== 内存数据库Hash索引分析 =============="<<endl;
	if (argc == 1)
	{
		cout<<"使用格式:ti 内存数据库名称"<<endl;
		cout<<"当前内存数据库:billing"<<endl;
		g_dbName="billing";
	}
	else
	{
		g_dbName=argv[1];
	}
	if (getDb())//获得DBInstance
	{
		if (connect())//获得Session
		{
			showHash();
		}
		disconnect();//断开连接，注销session
		
	}
	delete pDB;//删除数据库实例指针
	return 0;
}

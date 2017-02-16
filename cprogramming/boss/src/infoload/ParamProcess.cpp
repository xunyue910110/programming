#include "ParamProcess.h"
#include "DbInterface.h"
#include "MmInterface.h"
#include "appfrm/ErrorHandle.h"

ParamProcess::ParamProcess()
{
	m_srcdbi = NULL;
	m_objmmis = NULL;
}
		
		
ParamProcess::~ParamProcess()
{
}

		
void ParamProcess::init(DbInterface *srcdbi, vector<MmInterface*> *objmmis)
{
	m_srcdbi = srcdbi;
	m_objmmis = objmmis;
}

		
void ParamProcess::process(const char& dealtype, const string& selectsql, const string& tablename, 
					 	   const string& updatesql, const string& starttime, const string& endtime)
{
	int recnum = 0;
	
	//(1)查询记录
	try
	{
		//载入方式,0:先TRUNCATE,然后全部数据INSERT进入。
		if (dealtype=='0' || dealtype=='2')
		{		
			m_srcdbi->execStaticSql(selectsql);
		}
		//载入方式,1:只做UPDATE和INSERT操作，已有数据UPDATE，没有的数据INSERT。
		else
		{
			vector<DbParam> params;
			params.clear();
			DbParam param;
			param.m_seq = 1;
			param.m_action = PARAM_ACT_IN;
			param.m_type = PARAM_TYPE_DATE;
			strcpy(param.m_dateValue, starttime.c_str());
			params.push_back(param);
			param.m_seq = 2;
			param.m_action = PARAM_ACT_IN;
			param.m_type = PARAM_TYPE_DATE;
			strcpy(param.m_dateValue, endtime.c_str());
			params.push_back(param);
			
			m_srcdbi->execDynamicSql(selectsql, params);
		}
	}	
	catch (CException e)
	{
		throw e;
	}	

	//(2)执行清理+插入
	//(2.1)载入方式,0:先TRUNCATE,然后全部数据INSERT进入。
	if (dealtype=='0' || dealtype=='2')
	{
		try
		{
			//(2.1.1)完全清空,全表锁定
			for (vector<MmInterface*>::iterator iter=m_objmmis->begin(); iter!=m_objmmis->end(); ++iter)
			{
				if(dealtype != '2')
				{
					(*iter)->truncate(tablename);
				}
				(*iter)->beginTrans(tablename, 0);
			}
			
			//(2.1.2)记录插入内存数据库
			vector<string> record;
			while(m_srcdbi->getNext())
			{
				++recnum;
				
				//获取一行原始记录
				record.clear();
				for (int i=1; i<=m_srcdbi->getColumnNum(); ++i)
				{
					record.push_back(m_srcdbi->getColumnStringValue(i));
        }
			
				//记录插入内存数据库中
				for (vector<MmInterface*>::iterator iter=m_objmmis->begin(); iter!=m_objmmis->end(); ++iter)
				{
						(*iter)->insert(tablename, record);
				}
				
			   //记录数超过默认值,提交
				if (recnum%MAX_RECCOMMIT_NUM == 0)
				{
					for (vector<MmInterface*>::iterator iter=m_objmmis->begin(); iter!=m_objmmis->end(); ++iter)
					{	
						try
						{
							(*iter)->commitTrans();
						}
						catch (CException e)
				    {
							(*iter)->rollbackTrans();	
							(*iter)->beginTrans(tablename, 0);				
//							m_theErrorMessages->insert(DB_ERROR_NOCFG, "主键重复|"+record[0]+"|"+record[1]+"|"+record[2]+"|请核实记录");
							continue;
				    }
						(*iter)->beginTrans(tablename, 0);
					}
				}
			}

			//(2.1.3)提交内存数据库
			for (vector<MmInterface*>::iterator iter=m_objmmis->begin(); iter!=m_objmmis->end(); ++iter)
			{
				try
				{
					(*iter)->commitTrans();
				}
				catch (CException e)
				{
					(*iter)->rollbackTrans();	
//					m_theErrorMessages->insert(DB_ERROR_NOCFG, "主键重复|"+record[0]+"|"+record[1]+"|"+record[2]+"|请核实记录");
				}
			}
		}
		catch (CException e)
		{
			for (vector<MmInterface*>::iterator iter=m_objmmis->begin(); iter!=m_objmmis->end(); ++iter)
			{
				(*iter)->rollbackTrans();
			}
			throw e;
		}
	}
	//(2.2)更新+插入(注意：查询的SQL字段必须与更新＋插入的需要的字段顺序完全一致)
	else
	{		
		try
		{
			//(2.2.1)表记录事务开启
			for (vector<MmInterface*>::iterator iter=m_objmmis->begin(); iter!=m_objmmis->end(); ++iter)
			{
				(*iter)->beginTrans(tablename, 1);
			}
			
			//(2.2.2)记录插入内存数据库
			vector<string> record;
			while(m_srcdbi->getNext())
			{
				++recnum;
				
				//获取一行原始记录
				record.clear();
				for (int i=1; i<=m_srcdbi->getColumnNum(); ++i)
				{
					record.push_back(m_srcdbi->getColumnToStringValue(i));
				}
				
				//记录插入内存数据库中
				for (vector<MmInterface*>::iterator iter=m_objmmis->begin(); iter!=m_objmmis->end(); ++iter)
				{
					//如果没有更新到数据,那么插入数据
					if ((*iter)->update(tablename, updatesql, record) == 0)
					{
						(*iter)->insert(tablename, record);
					}
				}
				
				//记录数超过默认值,提交
				if (recnum%MAX_RECCOMMIT_NUM == 0)
				{
					for (vector<MmInterface*>::iterator iter=m_objmmis->begin(); iter!=m_objmmis->end(); ++iter)
					{	
						(*iter)->commitTrans();
						(*iter)->beginTrans(tablename, 1);
					}
				}
			}
			
			//(2.2.3)提交内存数据库
			for (vector<MmInterface*>::iterator iter=m_objmmis->begin(); iter!=m_objmmis->end(); ++iter)
			{
				(*iter)->commitTrans();
			}
		}
		catch (CException e)
		{
			for (vector<MmInterface*>::iterator iter=m_objmmis->begin(); iter!=m_objmmis->end(); ++iter)
			{
				(*iter)->rollbackTrans();
			}
			throw e;
		}
	
	}
}

